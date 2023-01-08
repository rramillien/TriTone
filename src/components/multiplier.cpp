#include "multiplier.hpp"

#include "../application.hpp"

namespace live::tritone::vie::processor::component
{
	multiplier::multiplier(nlohmann::json multiplier_definition) : processor_component(),
		id_(multiplier_definition["id"]),
		name_(multiplier_definition["name"]),
		nb_inputs_multipliers_(0),
		nb_inputs_multiplicands_(0),
		multipliers_filled_(false),
		multiplicands_filled_(false),
		nb_products_(0)
	{
		for (int i = 0; i < 32; i++) {
			multipliers_[i] = new float_array_component_output();
			multiplicands_[i] = new float_array_component_output();
			products_[i] = new float_array_component_output();
		}
	}
	
	multiplier::~multiplier() {
		for (int i = 0; i < 32; i++) {
			delete multipliers_[i];
			delete multiplicands_[i];
			delete products_[i];
		}
	}

	uint16_t multiplier::get_id()
	{
		return id_;
	}

	std::string multiplier::get_name()
	{
		return name_;
	}

	processor_component_type multiplier::get_type()
	{
		return processor_component_type::middle;
	}

	void multiplier::set_sample_rate(const double sample_rate)
	{
	}

	void multiplier::preprocess() {
	}

	bool multiplier::can_process()
	{
		return multipliers_filled_ && multiplicands_filled_;
	}

	void multiplier::process(output_process_data& output_process_data)
	{
		multipliers_filled_ = false;
		multiplicands_filled_ = false;

		nb_products_ = 0;

		for (uint_fast16_t input_id = 0; (input_id < nb_inputs_multipliers_) && (input_id < nb_inputs_multiplicands_); input_id++)
		{
			auto& output = *products_[input_id];

			//If nb of samples is greater than the ones currently allocated, reallocate.
			if (output_process_data.num_samples > output.values.nb_values)
			{
				if (output.values.nb_values > 0)
				{
					delete output.values.values;
				}
				output.values.values = new float[output_process_data.num_samples];
				output.values.nb_values = output_process_data.num_samples;
			}

			for (uint_fast32_t frame = 0; frame < output_process_data.num_samples; frame++)
			{
				output.note_id = id_;

				float operand1 = 0;
				auto multipliers_values = multipliers_[input_id]->to_float_array().values;
				if (nb_inputs_multipliers_ > input_id) {
					operand1 = multipliers_values[frame];
				}

				float operand2 = 0;
				auto multiplicands_values = multiplicands_[input_id]->to_float_array().values;
				if (nb_inputs_multiplicands_ > input_id) {
					operand2 = multiplicands_values[frame];
				}

				const float result = operand1 * operand2;

				output.values.values[frame] = result;
			}

			nb_products_++;
		}

		nb_inputs_multipliers_ = 0;
		nb_inputs_multiplicands_ = 0;
	}

	component_output** multiplier::get_outputs_pool(uint_fast16_t slot_id) {
		return (component_output**) products_;
	}

	uint_fast32_t multiplier::get_output_values(const uint_fast16_t slot_id, component_output* output_values[32])
	{
		output_values = (component_output**) products_;

		return nb_products_;
	}

	bool multiplier::has_finished()
	{
		return true;
	}

	uint_fast16_t multiplier::get_slot_id(const std::string& slot_name)
	{
		if (slot_name == multipliers_input_name)
		{
			return multipliers_input_id;
		}
		if (slot_name == multiplicands_input_name)
		{
			return multiplicands_input_id;
		}
		if (slot_name == products_output_name)
		{
			return products_output_id;
		}

		return -1;
	}

	void multiplier::set_input_values(const uint_fast16_t slot_id, component_output* values[32], const uint_fast32_t nb_values)
	{
		switch (slot_id)
		{
		case multipliers_input_id:
			assert(nb_values <= 32);
			nb_inputs_multipliers_ = nb_values;
			for (int i = 0; i < nb_inputs_multipliers_; i++) {
				multipliers_[i]->note_id = values[i]->note_id;
				multipliers_[i]->values = values[i]->to_float_array();
			}
			multipliers_filled_ = true;
			break;

		case multiplicands_input_id:	
			assert(nb_values <= 32);
			nb_inputs_multiplicands_ = nb_values;
			for (int i = 0; i < nb_inputs_multiplicands_; i++) {
				multiplicands_[i]->note_id = values[i]->note_id;
				multiplicands_[i]->values = values[i]->to_float_array();
			}
			multiplicands_filled_ = true;
			break;
		default:
			break;
		}
	}

	uint_fast32_t multiplier::get_max_nb_input_values(const uint_fast16_t slot_id)
	{
		switch (slot_id)
		{
		case multipliers_input_id:
		case multiplicands_input_id:
			return 32;
		default:
			break;
		}

		return -1;
	}

	void multiplier::set_parameter(parameter parameter)
	{

	}
} // namespace
