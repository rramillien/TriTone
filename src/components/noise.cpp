#include "noise.hpp"

#include "../application.hpp"

#include <q/synth/noise.hpp>

using namespace cycfi;
using namespace q::literals;

namespace live::tritone::vie::processor::component
{
	noise::noise(nlohmann::json noise_definition) :
		id_(noise_definition["id"]),
		name_(noise_definition["name"]),
		current_noises_descriptors_(new std::unordered_map<uint_fast16_t, noise_descriptor<void>>),
		next_noises_descriptors_(new std::unordered_map<uint_fast16_t, noise_descriptor<void>>),
		sample_rate_(0.0),
		noise_type_(noise_type::white),
		can_process_(false),
		nb_outputs_(0)
	{
	}

	noise::~noise()
	{
		for (uint_fast32_t i = 0; i < nb_outputs_; i++)
		{
			if (const float_array_component_output output = outputs_[nb_outputs_]; output.nb_samples > 0)
			{
				delete output.values;
			}
		}
	}

	uint16_t noise::get_id()
	{
		return id_;
	}

	std::string noise::get_name()
	{
		return name_;
	}

	processor_component_type noise::get_type()
	{
		return processor_component_type::middle;
	}

	void noise::set_sample_rate(const double sample_rate)
	{
		sample_rate_ = sample_rate;
	}

	//FIXME: Can't know if all inputs are set-up. So, we should check note by note if it can be processed. 
	bool noise::can_process()
	{
		return can_process_;
	}

	void noise::process(output_process_data& output_process_data)
	{
		can_process_ = false;

		nb_outputs_ = 0;
		for (auto& [note_id, noise_descriptor] : *current_noises_descriptors_)
		{
			float_array_component_output& output = outputs_[nb_outputs_];

			//If nb of samples is greater than the ones currently allocated, reallocate.
			if (output_process_data.num_samples > output.nb_samples)
			{
				if (output.nb_samples > 0)
				{
					delete output.values;
				}
				output.values = new float[output_process_data.num_samples];
				output.nb_samples = output_process_data.num_samples;
			}

			output.output_id = note_id;
			output.note_mode = noise_descriptor.note_mode;

			void* synth = noise_descriptor.noise_synth;
			switch (noise_type_)
			{
			case noise_type::white:
			{
				auto white_noise_synth = static_cast<cycfi::q::white_noise_synth*>(synth);
				for (uint_fast32_t frame = 0; frame < output_process_data.num_samples; frame++)
				{
					output.values[frame] = white_noise_synth->operator()();
				}
			}
				break;
			case noise_type::pink:
			{
				auto pink_noise_synth = static_cast<cycfi::q::pink_noise_synth*>(synth);
				for (uint_fast32_t frame = 0; frame < output_process_data.num_samples; frame++)
				{
					output.values[frame] = pink_noise_synth->operator()();
				}
			}
				break;
			case noise_type::brown:
			{
				auto brown_noise_synth = static_cast<cycfi::q::brown_noise_synth*>(synth);
				for (uint_fast32_t frame = 0; frame < output_process_data.num_samples; frame++)
				{
					output.values[frame] = brown_noise_synth->operator()();
				}
			}
				break;
			}
			nb_outputs_++;
		}
	}

	uint_fast32_t noise::get_output_values(const uint_fast16_t slot_id, void* output_values[])
	{
		*output_values = outputs_;

		return nb_outputs_;
	}

	bool noise::has_finished()
	{
		return false;
	}

	void noise::get_zombie_notes_ids(std::unordered_set<uint32_t>& zombie_notes_ids)
	{
	}

	void noise::set_zombie_notes_ids(const std::unordered_set<uint32_t>& zombie_notes_ids)
	{
	}

	uint_fast16_t noise::get_slot_id(const std::string& slot_name)
	{
		if (slot_name == onoff_input_name)
		{
			return onoff_input_id;
		}

		return -1;
	}

	void noise::set_input_values(const uint_fast16_t slot_id, void* values, const uint_fast32_t nb_values)
	{
		if (slot_id == onoff_input_id)
		{
			assert(nb_values <= 32);
			for (uint_fast16_t i = 0; i < nb_values; i++)
			{
				const auto& component_output = static_cast<novalue_component_output*>(values)[i];

				uint32_t input_id = component_output.output_id;

				if (const auto current_noise_descriptor = current_noises_descriptors_->find(input_id); current_noise_descriptor != current_noises_descriptors_->end())
				{
					//If synth was already set before, we don't want to erase it.
					//So, keep the old one.
					noise_descriptor<void>& noise_descriptor = next_noises_descriptors_->operator[](input_id);
					noise_descriptor.note_mode = component_output.note_mode;
					noise_descriptor.noise_synth = current_noise_descriptor->second.noise_synth;
				}
				else
				{
					//If synth is a new one, create it.
					noise_descriptor<void>& noise_descriptor = next_noises_descriptors_->operator[](input_id);
					noise_descriptor.note_mode = component_output.note_mode;
					noise_descriptor.noise_synth = new q::white_noise_synth{};
				}
			}

			//Switching between current and next synth is done to simplify the deletion of synths no more used.
			std::unordered_map<uint_fast16_t, noise_descriptor<void>>* tmp_noises_descriptors = current_noises_descriptors_;
			current_noises_descriptors_ = next_noises_descriptors_;
			next_noises_descriptors_ = tmp_noises_descriptors;

			//Clear the phases for next call.
			next_noises_descriptors_->clear();

			can_process_ = true;
		}
	}

	uint_fast32_t noise::get_max_nb_input_values(const uint_fast16_t slot_id)
	{
		if (slot_id == onoff_input_id)
		{
			return 32;
		}

		return -1;
	}

	void noise::set_noise_type(noise_type noise_type) {
		noise_type_ = noise_type;
	}
} // namespace
