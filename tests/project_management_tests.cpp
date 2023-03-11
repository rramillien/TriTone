#include "../src/application.hpp"
#include "../src/windows/editor_view.hpp"
#include "../src/vst_processor.hpp"

#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <WebView2.h>

#include <catch2/catch_test_macros.hpp>

using namespace live::tritone::vie;

class MockCoreWebView2: public ICoreWebView2
{
public:
    HRESULT QueryInterface(const IID& riid, void** ppvObject) override { return 0; }
    ULONG AddRef() override { return 0; }
    ULONG Release() override { return 0; }
	
    HRESULT STDMETHODCALLTYPE get_Settings(
        ICoreWebView2Settings** settings) override { return 0; }

    HRESULT STDMETHODCALLTYPE get_Source(
        LPWSTR* uri) override { return 0; }

    HRESULT STDMETHODCALLTYPE Navigate(
        LPCWSTR uri) override { return 0; }

    HRESULT STDMETHODCALLTYPE NavigateToString(
        LPCWSTR htmlContent) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_NavigationStarting(
        ICoreWebView2NavigationStartingEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_NavigationStarting(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_ContentLoading(
        ICoreWebView2ContentLoadingEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_ContentLoading(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_SourceChanged(
        ICoreWebView2SourceChangedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_SourceChanged(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_HistoryChanged(
        ICoreWebView2HistoryChangedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_HistoryChanged(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_NavigationCompleted(
        ICoreWebView2NavigationCompletedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_NavigationCompleted(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_FrameNavigationStarting(
        ICoreWebView2NavigationStartingEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_FrameNavigationStarting(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_FrameNavigationCompleted(
        ICoreWebView2NavigationCompletedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_FrameNavigationCompleted(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_ScriptDialogOpening(
        ICoreWebView2ScriptDialogOpeningEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_ScriptDialogOpening(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_PermissionRequested(
        ICoreWebView2PermissionRequestedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_PermissionRequested(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_ProcessFailed(
        ICoreWebView2ProcessFailedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_ProcessFailed(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE AddScriptToExecuteOnDocumentCreated(
        LPCWSTR javaScript,
        ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler* handler) override { return 0; }

    HRESULT STDMETHODCALLTYPE RemoveScriptToExecuteOnDocumentCreated(
        LPCWSTR id) override { return 0; }

    HRESULT STDMETHODCALLTYPE ExecuteScript(
        LPCWSTR javaScript,
        ICoreWebView2ExecuteScriptCompletedHandler* handler) override { return 0; }

    HRESULT STDMETHODCALLTYPE CapturePreview(
        COREWEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
        IStream* imageStream,
        ICoreWebView2CapturePreviewCompletedHandler* handler) override { return 0; }

    HRESULT STDMETHODCALLTYPE Reload(void) override { return 0; }

    HRESULT STDMETHODCALLTYPE PostWebMessageAsJson(LPCWSTR webMessageAsJson) override { 
        wcscpy_s(last_message, webMessageAsJson);
        return 0;
    }

    HRESULT STDMETHODCALLTYPE PostWebMessageAsString(
        LPCWSTR webMessageAsString) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_WebMessageReceived(
        ICoreWebView2WebMessageReceivedEventHandler* handler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_WebMessageReceived(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE CallDevToolsProtocolMethod(
        LPCWSTR methodName,
        LPCWSTR parametersAsJson,
        ICoreWebView2CallDevToolsProtocolMethodCompletedHandler* handler) override { return 0; }

    HRESULT STDMETHODCALLTYPE get_BrowserProcessId(
        UINT32* value) override { return 0; }

    HRESULT STDMETHODCALLTYPE get_CanGoBack(
        BOOL* canGoBack) override { return 0; }

    HRESULT STDMETHODCALLTYPE get_CanGoForward(
        BOOL* canGoForward) override { return 0; }

    HRESULT STDMETHODCALLTYPE GoBack(void) override { return 0; }

    HRESULT STDMETHODCALLTYPE GoForward(void) override { return 0; }

    HRESULT STDMETHODCALLTYPE GetDevToolsProtocolEventReceiver(
        LPCWSTR eventName,
        ICoreWebView2DevToolsProtocolEventReceiver** receiver) override { return 0; }

    HRESULT STDMETHODCALLTYPE Stop(void) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_NewWindowRequested(
        ICoreWebView2NewWindowRequestedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_NewWindowRequested(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_DocumentTitleChanged(
        ICoreWebView2DocumentTitleChangedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_DocumentTitleChanged(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE get_DocumentTitle(
        LPWSTR* title) override { return 0; }

    HRESULT STDMETHODCALLTYPE AddHostObjectToScript(
        LPCWSTR name,
        VARIANT* object) override { return 0; }

    HRESULT STDMETHODCALLTYPE RemoveHostObjectFromScript(
        LPCWSTR name) override { return 0; }

    HRESULT STDMETHODCALLTYPE OpenDevToolsWindow(void) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_ContainsFullScreenElementChanged(
        ICoreWebView2ContainsFullScreenElementChangedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_ContainsFullScreenElementChanged(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE get_ContainsFullScreenElement(
        BOOL* containsFullScreenElement) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_WebResourceRequested(
        ICoreWebView2WebResourceRequestedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_WebResourceRequested(
        EventRegistrationToken token) override { return 0; }

    HRESULT STDMETHODCALLTYPE AddWebResourceRequestedFilter(
        const LPCWSTR uri,
        const COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext) override { return 0; }

    HRESULT STDMETHODCALLTYPE RemoveWebResourceRequestedFilter(
        const LPCWSTR uri,
        const COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext) override { return 0; }

    HRESULT STDMETHODCALLTYPE add_WindowCloseRequested(
        ICoreWebView2WindowCloseRequestedEventHandler* eventHandler,
        EventRegistrationToken* token) override { return 0; }

    HRESULT STDMETHODCALLTYPE remove_WindowCloseRequested(
        EventRegistrationToken token) override { return 0; }

    LPCWSTR get_last_message() {
		return last_message;
    }
	
private:
    wchar_t last_message[300000];
};

class mock_param_value_queue : public Steinberg::Vst::IParamValueQueue
{
public:
    Steinberg::int32 sample_offset_;
    Steinberg::Vst::ParamValue value_;

    Steinberg::tresult PLUGIN_API queryInterface(const char _iid[16], void** obj)
    {
        return Steinberg::kResultTrue;
    }

    Steinberg::uint32 PLUGIN_API addRef()
    {
        return 0;
    }

    Steinberg::uint32 PLUGIN_API release()
    {
        return 0;
    }

    Steinberg::Vst::ParamID PLUGIN_API getParameterId()
    {
        return 0;
    }

    Steinberg::int32 PLUGIN_API getPointCount()
    {
        return 1;
    }

    Steinberg::tresult PLUGIN_API getPoint(Steinberg::int32 index, Steinberg::int32& sampleOffset /*out*/, Steinberg::Vst::ParamValue& value /*out*/)
    {
        sampleOffset = sample_offset_;
        value = value_;
        return Steinberg::kResultTrue;
    }

    Steinberg::tresult PLUGIN_API addPoint(Steinberg::int32 sampleOffset, Steinberg::Vst::ParamValue value, Steinberg::int32& index /*out*/)
    {
        index = 0;
        sample_offset_ = sampleOffset;
        value_ = value;
        return Steinberg::kResultTrue;
    }
};

class mock_parameter_changes : public Steinberg::Vst::IParameterChanges
{
public:
    mock_param_value_queue param_value_queue_;

    Steinberg::tresult PLUGIN_API queryInterface(const char _iid[16], void** obj) override
    {
        return Steinberg::kResultTrue;
    }

    Steinberg::uint32 PLUGIN_API addRef() override
    {
        return 0;
    }

    Steinberg::uint32 PLUGIN_API release() override
    {
        return 0;
    }

    Steinberg::int32 PLUGIN_API getParameterCount() override
    {
        return 1;
    }

    Steinberg::Vst::IParamValueQueue* PLUGIN_API getParameterData(Steinberg::int32 index) override
    {
        return &param_value_queue_;
    }

    Steinberg::Vst::IParamValueQueue* PLUGIN_API addParameterData(const Steinberg::Vst::ParamID& id, Steinberg::int32& index /*out*/) override
    {
        index = 0;
        return &param_value_queue_;
    }
};

live::tritone::vie::vst::vst_processor vst_processor;

class mock_host_callback : public host_callback
{
public:
    void beginEdit(uint32_t id)
    {
        last_edition_started_ = id;
    }

    void performEdit(uint32_t id, double value_normalized)
    {
        last_value_ = value_normalized;
    }

    void endEdit(uint32_t id)
    {
        //Test acts as a host: Propagate message from view to processor.
        Steinberg::Vst::ProcessData data;
        mock_parameter_changes parameter_changes;
        Steinberg::Vst::IParamValueQueue* param_value_queue = parameter_changes.getParameterData(0);
        Steinberg::Vst::ParamValue param_value = last_value_;
        param_value_queue->addPoint(0, param_value, index_);
        data.inputParameterChanges = &parameter_changes;
        vst_processor.process(data);

        last_edition_ended_ = id;
    }

    void restartComponent(int32_t flags)
    {
    }

    Steinberg::int32 index_ = 0;
    uint32_t last_edition_started_;
    double last_value_;
    uint32_t last_edition_ended_;
};

void reinit()
{
    application_.nb_projects_ = 0;
}

live::tritone::vie::editor_view editor_view_;
MockCoreWebView2 mock_core_web_view_;
mock_host_callback mock_callback_;
mock_parameter_changes parameter_changes;

SCENARIO("Retrieve standard modules.", "[editor view]") {
    SECTION("Initialisation") {
        reinit();
    }

    GIVEN("Standard modules exists") {
        WHEN("on_message_get_modules is called") {
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"get_modules\",  \"body\": {}}");

            THEN("Modules are retrieved.") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected_stream;

                expected_stream << L"{ \"action\": \"get_modules_callback\", ";
                expected_stream << "\"body\": [";
                expected_stream << " { \"id\": 0, \"name\": \"audio-in\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"audio-in\" } ], \"output_slots\": [ { \"id\": 0, \"name\": \"audio-out\" } ] },";
                expected_stream << " { \"id\": 1, \"name\": \"audio-out\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"amplitudes\" } ], \"output_slots\": [ ] },";
                expected_stream << " { \"id\": 3, \"name\": \"envelope\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"velocities\" }, { \"id\": 2, \"name\": \"notes on\" }, { \"id\": 3, \"name\": \"notes off\" } ], \"output_slots\": [ { \"id\": 4, \"name\": \"amplitudes\" }, { \"id\": 5, \"name\": \"notes off\" }, { \"id\": 6, \"name\": \"sustain start\" }, { \"id\": 7, \"name\": \"sustain end\" }, { \"id\": 8, \"name\": \"sustain loop\" } ] },";
                expected_stream << " { \"id\": 4, \"name\": \"gain\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"signal\" }, { \"id\": 2, \"name\": \"gain\" } ], \"output_slots\": [ { \"id\": 3, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 5, \"name\": \"high-pass\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"signal\" }, { \"id\": 2, \"name\": \"cutoff\" }, { \"id\": 3, \"name\": \"resonance\" } ], \"output_slots\": [ { \"id\": 4, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 6, \"name\": \"low-pass\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"signal\" }, { \"id\": 2, \"name\": \"cutoff\" }, { \"id\": 3, \"name\": \"resonance\" } ], \"output_slots\": [ { \"id\": 4, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 7, \"name\": \"midi-in\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" } ], \"output_slots\": [ { \"id\": 1, \"name\": \"notes on\" }, { \"id\": 2, \"name\": \"notes off\" }, { \"id\": 3, \"name\": \"frequencies\" }, { \"id\": 4, \"name\": \"velocities\" } ] },";
                expected_stream << " { \"id\": 8, \"name\": \"mixer\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"amplitudes\" } ], \"output_slots\": [ { \"id\": 2, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 9, \"name\": \"multiplier\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"multipliers\" }, { \"id\": 2, \"name\": \"mutiplicands\" } ], \"output_slots\": [ { \"id\": 3, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 10, \"name\": \"noise\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" } ], \"output_slots\": [ { \"id\": 1, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 11, \"name\": \"oscillator\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"frequencies\" }, { \"id\": 2, \"name\": \"signal type\" } ], \"output_slots\": [ { \"id\": 3, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 12, \"name\": \"recorder\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"amplitudes\" } ], \"output_slots\": [ { \"id\": 2, \"name\": \"amplitudes\" } ] },";
                expected_stream << " { \"id\": 13, \"name\": \"sample\", \"icon\": \"<svg width=\\\"16\\\" height=\\\"16\\\" viewBox=\\\"0 0 16 16\\\" xmlns=\\\"http://www.w3.org/2000/svg\\\" fill=\\\"currentColor\\\">  <path id=\\\"mainPath\\\" d=\\\"M15 5.5a4.394 4.394 0 0 1-4 4.5 2.955 2.955 0 0 0-.2-1A3.565 3.565 0 0 0 14 5.5a3.507 3.507 0 0 0-7-.3A3.552 3.552 0 0 0 6 5a4.622 4.622 0 0 1 4.5-4A4.481 4.481 0 0 1 15 5.5zM5.5 6a4.5 4.5 0 1 0 0 9.001 4.5 4.5 0 0 0 0-9z\\\"/></svg>\", \"input_slots\": [ { \"id\": 0, \"name\": \"on/off\" }, { \"id\": 1, \"name\": \"play\" }, { \"id\": 2, \"name\": \"stop\" }, { \"id\": 3, \"name\": \"sample name\" } ], \"output_slots\": [ { \"id\": 4, \"name\": \"play at\" }, { \"id\": 5, \"name\": \"amplitudes\" } ] }";
                expected_stream << " ] }";

                std::wstring expected = expected_stream.str();
                REQUIRE(actual == expected);
            }
        }
    }
}

SCENARIO("get_projects returns no project when no project exists.", "[editor view]") {
    SECTION("Initialisation") {
        reinit();
    }
	
	GIVEN("No project exists") {
		WHEN("on_message_get_projects is called") {
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"get_projects\"}");

			THEN("an empty response is returned") {								
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"get_projects_callback\",";
                expected << L" \"body\": [";
                expected << L" ]";
                expected << L" }";
				
				REQUIRE(actual == expected.str());
			}
		}
    }
}

SCENARIO("Create a project then get_projects returns a project (created project is saved automatically).", "[editor view]") {
	SECTION("Initialisation") {
        reinit();
	}
	
    GIVEN("No project exists") {		
        WHEN("on_message_new_project is called") {
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"new_project\"}");

            THEN("A project id and name is returned") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"new_project_callback\",";
                expected << L" \"body\": {";
                expected << L" \"id\": 0,";
                expected << L" \"name\": \"Project 0\"";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());
            }
        }
        WHEN("on_message_get_projects is called") {
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"get_projects\"}");

            THEN("New project is returned (project created previously is automatically saved)") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"get_projects_callback\",";
                expected << L" \"body\": [";
                expected << L" {";
                expected << L" \"id\": 0,";
                expected << L" \"name\": \"Project 0\",";
                expected << L" \"description\": \"\"";
                expected << L" }";
                expected << L" ]";
                expected << L" }";

                REQUIRE(actual == expected.str());
            }
        }
    }
}

SCENARIO("Create a project, add a module, export project then import it and retrieve project and module.", "[editor view]") {
    SECTION("Initialisation") {
        reinit();
    }

    GIVEN("No project exists") {
        WHEN("on_message_new_project is called") {
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"new_project\"}");

            THEN("A project name is returned") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"new_project_callback\",";
                expected << L" \"body\": {";
                expected << L" \"id\": 0,";
                expected << L" \"name\": \"Project 0\"";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());
            }
        }
        WHEN("on_add_module is called") {
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"add_module\", \"body\": { \"type\": \"midi-in\", \"position\": { \"x\": 1, \"y\": 2, \"z\": 3 } }}");

            THEN("A module id is returned") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"add_module_callback\",";
                expected << L" \"body\": {";
                expected << L" \"module\": { \"id\": 0 }";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());
            }
        }
        WHEN("on_message_export_project is called") {
			//FIXME: do not hardcode path.
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"export_project\",  \"body\": {\"path\": \"c:/tmp/project0.json\"}}");

            THEN("The project is saved to disk.") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"export_project_callback\",";
                expected << L" \"body\": {";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());
            }
        }
        WHEN("on_message_import_project is called") {
            //FIXME: do not hardcode path.
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"import_project\",  \"body\": {\"path\": \"c:/tmp/project0.json\"}}");
			
            THEN("The project is imported from disk.") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"import_project_callback\",";
                expected << L" \"body\": {";
                expected << L" \"id\": 0,";
                expected << L" \"name\": \"Project 0\",";
                expected << L" \"description\": \"\"";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());
            }
            AND_THEN("Module is also imported.") {
                auto module_view = *(application_.modules_views_instances_)[0];
                REQUIRE(module_view.id == 0);
                REQUIRE(module_view.position[0] == 1);
                REQUIRE(module_view.position[1] == 2);
                REQUIRE(module_view.position[2] == 3);
            }
        }
    }
}

SCENARIO("Create a project, add a module, create another project, then load first one and retrieve project and module.", "[editor view]") {
    SECTION("Initialisation") {
        reinit();
    }

    GIVEN("A first project is created and a module inserted in it and a second one is created again.") {
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"new_project\"}");
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"add_module\", \"body\": { \"type\": \"midi-in\", \"position\": { \"x\": 2, \"y\": 3, \"z\": 4 } }}");
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"new_project\"}");

        WHEN("We load the first project") {
            //FIXME: do not hardcode path.
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"load_project\",  \"body\": {\"id\": 0}}");

            THEN("The first project headers are loaded from disk.") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"load_project_callback\",";
                expected << L" \"body\": {";
                expected << L" \"id\": 0,";
                expected << L" \"name\": \"Project 0\",";
                expected << L" \"description\": \"\"";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());

                AND_THEN("The module previously added is also imported.") {
                    auto module_view = *(application_.modules_views_instances_)[0];
                    REQUIRE(module_view.id == 0);
                    REQUIRE(module_view.position[0] == 2);
                    REQUIRE(module_view.position[1] == 3);
                    REQUIRE(module_view.position[2] == 4);
                }
            }
        }
    }
}

