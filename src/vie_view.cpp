#include "vie_view.hpp"

using namespace Steinberg;
using namespace Vst;

using namespace nlohmann;

namespace live::tritone::vie
{
	vie_view::vie_view(host_callback* host_callback) :
		nb_ref_(0),
		ptr_frame_(nullptr),
		width_(1024),
		height_(600)
	{
		editor_view_.set_host_callback(host_callback);
	}

	tresult __stdcall vie_view::queryInterface(const TUID iid, void** obj)
	{
		if (FUnknownPrivate::iidEqual(iid, FUnknown::iid))
		{
			addRef();
			*obj = this;
			return kResultOk;
		}

		return kResultFalse;
	}

	uint32 __stdcall

	vie_view::addRef()
	{
		return ++nb_ref_;
	}

	uint32 __stdcall

	vie_view::release()
	{
		//FIXME: Try to delete this if 0.
		return --nb_ref_;
	}

	tresult __stdcall vie_view::isPlatformTypeSupported(FIDString type)
	{
		if (
			strcmp(type, "HWND") == 0)
		{
			return kResultTrue;
		}

		return kResultFalse;
	}

	tresult __stdcall vie_view::attached(void* parent, FIDString /*type*/)
	{
		editor_view_.attached(parent);

		return kResultTrue;
	}

	tresult __stdcall vie_view::removed()
	{
		editor_view_.removed();

		return kResultTrue;
	}

	tresult __stdcall vie_view::onWheel(float /*distance*/)
	{
		return kResultOk;
	}

	tresult __stdcall vie_view::onKeyDown(char16
	                    key, int16 /*keyCode*/, int16 /*modifiers*/)
	{
		return kResultOk;
	}

	tresult __stdcall vie_view::onKeyUp(char16 /*key*/, int16 /*keyCode*/, int16 /*modifiers*/)
	{
		return kResultOk;
	}

	tresult __stdcall vie_view::getSize(ViewRect* size)
	{
		size->left = 0;
		size->top = 0;
		size->right = width_;
		size->bottom = height_;

		return kResultOk;
	}

	tresult __stdcall vie_view::onSize(ViewRect* new_size)
	{
		return kResultOk;
	}

	tresult __stdcall vie_view::onFocus(TBool /*state*/)
	{
		return kResultOk;
	}

	tresult __stdcall vie_view::setFrame(IPlugFrame* frame)
	{
		ptr_frame_ = frame;

		return kResultOk;
	}

	tresult __stdcall vie_view::canResize()
	{
		return kResultTrue;
	}

	tresult __stdcall vie_view::checkSizeConstraint(ViewRect* /*rect*/)
	{
		return kResultOk;
	}

	tresult vie_view::set_state(IBStream* state)
	{
		return kResultTrue;
	}

	tresult vie_view::get_state(IBStream* state)
	{
		return kResultTrue;
	}

	void vie_view::set_host_callback(host_callback* callback)
	{
		editor_view_.set_host_callback(callback);
	}
	
	void vie_view::initialize()
	{
		editor_view_.initialize();
	}

	nlohmann::json vie_view::serialize()
	{
		return json();
	}

	void vie_view::load(nlohmann::json instrument)
	{
		parse_modules(instrument["modules"]);
		parse_links(instrument["links"]);
		parse_ui(instrument["ui"]);
	}

	void vie_view::add_module(nlohmann::json module)
	{
		parse_module(module);
	}

	void vie_view::render()
	{
	}

	void vie_view::parse_modules(json modules_definitions)
	{
		for (auto& [index, module_definition] : modules_definitions.items()) {
			parse_module(module_definition);
		}
	}

	void vie_view::parse_module(json module_definition)
	{
		const std::string type = module_definition["type"];
	}

	void vie_view::parse_links(json links_definitions)
	{
		for (auto& [index, link_definition] : links_definitions.items()) {
			parse_link(link_definition);
		}
	}

	void vie_view::parse_link(json link_definition)
	{
	}

	void vie_view::parse_ui(json ui_definitions)
	{
		for (auto& [index, ui_definition] : ui_definitions.items()) {
		}
	}
} // namespace