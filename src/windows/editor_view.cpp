#include "editor_view.hpp"

#include <functional>
#include <OAIdl.h>

#include <locale>
#include <codecvt>
#include <sstream>

#include "../application.hpp"
#include "../processor_module.hpp"

using namespace std;
using json = nlohmann::json;

namespace live::tritone::vie
{
	editor_view::editor_view() :
		ptr_web_view_controller_(nullptr),
		ptr_web_view_window_(nullptr),
		web_message_received_token_(),
		host_callback_(nullptr)
	{
		//Get appdata path which will be used to create temporary files required to use webview.
		char* appdata_ASCII_path = getenv("APPDATA");
		MultiByteToWideChar(CP_ACP, 0, appdata_ASCII_path, -1, appdata_path, 4096);
	}

	editor_view::~editor_view()
	{
	}

	void editor_view::attached(void* parent) {
		HWND hWnd = static_cast<HWND>(parent);

		// Create web view environment based on Appdata path.
		CreateCoreWebView2EnvironmentWithOptions(nullptr, appdata_path, nullptr,
			Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[hWnd, this]
		(HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
					// Create a CoreWebView2Controller and get the associated CoreWebView2 whose parent is the main window hWnd
					env->CreateCoreWebView2Controller(hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[hWnd, this]
					(HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
							if (controller != nullptr) {
								ptr_web_view_controller_ = controller;
								controller->AddRef();
								ptr_web_view_controller_->get_CoreWebView2(&ptr_web_view_window_);
								ptr_web_view_window_->AddRef();
								ptr_web_view_window_->OpenDevToolsWindow();

								// Should use AddHostObjectToScript but can't make it work. So use messages instead.
								ptr_web_view_window_->add_WebMessageReceived(
									Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
										[this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) {
											if (host_callback_ != nullptr) {
												//Get message from webview.
												wil::unique_cotaskmem_string message_string;
												args->get_WebMessageAsJson(&message_string);

												//Transform message to string
												std::wstring json_message_wstring = message_string.get();
												on_message_received(sender, json_message_wstring);
											}
											return S_OK;
										}).Get(), &web_message_received_token_);

								// Resize WebView to fit the bounds of the parent window
								RECT bounds;
								GetClientRect(hWnd, &bounds);
								ptr_web_view_controller_->put_Bounds(bounds);

								// Open VIE HTML view.
								ptr_web_view_window_->Navigate(
									(std::wstring(L"file://") + content_path + L"/view/index.html").c_str()
								);
							}
							return S_OK;
						}).Get());
					return S_OK;
				}).Get());
	}

	void editor_view::removed()
	{
	}
	
	void editor_view::on_message_received(ICoreWebView2* sender, std::wstring json_wstring)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &json_wstring[0], (int)json_wstring.size(), NULL, 0, NULL, NULL);
		std::string json_message_string(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &json_wstring[0], (int)json_wstring.size(), &json_message_string[0], size_needed, NULL, NULL);

		//Transform message string to json object.
		const char* json_message_char_ptr = json_message_string.c_str();
		json json_message = json::parse(json_message_char_ptr);

		//get parameter values from json message.
		const std::string action_name = json_message["action"];
		if (action_name.compare("get_projects") == 0) {
			on_message_get_projects(sender, json_message);
		}
		else if (action_name.compare("new_project") == 0) {
			on_message_new_project(sender, json_message);
		}
		else if (action_name.compare("export_project") == 0) {
			on_message_export_project(sender, json_message);
		}
		else if (action_name.compare("import_project") == 0) {
			on_message_import_project(sender, json_message);
		}
		else if (action_name.compare("get_modules") == 0) {
			on_message_get_modules(sender, json_message);
		}
		else if (action_name.compare("add_module") == 0) {
			on_message_add_module(sender, json_message);
		}
		else if (action_name.compare("set_module_name") == 0) {
			on_message_set_module_name(sender, json_message);
		}
		else if (action_name.compare("delete_module") == 0) {
			on_message_delete_module(sender, json_message);
		}
		else if (action_name.compare("set_module_parameter_value") == 0) {
			on_message_set_module_parameter_value(sender, json_message);
		}
		else if (action_name.compare("link_modules") == 0) {
			on_message_link_modules(sender, json_message);
		}
		else if (action_name.compare("undo") == 0) {
			on_message_undo(sender, json_message);
		}
		else if (action_name.compare("redo") == 0) {
			on_message_redo(sender, json_message);
		}
	}

	void editor_view::set_host_callback(host_callback* host_callback)
	{
		host_callback_ = host_callback;
	}

	void editor_view::on_message_get_projects(ICoreWebView2* sender, json message) {
		int nb_projects;
		std::array<project_info, 32> projects = application_.get_projects(&nb_projects);

		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"get_projects_callback\", \"body\": [";
		
		for (int i = 0; i < nb_projects; i++) {
			auto project = projects[i];

			reply << L" {";
			reply << L" \"id\": ";
			reply << project.id;
			reply << L", \"name\": \"";
			reply << project.name;
			reply << L"\", \"description\": \"";
			reply << project.description;
			reply << L"\" }";
			if(i < nb_projects - 1)
			{
				reply << L",";
			}
		}

		reply << L" ] }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_new_project(ICoreWebView2* sender, json message) {
		project_info project = application_.new_project();

		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"new_project_callback\", \"body\": { \"id\": ";
		reply << project.id;
		reply << L", \"name\": \"";
		reply << project.name;
		reply << L"\" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_export_project(ICoreWebView2* sender, json message) {
		std::string project_path = message["body"]["path"];

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring project_path_w = converter.from_bytes(project_path);

		application_.export_project(project_path_w);
		
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"export_project_callback\",";
		reply << L" \"body\": {";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_import_project(ICoreWebView2* sender, json message) {
		std::string project_path = message["body"]["path"];

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring project_path_w = converter.from_bytes(project_path);

		project_info project = application_.import_project(project_path_w);
		
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"import_project_callback\",";
		reply << L" \"body\": {";
		reply << L" \"id\": ";
		reply << project.id;
		reply << L",";
		reply << L" \"name\": \"";
		reply << project.name;
		reply << L"\",";
		reply << L" \"description\": \"";
		reply << project.description;
		reply << L"\"";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_get_modules(ICoreWebView2* sender, json message) {
		auto modules = application_.get_modules();
		
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"get_modules_callback\",";
		reply << L" \"body\": [";
		
		for (auto module : modules)
		{
			reply << L" {";
			reply << L" \"id\": ";
			reply << module.id;
			reply << L", \"name\": ";
			reply << module.name;
			reply << L", \"icon\": ";
			reply << module.icon;
			reply << L", \"input_slots\": ";
			reply << L" [";
			
			for (auto input_slot : module.input_slots)
			{
				reply << L" {";
				reply << L" \"id\": ";
				reply << input_slot.id;
				reply << L", \"name\": ";
				reply << input_slot.name;
				reply << L" }";
			}
			
			reply << L" ],";
			reply << L" \"output_slots\": ";
			reply << L" [";

			for (auto input_slot : module.output_slots)
			{
				reply << L" {";
				reply << L" \"id\": ";
				reply << input_slot.id;
				reply << L", \"name\": ";
				reply << input_slot.name;
				reply << L" }";
			}
			
			reply << L" ]";
			reply << L" }";
		}
		
		reply << L" ]";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_add_module(ICoreWebView2* sender, json message) {
		nlohmann::json module_json = message.at("body");

		uint16_t module_id = application_.add_module(module_json);
		
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"add_module_callback\",";
		reply << L" \"body\": {";
		reply << L" \"module\": {";
		reply << L" \"id\": ";
		reply << std::to_wstring(module_id);
		reply << L" }";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_delete_module(ICoreWebView2* sender, json message) {
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"delete_module_callback\",";
		reply << L" \"body\": {";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_set_module_name(ICoreWebView2* sender, json message) {
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"set_module_name_callback\",";
		reply << L" \"body\": {";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_set_module_parameter_value(ICoreWebView2* sender, json message) {
		nlohmann::json parameters = message["parameters"];
		uint_fast8_t module_id;
		string parameter_id;
		string parameter_value;
		bool module_found = false;
		bool parameter_id_found = false;
		bool parameter_value_found = false;

		for (auto& it : parameters.items()) {
			const std::string parameter_name = it.value()["id"];
			if (parameter_name.compare("module_id") == 0) {
				module_found = true;
				module_id = it.value().at("value");
			}
			else if (parameter_name.compare("parameter_id") == 0) {
				parameter_id_found = true;
				parameter_id = it.value().at("value");
			}
			else if (parameter_name.compare("parameter_value") == 0) {
				parameter_value_found = true;
				parameter_value = it.value().at("value");
			}
		}

		if (module_found && parameter_id_found && parameter_value_found) {
			processor_module& processor_module = application_.get_processor_by_id(module_id);
			uint16_t parameter_component_id = processor_module.get_id();
			uint16_t parameter_slot_id = processor_module.get_slot_id(parameter_id);

			unsigned long module_parameter_id = (parameter_component_id << 16) | parameter_slot_id;

			//Transmit parameter to host.
			host_callback_->beginEdit(module_parameter_id);
			host_callback_->performEdit(module_parameter_id, std::stof(parameter_value));
			host_callback_->endEdit(module_parameter_id);
		}
		else {
			if (!module_found) {
				sender->PostWebMessageAsJson(L"{ \"action\": \"set_module_parameter_value_callback\", \"error\": 0 }");
			}
			if (!parameter_id_found) {
				sender->PostWebMessageAsJson(L"{ \"action\": \"set_module_parameter_value_callback\", \"error\": 1 }");
			}
			if (!parameter_value_found) {
				sender->PostWebMessageAsJson(L"{ \"action\": \"set_module_parameter_value_callback\", \"error\": 2 }");
			}
		}
	}

	void editor_view::on_message_link_modules(ICoreWebView2* sender, json message) {
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"link_modules_callback\",";
		reply << L" \"body\": {";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_undo(ICoreWebView2* sender, json message) {
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"undo_callback\",";
		reply << L" \"body\": {";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}

	void editor_view::on_message_redo(ICoreWebView2* sender, json message) {
		std::wstringstream reply;
		reply << L"{";
		reply << L" \"action\": \"redo_callback\",";
		reply << L" \"body\": {";
		reply << L" }";
		reply << L" }";
		sender->PostWebMessageAsJson(reply.str().c_str());
	}
} // namespace
