#include "editor_view.hpp"

#include <functional>
#include <OAIdl.h>
#include <json.hpp>

#include <locale>
#include <codecvt>
#include <sstream>

#include "../application.hpp"
#include "../processor_component.hpp"

using namespace std;
using json = nlohmann::json;

namespace live::tritone::vie
{
	editor_view::editor_view() :
		ptr_web_view_controller_(nullptr),
		ptr_web_view_window_(nullptr),
		web_message_received_token_(),
		handler_(nullptr)
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

								// Should use AddHostObjectToScript but can't make it work. So use messages instead.
								ptr_web_view_window_->add_WebMessageReceived(
									Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
										[this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) {
											if (handler_ != nullptr) {
												//Get message from webview.
												wil::unique_cotaskmem_string message_string;
												args->get_WebMessageAsJson(&message_string);

												//Transform message to string
												std::wstring json_message_wstring = message_string.get();
												int size_needed = WideCharToMultiByte(CP_UTF8, 0, &json_message_wstring[0], (int)json_message_wstring.size(), NULL, 0, NULL, NULL);
												std::string json_message_string(size_needed, 0);
												WideCharToMultiByte(CP_UTF8, 0, &json_message_wstring[0], (int)json_message_wstring.size(), &json_message_string[0], size_needed, NULL, NULL);

												//Transform message string to json object.
												const char* json_message_char_ptr = json_message_string.c_str();
												nlohmann::json json_message = nlohmann::json::parse(json_message_char_ptr);

												//get parameter values from json message.
	 											//const uint16_t parameter_component_id = json_message["component"];
												const std::string parameter_component_name = json_message["component"];
												processor_component& processor_component = application::get_processor_components().get_by_name(parameter_component_name);
												uint16_t parameter_component_id = processor_component.get_id();
												//const uint16_t parameter_slot_id = json_message["slot"];
												const std::string parameter_slot_name = json_message["slot"];
												uint16_t parameter_slot_id = processor_component.get_slot_id(parameter_slot_name);
												const double parameter_value = json_message["value"];

												//Compute component and its parameter ids to one single id.
												unsigned long parameter_id = (parameter_component_id << 16) | parameter_slot_id;

												//Transmit parameter to host.
												handler_->beginEdit(parameter_id);
												handler_->performEdit(parameter_id, parameter_value);
												handler_->endEdit(parameter_id);
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

	void editor_view::set_component_handler(Steinberg::Vst::IComponentHandler* handler)
	{
		handler_ = handler;
	}
} // namespace
