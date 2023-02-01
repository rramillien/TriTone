#pragma once

#include <json.hpp>

#include "../processor_component.hpp"

namespace live::tritone::vie::processor::component
{
	/**
	* @brief A sample processor component.
	* 
	*/
	class high_pass final : public processor_component
	{
	public:
		explicit high_pass(nlohmann::json high_pass_definition);

		~high_pass();

		uint16_t get_id() override;

		std::string get_name() override;

		processor_component_type get_type() override;

		void set_sample_rate(double sample_rate) override;

		void preprocess() override;

		bool can_process() override;

		void process(output_process_data& output_process_data) override;

		/**
		* Set a pair<string, float> where string is the id of this high_pass and float is the high_pass output value.
		*/
		uint_fast8_t get_output_values(uint_fast16_t slot_id, std::array<component_output*, 32>& values) override;

		bool has_finished() override;

		uint_fast16_t get_slot_id(const std::string& slot_name) override;

		void set_input_values(uint_fast16_t slot_id, std::array<component_output*, 32>& values, uint_fast8_t nb_values) override;

		uint_fast32_t get_max_nb_input_values(uint_fast16_t slot_id) override;

		void set_parameter(parameter parameter) override;

	private:
		static constexpr const char* onoff_input_name = "on/off input";
		static constexpr int onoff_input_id = 0;
		
		constexpr static const char* generics_input_name = "signal input";
		constexpr static int generics_input_id = 1;

		constexpr static const char* cutoff_input_name = "cutoff input";
		constexpr static int cutoff_input_id = 2;

		constexpr static const char* resonance_input_name = "resonance input";
		constexpr static int resonance_input_id = 3;

		constexpr static const char* average_output_name = "filtered output";
		constexpr static int average_output_id = 4;

		uint16_t id_;
		std::string name_;
		std::string type_;

		uint_fast32_t nb_inputs_;
		std::array<float_array, 32> inputs_;
		bool inputs_set_;

		float cutoff_;
		bool cutoff_set_;
		float resonance_;
		bool resonance_set_;

		float omega;
		float sin_omega;
		float alpha;
		float cos_omega;
		float input_prev_[2]; // Entr�es pr�c�dentes
		float output_prev_[2]; // Sorties pr�c�dentes
		float a1, a2, b0, b1, b2; // Coefficients du filtre

		bool input_modified_;

		double sample_rate_;

		std::array<float_array_component_output*, 32> filtered_;
	};
} // namespace
