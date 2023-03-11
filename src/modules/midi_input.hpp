#pragma once

#include <json.hpp>
#include <unordered_set>

#include "module_view_descriptor.hpp"
#include "../processor_module.hpp"
#include "../note_event.hpp"

namespace live::tritone::vie::view::module
{
	struct midi_input_descriptor : public module_view_descriptor {
		midi_input_descriptor()
		{
			id = 7;
			name = L"midi-in";
			icon = L"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>";
			input_slots = { {
				{0, L"on/off"}
			} };
			nb_input_slots = 1;
			output_slots = { {
				{1, L"notes on"},
				{2, L"notes off"},
				{3, L"frequencies"},
				{4, L"velocities"}
			} };
			nb_output_slots = 4;
		}
	};
}

namespace live::tritone::vie::processor::module
{
	class midi_input final : public processor_module
	{
	public:
		/**
		* Initialize class based on given JSON configuration.
		*/
		explicit midi_input(nlohmann::json midi_definition);

		~midi_input() override;

		uint16_t get_id() override;

		std::string get_name() override;

		processor_module_type get_type() override;

		void set_sample_rate(double sample_rate) override;

        uint_fast16_t get_slot_id(const std::string& slot_name) override;

		void set_input_values(uint_fast16_t slot_id, std::array<module_output*, 32>& values, uint_fast8_t nb_values) override;

		uint_fast32_t get_max_nb_input_values(uint_fast16_t slot_id) override;

		void preprocess() override;

		bool can_process() override;

		void process(output_process_data& output_process_data) override;

		uint_fast8_t get_output_values(uint_fast16_t slot_id, std::array<module_output*, 32>& values) override;

		bool has_finished() override;

		void set_parameter(parameter parameter) override;

		/* Receive a note on event from host and set its id.*/
		void note_on(note_event& note_on_event);

		/* Receive a note off event from host and set its id.*/
		void note_off(note_event& note_off_event);

		nlohmann::json serialize() override;
		
#ifdef UNIT_TESTING
	public:
#else
	private:
#endif
		static constexpr const char* onoff_input_name = "on/off input";
		static constexpr int onoff_input_id = 0;
		static constexpr const char* notes_on_output_name = "notes on output";
		static constexpr int notes_on_output_id = 1;
		static constexpr const char* notes_off_output_name = "notes off output";
		static constexpr int notes_off_output_id = 2;
		static constexpr const char* frequencies_output_name = "frequencies output";
		static constexpr int frequencies_output_id = 3;
		static constexpr const char* velocities_output_name = "velocities output";
		static constexpr int velocities_output_id = 4;

		uint16_t id_;

		std::string name_;
		std::unordered_map<uint32_t, note_event> notes_;

		uint_fast32_t nb_values_;
		std::array<float_module_output*, 32> frequencies_outputs_;
		std::array<float_module_output*, 32> velocities_outputs_;
		uint_fast32_t nb_notes_on_values_;
		std::array<novalue_module_output*, 32> notes_on_outputs_;
		uint_fast32_t nb_notes_off_values_;
		std::array<novalue_module_output*, 32> notes_off_outputs_;
		
		std::unordered_set<uint32_t> notes_ids_to_delete_;

		bool is_on = true;

		bool note_on_processed = false;
		bool note_off_processed = false;
	};
} // namespace
