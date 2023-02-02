#include "editor_view.hpp"

#include <functional>
#include <OAIdl.h>
#include <json.hpp>

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
                                                const std::string action_name = json_message["action"];
                                                if(action_name.compare("set_module_parameter_value") == 0) {
                                                    nlohmann::json parameters = json_message["parameters"];
                                                    uint_fast8_t module_id;
                                                    string parameter_id;
                                                    string parameter_value;

													for (auto& it : parameters.items()) {
														const std::string parameter_name = it.value()["id"];
														if (parameter_name.compare("module_id") == 0) {
															module_id = it.value().at("value");
														}
														else if (parameter_name.compare("parameter_id") == 0) {
															parameter_id = it.value().at("value");
														}
														else if (parameter_name.compare("parameter_value") == 0) {
															parameter_value = it.value().at("value");
														}
													}
													
                                                    processor_module& processor_module = application::get_processor_modules().get_by_id(module_id);
												    uint16_t parameter_module_id = processor_module.get_id();
												    uint16_t parameter_slot_id = processor_module.get_slot_id(parameter_id);
													
                                                    unsigned long module_parameter_id = (parameter_module_id << 16) | parameter_slot_id;

    //												//Transmit parameter to host.
                                                    handler_->beginEdit(module_parameter_id);
                                                    handler_->performEdit(module_parameter_id, std::stof(parameter_value));
                                                    handler_->endEdit(module_parameter_id);
                                                }
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
