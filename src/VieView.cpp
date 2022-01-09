#include "VieView.h"

#include "Application.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <nuklear.h>

#include <fstream>

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#include "SwapChain.h"


using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace Diligent;

static const char* VSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn) 
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)";

// Pixel shader simply outputs interpolated vertex color
static const char* PSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";

namespace live::tritone::vie {
	Diligent::IEngineFactoryVk* VieView::pFactoryVk;
	Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  VieView::m_pDevice;
	Diligent::RefCntAutoPtr<Diligent::IDeviceContext> VieView::m_pImmediateContext;

	VieView::VieView(FrequencyParameter* frequencyParameter) :
		nbRef_(0),
		frame_(nullptr),
		width_(1024),
		height_(600),
		frequencyParameter_(frequencyParameter),
		isRendererRunning(true),
		m_pNkDlgCtx(nullptr),
		m_pNkCtx(nullptr)
	{
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, MessageProc,
						   0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"VIE", NULL };
		RegisterClassEx(&wcex);

		//TODO: Move this static initialization to another class.
		if (pFactoryVk == nullptr) {
			pFactoryVk = GetEngineFactoryVk();
			EngineVkCreateInfo EngineCI;
			pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice, &m_pImmediateContext);
		}
	}

	VieView::~VieView()
	{
		UnregisterClass(L"VIE", GetModuleHandle(NULL));
	}

	LRESULT CALLBACK VieView::MessageProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		live::tritone::vie::VieView* vieView = (live::tritone::vie::VieView*)(LONG_PTR)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		if (vieView) {
			vieView->HandleWin32Message(hwnd, msg, wparam, lparam);
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	LONG_PTR WINAPI VieView::HandleWin32Message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		int result = 0;

		if (m_pNkCtx) {
			switch (msg)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			{
				int down = !((lparam >> 31) & 1);
				int ctrl = GetKeyState(VK_CONTROL) & (1 << 15);

				switch (wparam)
				{
				case VK_SHIFT:
				case VK_LSHIFT:
				case VK_RSHIFT:
					nk_input_key(m_pNkCtx, NK_KEY_SHIFT, down);
					result = 1;
					break;
				case VK_DELETE:
					nk_input_key(m_pNkCtx, NK_KEY_DEL, down);
					result = 1;
					break;
				case VK_RETURN:
					nk_input_key(m_pNkCtx, NK_KEY_ENTER, down);
					result = 1;
					break;
				case VK_TAB:
					nk_input_key(m_pNkCtx, NK_KEY_TAB, down);
					result = 1;
					break;
				case VK_LEFT:
					if (ctrl)
						nk_input_key(m_pNkCtx, NK_KEY_TEXT_WORD_LEFT, down);
					else
						nk_input_key(m_pNkCtx, NK_KEY_LEFT, down);
					result = 1;
					break;
				case VK_RIGHT:
					if (ctrl)
						nk_input_key(m_pNkCtx, NK_KEY_TEXT_WORD_RIGHT, down);
					else
						nk_input_key(m_pNkCtx, NK_KEY_RIGHT, down);
					result = 1;
					break;
				case VK_BACK:
					nk_input_key(m_pNkCtx, NK_KEY_BACKSPACE, down);
					result = 1;
					break;
				case VK_HOME:
					nk_input_key(m_pNkCtx, NK_KEY_TEXT_START, down);
					result = 1;
					break;
				case VK_END:
					nk_input_key(m_pNkCtx, NK_KEY_TEXT_END, down);
					result = 1;
					break;
				case VK_NEXT:
					nk_input_key(m_pNkCtx, NK_KEY_SCROLL_DOWN, down);
					result = 1;
					break;
				case VK_PRIOR:
					nk_input_key(m_pNkCtx, NK_KEY_SCROLL_UP, down);
					result = 1;
					break;
				case 'C':
					if (ctrl)
					{
						nk_input_key(m_pNkCtx, NK_KEY_COPY, down);
						result = 1;
					}
					break;
				case 'V':
					if (ctrl)
					{
						nk_input_key(m_pNkCtx, NK_KEY_PASTE, down);
						result = 1;
					}
					break;
				case 'X':
					if (ctrl)
					{
						nk_input_key(m_pNkCtx, NK_KEY_CUT, down);
						result = 1;
					}
					break;
				case 'Z':
					if (ctrl)
					{
						nk_input_key(m_pNkCtx, NK_KEY_TEXT_UNDO, down);
						result = 1;
					}
					break;
				case 'Y':
					if (ctrl)
					{
						nk_input_key(m_pNkCtx, NK_KEY_TEXT_REDO, down);
						result = 1;
					}
					break;
				default:
					break;
				}
			}
			break;
			case WM_ERASEBKGND:
				result = 1;
				break;
			case WM_PAINT:
				result = 1;
				break;
			case WM_MOUSEMOVE:
				nk_input_motion(m_pNkCtx, (short)LOWORD(lparam), (short)HIWORD(lparam));
				result = 1;
				break;
			case WM_LBUTTONDOWN:
				nk_input_button(m_pNkCtx, NK_BUTTON_LEFT, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
				SetCapture(hwnd);
				result = 1;
				break;
			case WM_LBUTTONUP:
				nk_input_button(m_pNkCtx, NK_BUTTON_LEFT, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
				ReleaseCapture();
				result = 1;
				break;
			case WM_MBUTTONDOWN:
				nk_input_button(m_pNkCtx, NK_BUTTON_MIDDLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
				SetCapture(hwnd);
				result = 1;
				break;
			case WM_MBUTTONUP:
				nk_input_button(m_pNkCtx, NK_BUTTON_MIDDLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
				ReleaseCapture();
				result = 1;
				break;
			case WM_RBUTTONDOWN:
				nk_input_button(m_pNkCtx, NK_BUTTON_RIGHT, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
				SetCapture(hwnd);
				result = 1;
				break;
			case WM_RBUTTONUP:
				nk_input_button(m_pNkCtx, NK_BUTTON_RIGHT, (short)LOWORD(lparam), (short)HIWORD(lparam), 0);
				ReleaseCapture();
				result = 1;
				break;
			case WM_MOUSEWHEEL:
				nk_input_scroll(m_pNkCtx, nk_vec2(0, (float)(short)HIWORD(wparam) / WHEEL_DELTA));
				result = 1;
				break;
			case WM_LBUTTONDBLCLK:
				nk_input_button(m_pNkCtx, NK_BUTTON_DOUBLE, (short)LOWORD(lparam), (short)HIWORD(lparam), 1);
				result = 1;
				break;
			case WM_CHAR:
				if (wparam >= 32)
				{
					nk_input_unicode(m_pNkCtx, (nk_rune)wparam);
					result = 1;
				}
				break;
			}
		}

		return result;
	}

	tresult PLUGIN_API VieView::queryInterface(const TUID iid, void** obj)
	{
		if (::Steinberg::FUnknownPrivate::iidEqual(iid, ::FUnknown::iid)) {
			addRef();
			*obj = static_cast<VieView*>(this);
			return ::Steinberg::kResultOk;
		}

		return kResultFalse;
	}

	uint32 PLUGIN_API VieView::addRef()
	{
		return ++nbRef_;
	}

	uint32 PLUGIN_API VieView::release()
	{
		return --nbRef_;
	}

	tresult PLUGIN_API VieView::isPlatformTypeSupported(FIDString type)
	{
		if (strcmp(type, "HWND") == 0) {
			return kResultTrue;
		}

		return kResultFalse;
	}

	tresult PLUGIN_API VieView::attached(void* parent, FIDString /*type*/)
	{
		RECT WndRect = { 0, 0, 1024, 768 };

		std::wstringstream TitleSS;
		TitleSS << L"Virtual Instrument Engine";
		auto Title = TitleSS.str();

		AdjustWindowRect(&WndRect, WS_OVERLAPPEDWINDOW, FALSE);
		m_Window.hWnd = CreateWindow(L"VIE", Title.c_str(),
			WS_CHILD | WS_VISIBLE, 
			0, 0,
			WndRect.right - WndRect.left, WndRect.bottom - WndRect.top,
			(HWND) parent, NULL, GetModuleHandle(NULL), NULL);
		
		if (!m_Window.hWnd)
		{
			MessageBox(NULL, L"Cannot create window", L"Error", MB_OK | MB_ICONERROR);
			return -1;
		}

		SetWindowLongPtr(m_Window.hWnd, GWLP_USERDATA, (__int3264)(LONG_PTR)this);

		ShowWindow(m_Window.hWnd, 1);

		if (!InitializeDiligentEngine())
			return -1;

		CreateResources();

		InitializeNuklear();

		auto viewRendererThread = [](VieView* view, bool const* isRunning) {
			using ms = std::chrono::milliseconds;
			while (*isRunning) {
				view->render();
				std::this_thread::sleep_for(ms(15));
			}
		};
		rendererThread.reset(new std::thread(viewRendererThread, this, &isRendererRunning));

		return kResultTrue;
	}

	tresult PLUGIN_API VieView::removed()
	{
		//Stop renderer loop.
		isRendererRunning = false;

		if (rendererThread) {
			rendererThread->join();
			rendererThread.reset();
			rendererThread = nullptr;
		}

		DestroyWindow(m_Window.hWnd);
		m_Window.hWnd = nullptr;

		return kResultTrue;
	}

	tresult PLUGIN_API VieView::onWheel(float /*distance*/)
	{
		return kResultOk;
	}

	tresult PLUGIN_API VieView::onKeyDown(char16 key, int16 /*keyCode*/, int16 /*modifiers*/)
	{
		return kResultOk;
	}

	tresult PLUGIN_API VieView::onKeyUp(char16 /*key*/, int16 /*keyCode*/, int16 /*modifiers*/)
	{
		return kResultOk;
	}

	tresult PLUGIN_API VieView::getSize(ViewRect* size)
	{
		size->left = 0;
		size->top = 0;
		size->right = width_;
		size->bottom = height_;

		return kResultOk;
	}

	tresult PLUGIN_API VieView::onSize(ViewRect* newSize)
	{
		nk_diligent_resize(m_pNkDlgCtx, m_pImmediateContext, newSize->getWidth(), newSize->getHeight());
		if (m_Window.pSwapChain)
			m_Window.pSwapChain->Resize(newSize->getWidth(), newSize->getHeight());

		return kResultOk;
	}

	tresult PLUGIN_API VieView::onFocus(TBool /*state*/)
	{
		return kResultOk;
	}

	tresult PLUGIN_API VieView::setFrame(IPlugFrame* frame)
	{
		frame_ = frame;

		return kResultOk;
	}

	tresult PLUGIN_API VieView::canResize()
	{
		return kResultTrue;
	}
	
	tresult PLUGIN_API VieView::checkSizeConstraint(ViewRect* /*rect*/)
	{
		return kResultOk;
	}

	Steinberg::tresult PLUGIN_API VieView::setState(IBStream* state) {
		return kResultTrue;
	}

	Steinberg::tresult PLUGIN_API VieView::getState(IBStream* state) {
		return kResultTrue;
	}

	void VieView::render() {
		nk_input_end(m_pNkCtx);
		if (m_pNkCtx) {
			m_ui.render(m_pNkCtx);
			m_nodeEditor.render(m_pNkCtx);
		}
		nk_input_begin(m_pNkCtx);

		Render();
		Present();
	}

	bool VieView::InitializeDiligentEngine()
	{		
		if (!m_Window.pSwapChain && m_Window.hWnd != nullptr)
		{
			SwapChainDesc SCDesc;
			Win32NativeWindow Window{ m_Window.hWnd };
			pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_Window.pSwapChain);
			SCDesc.IsPrimary = false;
		}

		return true;
	}

	void VieView::CreateResources()
	{
		// Pipeline state object encompasses configuration of all GPU stages

		GraphicsPipelineStateCreateInfo PSOCreateInfo;

		// Pipeline state name is used by the engine to report issues.
		// It is always a good idea to give objects descriptive names.
		PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

		// This is a graphics pipeline
		PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

		// clang-format off
		// This tutorial will render to a single render target
		PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
		// Set render target format which is the format of the swap chain's color buffer
		ISwapChain* pSwapChain = m_Window.pSwapChain;
		PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = pSwapChain->GetDesc().ColorBufferFormat;
		// Use the depth buffer format from the swap chain
		PSOCreateInfo.GraphicsPipeline.DSVFormat = pSwapChain->GetDesc().DepthBufferFormat;
		// Primitive topology defines what kind of primitives will be rendered by this pipeline state
		PSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		// No back face culling for this tutorial
		PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
		// Disable depth testing
		PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
		// clang-format on

		ShaderCreateInfo ShaderCI;
		// Tell the system that the shader source code is in HLSL.
		// For OpenGL, the engine will convert this into GLSL under the hood
		ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
		// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
		ShaderCI.UseCombinedTextureSamplers = true;
		// Create a vertex shader
		RefCntAutoPtr<IShader> pVS;
		{
			ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Triangle vertex shader";
			ShaderCI.Source = VSSource;
			m_pDevice->CreateShader(ShaderCI, &pVS);
		}

		// Create a pixel shader
		RefCntAutoPtr<IShader> pPS;
		{
			ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Triangle pixel shader";
			ShaderCI.Source = PSSource;
			m_pDevice->CreateShader(ShaderCI, &pPS);
		}

		// Finally, create the pipeline state
		PSOCreateInfo.pVS = pVS;
		PSOCreateInfo.pPS = pPS;
		m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
	}

	void VieView::InitializeNuklear() {
		constexpr Uint32 NuklearMaxVBSize = 512 * 1024;
		constexpr Uint32 NuklearMaxIBSize = 128 * 1024;

		const auto& SCDesc = m_Window.pSwapChain->GetDesc();

		m_pNkDlgCtx = nk_diligent_init(m_pDevice, SCDesc.Width, SCDesc.Height, SCDesc.ColorBufferFormat, SCDesc.DepthBufferFormat, NuklearMaxVBSize, NuklearMaxIBSize);
		m_pNkCtx = nk_diligent_get_nk_ctx(m_pNkDlgCtx);

		nk_font_atlas* atlas = nullptr;
		nk_diligent_font_stash_begin(m_pNkDlgCtx, &atlas);
		nk_diligent_font_stash_end(m_pNkDlgCtx, m_pImmediateContext);

		m_uiStyle.set_style(m_pNkCtx, UserInterfaceStyle::theme::THEME_DARK);
	}

	void VieView::Render()
	{
		ITextureView* pRTV = m_Window.pSwapChain->GetCurrentBackBufferRTV();
		ITextureView* pDSV = m_Window.pSwapChain->GetDepthBufferDSV();
		m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		const float ClearColor[] = { 0.350f, 0.350f, 0.350f, 1.0f };
		m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		m_pImmediateContext->SetPipelineState(m_pPSO);

		DrawAttribs drawAttrs;
		drawAttrs.NumVertices = 3; // Render 3 vertices
		m_pImmediateContext->Draw(drawAttrs);

		nk_diligent_render(m_pNkDlgCtx, m_pImmediateContext, NK_ANTI_ALIASING_ON);
	}

	void VieView::Present()
	{
		if (!m_Window.pSwapChain)
			return;

		m_Window.pSwapChain->Present();
	}
} // namespace
