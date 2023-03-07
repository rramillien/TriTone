#include <Windows.h>

#include <codecvt>

#include "../application.hpp"
#include "../logger.hpp"

#include "../boolean_parameter.hpp"
#include "../float_parameter.hpp"

#include "../modules/audio_input.hpp"
#include "../modules/audio_output.hpp"
#include "../modules/envelope.hpp"
#include "../modules/gain.hpp"
#include "../modules/high_pass.hpp"
#include "../modules/low_pass.hpp"
#include "../modules/midi_input.hpp"
#include "../modules/mixer.hpp"
#include "../modules/multiplier.hpp"
#include "../modules/noise.hpp"
#include "../modules/oscillator.hpp"
#include "../modules/recorder.hpp"
#include "../modules/sample.hpp"

using namespace live::tritone::vie;
using namespace live::tritone::vie::processor::module;
using namespace live::tritone::vie::view::module;

extern "C" {
	bool InitDll() {
		return true;
	}

	bool ExitDll() {
		return true;
	}

	std::wstring content_path;

#ifdef VIE_DEBUG
	live::tritone::vie::debug::logger debugLogger;
#endif //VIE_DEBUG
} // extern "C"

void get_content_path(wchar_t(&local_content_path)[1024]) {
	HMODULE h_plugin;
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCWSTR>(&get_content_path),
		&h_plugin);

	//Get full path including dll file.
	wchar_t dll_path[1024];
	GetModuleFileNameW(h_plugin, dll_path, sizeof(dll_path) / sizeof(wchar_t));

	//Remove the dll file name.
	const wchar_t* dll_file_name = wcsrchr(dll_path, '\\');
	wcsncpy_s(local_content_path, dll_path, wcslen(dll_path) - wcslen(dll_file_name));
}

BOOL WINAPI DllMain(HINSTANCE /*h_instance*/, DWORD /*dw_reason*/, LPVOID /*lpv_reserved*/) {
	wchar_t local_content_path[1024];
	get_content_path(local_content_path);
	local_content_path[1023] = '\0';
	content_path = std::wstring(local_content_path) + L"\\";

#ifdef VIE_DEBUG
	debugLogger.open("debug.log", true);
#endif // VIE_DEBUG

	return TRUE;
}

parameter application::add_parameter(uint_fast8_t id, parameter parameter)
{
	auto parameter_ptr = new live::tritone::vie::parameter(parameter);

	parameters_[nb_parameters_] = parameter_ptr;
	nb_parameters_++;

	return *parameter_ptr;
}

parameter application::add_parameter(uint_fast8_t id, std::string name, std::string short_name, std::string type, std::string unit, float value) {
	if (type == "boolean") {
		//FIXME: use wstring in parameters and fund a way to parse wstring with json.
		boolean_parameter parameter(id,
			std::wstring(name.begin(), name.end()).c_str(),
			std::wstring(short_name.begin(), short_name.end()).c_str(),
			std::wstring(unit.begin(), unit.end()).c_str(),
			value);
		return add_parameter(id, parameter);
	}
	else {
		float_parameter parameter(id,
			std::wstring(name.begin(), name.end()).c_str(),
			std::wstring(short_name.begin(), short_name.end()).c_str(),
			std::wstring(unit.begin(), unit.end()).c_str(),
			value);
		return add_parameter(id, parameter);
	}
}

parameter application::get_parameter(uint_fast8_t id)
{
	return *parameters_[id];
}

uint_fast8_t application::get_parameters_count()
{
	return nb_parameters_;
}

project_info& application::new_project()
{
	//reinit all components
	nb_parameters_ = 0;
	nb_modules_ = 0;
	nb_relations_ = 0;
	
	vie_processor_.initialize();
	vie_view_->initialize();
	
	//Generate empty project.
	//FIXME: Generate a UUID.
	current_project_.id = nb_projects_;
	//FIXME: Generate a random name.
	current_project_.name = L"Project " + std::to_wstring(nb_projects_);
	
	//Save project.
	save_project();
	
	//Update list of projects.
	projects_infos_[nb_projects_] = current_project_;
	nb_projects_++;
	
	//Return generated project infos.
	return current_project_;
}

std::array<project_info, 32> application::get_projects(int* nb_projects)
{	
	*nb_projects = nb_projects_;
	return projects_infos_;
}

void application::save_project()
{
	std::wstringstream path_stream;
	path_stream << L"c:/tmp/" << current_project_.id << L".json";
	export_project(path_stream.str());
}

project_info application::load_project(uint_fast16_t id)
{
	std::wstringstream path_stream;
	path_stream << L"c:/tmp/" << id << L".json";
	
	return import_project(path_stream.str());
}

void application::export_project(std::wstring project_path)
{
	nlohmann::json project_json = nlohmann::json();
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	project_json["id"] = current_project_.id;
	project_json["name"] = converter.to_bytes(current_project_.name);
	project_json["description"] = converter.to_bytes(current_project_.description);

	nlohmann::json processor_json = vie_processor_.serialize();
	nlohmann::json view_json = vie_view_->serialize();

	nlohmann::json root = nlohmann::json();
	root["project"] = project_json;
	root["processor"] = processor_json;
	root["view"] = view_json;

	std::ofstream projectStream(project_path);
	projectStream << root;
}

