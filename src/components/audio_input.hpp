#pragma once

#include <json.hpp>

#include "../processor_component.hpp"

namespace live::tritone::vie::processor::component
{
	class audio_input final : public processor_component
	{
	public:
		//FIXME: Just to test. To remove:
		  explicit audio_input();
		explicit audio_input(nlohmann::json audio_input_definition);
		audio_input(const audio_input&) = default;
		audio_input(audio_input&&) = default;

		~audio_input() override;

		audio_input& operator=(const audio_input&) = default;
		audio_input& operator=(audio_input&&) = default;

		uint16_t get_id() override;

		std::string get_name() override;

		processor_component_type get_type() override;

		void set_sample_rate(double audio_input_rate) override;

		void preprocess() override;

		bool can_process() override;

		component_output** get_outputs_pool(uint_fast16_t slot_id) override;

		void process(output_process_data& output_process_data) override;

		uint_fast32_t get_output_values(uint_fast16_t slot_id, component_output* output_values[32]) override;

		bool has_finished() override;

		uint_fast16_t get_slot_id(const std::string& slot_name) override;

		void set_input_values(uint_fast16_t slot_id, component_output* values[32], uint_fast32_t nb_values) override;

		uint_fast32_t get_max_nb_input_values(uint_fast16_t slot_id) override;

		void set_parameter(parameter parameter) override;

		void set_buffer(audio_bus_buffers* buffer);

	private:
		static constexpr const char* onoff_input_name = "on/off input";
		static constexpr int onoff_input_id = 0;

		static constexpr const char* audio_output_name = "Audio output";
		static constexpr int audio_output_id = 1;

		uint16_t id_;
		std::string name_;
		double sample_rate_;

		struct audio_input_descriptor {
			uint32_t nb_channels;
			uint32_t nb_frames;
			uint32_t rate;
			int format;
			float* buffers[8]; //8 channels max for now to handle surround 7.1.
 		};

		std::unordered_map<int, audio_input_descriptor> buffer;

		struct audio_input_state {
			bool activated;
			uint32_t position;
		};

		float_array_component_output empty_array_component_;

		float_array_component_output* output_;

		audio_bus_buffers* buffer_;
	};
} // namespace