SCENARIO("Create a project, add 1 module and set a parameter value.", "[editor view]") {
    SECTION("Initialisation") {
        reinit();
        editor_view_.set_host_callback(&mock_callback_);
    }

    GIVEN("A project is created and a module is inserted.") {
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"new_project\"}");
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"add_module\", \"body\": { \"type\": \"midi-in\", \"position\": { \"x\": 0, \"y\": 1, \"z\": 2 } }}");

        WHEN("We add a parameter.") {
            reinit();
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"set_module_parameter_value\",  \"body\": {\"module_id\": 0, \"parameter_id\": 0, \"value\": \"0\"}}");

            THEN("The parameter value is set to its new value.") {
                auto midi_in = reinterpret_cast<processor::module::midi_input*>(&(application_.get_processor_by_id(0)));
                bool actual = midi_in->is_on;

                REQUIRE(actual == false);
            }
        }
    }
}

SCENARIO("Create a project, add 2 modules and a link between them, save then load it to verify saving.", "[editor view]") {
    SECTION("Initialisation") {
        reinit();
    }

    GIVEN("A project is created, 2 modules are inserted and a link is created between them.") {
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\": \"new_project\"}");
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"add_module\", \"body\": { \"type\": \"midi-in\", \"position\": { \"x\": 0, \"y\": 1, \"z\": 2 } }}");
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"add_module\", \"body\": { \"type\": \"audio-out\", \"position\": { \"x\": 3, \"y\": 4, \"z\": 5 } }}");
        editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"link_modules\", \"body\": { \"source_module\": 0, \"source_slot\": 3, \"target_module\": 1, \"target_slot\": 1 }}");

        WHEN("We load the project") {
            reinit();
            editor_view_.on_message_received(&mock_core_web_view_, L"{\"action\":\"load_project\",  \"body\": {\"id\": 0}}");

            THEN("The project headers are loaded from disk.") {
                LPCWSTR actual = mock_core_web_view_.get_last_message();
                std::wstringstream expected;
                expected << L"{";
                expected << L" \"action\": \"load_project_callback\",";
                expected << L" \"body\": {";
                expected << L" \"id\": 0,";
                expected << L" \"name\": \"Project 0\",";
                expected << L" \"description\": \"\"";
                expected << L" }";
                expected << L" }";

                REQUIRE(actual == expected.str());

                AND_THEN("The modules previously added are also loaded.") {
                    REQUIRE(application_.nb_modules_ == 2);
                    REQUIRE(application_.nb_links_ == 1);
                    
                    auto module_view = *(application_.modules_views_instances_)[0];
                    REQUIRE(module_view.id == 0);
                    REQUIRE(module_view.position[0] == 0);
                    REQUIRE(module_view.position[1] == 1);
                    REQUIRE(module_view.position[2] == 2);
                    module_view = *(application_.modules_views_instances_)[1];
                    REQUIRE(module_view.position[0] == 3);
                    REQUIRE(module_view.position[1] == 4);
                    REQUIRE(module_view.position[2] == 5);
                    auto link = application_.links_[0];
                    REQUIRE(link.source_module->get_id() == 0);
                    REQUIRE(link.target_module->get_id() == 1);
                    REQUIRE(link.source_slot_id == 3);
                    REQUIRE(link.target_slot_id == 1);
                }
            }
        }
    }
}