project_info application::import_project(std::wstring project_path)
{
	std::ifstream projectStream(project_path);
	nlohmann::json project_json;
	projectStream >> project_json;

	//Load processor.
	for (auto& [index, processor] : project_json["modules"].items()) {
		add_module(processor);
	}
	for (auto& [index, relation] : project_json["relations"].items()) {
		add_relation(relation);
	}

	//Load view.
	vie_view_->load(project_json);

	//Load project infos.
	nlohmann::json project_info_json = project_json["project"];

	uint_fast16_t id = project_info_json["id"];
	std::string name = project_info_json["name"];
	std::string description = project_info_json["description"];

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	std::wstring name_w = converter.from_bytes(name);
	std::wstring description_w = converter.from_bytes(description);
	
	return project_info({ id, name_w, description_w });
}

std::array<module_view_descriptor, 64> application::get_available_modules(uint_fast8_t* nb_modules)
{
	*nb_modules = 13;
	return std::array<module_view_descriptor, 64> {
			audio_input_descriptor(),
			audio_output_descriptor(),
			envelope_descriptor(),
			gain_descriptor(),
			high_pass_descriptor(),
			low_pass_descriptor(),
			midi_input_descriptor(),
			mixer_descriptor(),
			multiplier_descriptor(),
			noise_descriptor(),
			oscillator_descriptor(),
			recorder_descriptor(),
			sample_descriptor()
		};
}

uint16_t application::add_module(nlohmann::json module)
{
	uint16_t module_id = nb_modules_;

	module["id"] = module_id;
	
	auto& processor = create_processor_module(module);
	vie_processor_.add_processor(*processor);
	
	modules_views_instances_[module_id] = std::make_unique<module_view_instance>();
	modules_views_instances_[module_id]->id = module_id;
	modules_views_instances_[module_id]->position[0] = module["position"]["x"];
	modules_views_instances_[module_id]->position[1] = module["position"]["y"];
	modules_views_instances_[module_id]->position[2] = module["position"]["z"];

	nb_modules_++;

	save_project();

	return module_id;
}

uint16_t application::add_relation(nlohmann::json relation)
{
	uint16_t relation_id = nb_relations_;

	relation["id"] = relation_id;
	auto& module_relation = create_module_relation(relation);
	vie_processor_.add_relation(module_relation.get());

	nb_relations_++;
	
	save_project();

	return relation_id;
}

vie_processor& application::get_vie_processor()
{
	return vie_processor_;
}

vie_view* application::create_view(host_callback* callback)
{
	vie_view_ = new vie_view(callback);
	return vie_view_;
}

vie_view* application::deleteView()
{
	delete vie_view_;
	vie_view_ = nullptr;
	return nullptr;
}

std::unique_ptr<processor_module>& application::create_processor_module(nlohmann::json processor_definition) {

	const std::string type = processor_definition["type"];
	if (type == "midi-in")
	{
		processors_[nb_modules_] = std::make_unique<midi_input>(processor_definition);
	}
	else if (type == "audio-in")
	{
		processors_[nb_modules_] = std::make_unique<audio_input>(processor_definition);
	}
	else if (type == "oscillator")
	{
		processors_[nb_modules_] = std::make_unique<oscillator>(processor_definition);
	}
	else if (type == "envelope")
	{
		processors_[nb_modules_] = std::make_unique<envelope>(processor_definition);
	}
	else if (type == "multiplier")
	{
		processors_[nb_modules_] = std::make_unique<multiplier>(processor_definition);
	}
	else if (type == "mixer")
	{
		processors_[nb_modules_] = std::make_unique<mixer>(processor_definition);
	}
	else if (type == "sample")
	{
		processors_[nb_modules_] = std::make_unique<sample>(processor_definition);
	}
	else if (type == "audio out")
	{
		processors_[nb_modules_] = std::make_unique<audio_output>(processor_definition);
	}
	else if (type == "low-pass")
	{
		processors_[nb_modules_] = std::make_unique<low_pass>(processor_definition);
	}
	else if (type == "high-pass")
	{
		processors_[nb_modules_] = std::make_unique<high_pass>(processor_definition);
	}
	else if (type == "gain")
	{
		processors_[nb_modules_] = std::make_unique<gain>(processor_definition);
	}
	else if (type == "recorder")
	{
		processors_[nb_modules_] = std::make_unique<recorder>(processor_definition);
	}

	processors_[nb_modules_]->initialize(processor_definition);

#ifdef VIE_DEBUG
	debugLogger.write("Added processor: " + processor->get_name());
#endif

	return processors_[nb_modules_];
}

std::unique_ptr<module_relation>& application::create_module_relation(nlohmann::json relation_definition) {
	const int source_module_id = relation_definition["sourceComponent"];

	relations_[nb_relations_]->source_module = processors_[source_module_id].get();

	const uint_fast16_t source_output_id = relation_definition["sourceOutputSlot"];
	relations_[nb_relations_]->source_slot_id = source_output_id;

	const int target_module_id = relation_definition["targetComponent"];
	relations_[nb_relations_]->target_module = processors_[target_module_id].get();

	const uint_fast16_t target_input_id = relation_definition["targetInputSlot"];
	relations_[nb_relations_]->target_slot_id = target_input_id;

	return relations_[nb_relations_];
}

processor_module& application::get_processor_by_id(uint_fast8_t id)
{
	return *processors_[id];
}

module_relation& application::get_relation_by_id(uint_fast8_t id)
{
	return *relations_[id];
}

application application_;