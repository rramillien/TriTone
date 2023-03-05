#include "oscillator.hpp"

#include "../application.hpp"

#include <q/synth/sin.hpp>
#include <q/synth/saw.hpp>

using namespace cycfi;

namespace live::tritone::vie::processor::module
{
	oscillator::oscillator(nlohmann::json oscillator_definition) : processor_module(),
		id_(oscillator_definition["id"]),
		name_(oscillator_definition["name"]),
		sample_rate_(.0),
		current_phases_descriptors_(new std::unordered_map<uint_fast16_t, phase_descriptor>),
		next_phases_descriptors_(new std::unordered_map<uint_fast16_t, phase_descriptor>),
		can_process_(false),
		nb_outputs_(0)
	{
		auto& parameters_definition = oscillator_definition["parameters"];

		if (parameters_definition != nullptr) {
			type_ = parameters_definition["type"];
			if (type_ == "saw")
			{
				signal_type_ = signal_type::saw;
			}
			else {
				signal_type_ = signal_type::sin;
			}
		}
		else {
			signal_type_ = signal_type::sin;
		}
		
		for (int i = 0; i < 32; i++) {
			outputs_[i] = new float_array_module_output();
		}
	}

	oscillator::~oscillator()
	{
		for (int i = 0; i < 32; i++) {
			delete[] outputs_[i]->values.values;
			delete outputs_[i];
		}

		delete current_phases_descriptors_;
		delete next_phases_descriptors_;
	}

	uint16_t oscillator::get_id()
	{
		return id_;
	}

	std::string oscillator::get_name()
	{
		return name_;
	}

	processor_module_type oscillator::get_type()
	{
		return processor_module_type::middle;
	}

	void oscillator::set_sample_rate(const double sample_rate)
	{
		sample_rate_ = sample_rate;
	}

	void oscillator::preprocess() {
	}

	bool oscillator::can_process()
	{
		return can_process_;
	}

	void oscillator::process(output_process_data& output_process_data)
	{
		can_process_ = false;

		nb_outputs_ = 0;
		for (auto& [note_id, phase_descriptor] : *current_phases_descriptors_)
		{
			float_array_module_output* output = outputs_[nb_outputs_];

			//If nb of samples is greater than the ones currently allocated, reallocate.
			if (output_process_data.num_samples > output->values.nb_values)
			{
				if (output->values.nb_values > 0)
				{
					delete[] output->values.values;
				}
				output->values.values = new float[output_process_data.num_samples];
				output->values.nb_values = output_process_data.num_samples;
			}

			output->note_id = note_id;
			switch (signal_type_)
			{
			case signal_type::sin:
				for (uint_fast32_t frame = 0; frame < output_process_data.num_samples; frame++)
				{
					output->values.values[frame] = q::sin(phase_descriptor.phase_iterator);
					++phase_descriptor.phase_iterator;
				}
				break;
			case signal_type::saw:
				for (uint_fast32_t frame = 0; frame < output_process_data.num_samples; frame++)
				{
					output->values.values[frame] = q::saw(phase_descriptor.phase_iterator);
					++phase_descriptor.phase_iterator;
				}
				break;
			}
			nb_outputs_++;
		}
	}

	uint_fast8_t oscillator::get_output_values(const uint_fast16_t slot_id, std::array<module_output*, 32>& values)
	{
		switch (slot_id)
		{
		case amplitudes_output_id:
			values = reinterpret_cast<std::array<module_output*, 32>&>(outputs_);
			return nb_outputs_;
		}
		
		throw std::invalid_argument("Invalid slot id");
	}

	bool oscillator::has_finished()
	{
		return true;
	}

	uint_fast16_t oscillator::get_slot_id(const std::string& slot_name)
	{
		if (slot_name == onoff_input_name)
		{
			return onoff_input_id;
		}
		else if (slot_name == frequency_input_name)
		{
			return frequency_input_id;
		}
		else if (slot_name == signal_type_input_name)
		{
			return signal_type_input_id;
		}
		else if (slot_name == amplitudes_output_name)
		{
			return amplitudes_output_id;
		}

		throw std::invalid_argument("Invalid slot name");
	}

	void oscillator::set_input_values(const uint_fast16_t slot_id, std::array<module_output*, 32>& values, uint_fast8_t nb_values)
	{
		switch (slot_id)
		{
		case frequency_input_id:
			assert(nb_values <= 32);
			for (uint_fast16_t i = 0; i < nb_values; i++)
			{
				auto module_values = values[i];

				const auto note_id = module_values->note_id;

				if (const auto current_phase_descriptor = current_phases_descriptors_->find(note_id); current_phase_descriptor != current_phases_descriptors_->end())
				{
					//If frequency was already set before, we don't want to erase its phase.
					//So, keep the old one.
					phase_descriptor& phase_descriptor = next_phases_descriptors_->operator[](note_id);
					phase_descriptor.phase_iterator = current_phase_descriptor->second.phase_iterator;
				}
				else
				{
					//If frequency is a new one, create a phase for it.
					const auto raw_frequency = module_values->to_float();
					const auto frequency = q::frequency(static_cast<double>(raw_frequency));

					phase_descriptor& phase_descriptor = next_phases_descriptors_->operator[](note_id);
					phase_descriptor.phase_iterator = q::phase_iterator(frequency, static_cast<std::uint32_t>(sample_rate_));
				}
			}

			{
				//Switching between current and next phases is done to simplify the deletion of phases no more used.
				std::unordered_map<uint_fast16_t, phase_descriptor>* tmp_phases_descriptors = current_phases_descriptors_;
				current_phases_descriptors_ = next_phases_descriptors_;
				next_phases_descriptors_ = tmp_phases_descriptors;
			}

			//Clear the phases for next call.
			next_phases_descriptors_->clear();

			can_process_ = true;
			return;
		case signal_type_input_id:
			const auto value = values[0]->to_boolean();
			if (value)
			{
				signal_type_ = signal_type::saw;
			}
			else
			{
				signal_type_ = signal_type::sin;
			}
			return;
		}
		
		throw std::invalid_argument("Invalid slot name");
	}

	uint_fast32_t oscillator::get_max_nb_input_values(const uint_fast16_t slot_id)
	{
		if (slot_id == frequency_input_id)
		{
			return 32;
		}

		throw std::invalid_argument("Invalid slot name");
	}

	void oscillator::set_parameter(parameter parameter)
	{

	}

	nlohmann::json oscillator::serialize()
	{
		nlohmann::json root;
		root["id"] = id_;
		root["name"] = name_;

		return root;
	}
} // namespace
