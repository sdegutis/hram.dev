#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <shlwapi.h>
#include <shlobj_core.h>
#include <stdint.h>
#include <atlstr.h>

#include <string>

#include "PixelShader.h"
#include "VertexShader.h"
#include <lua/lua.hpp>

#include "Image.h"

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")


HCURSOR hCursor;
HWND subwin;




Image screen1{ 320,180 };
Image screen2{ 320 * 2,180 * 2 };
Image* screen = &screen1;


int scale = 3;
int winw = screen->resw * scale;
int winh = screen->resh * scale;

ID3D11Device* device;
ID3D11DeviceContext* devicecontext;
IDXGISwapChain* swapchain;

ID3D11Texture2D* framebuffer;
ID3D11RenderTargetView* framebufferRTV;

int subx = 0;
int suby = 0;
int subw = 0;
int subh = 0;

void checkSubWindow() {
	subw = screen->resw;
	subh = screen->resh;
	scale = 1;

	while (
		subw + screen->resw <= winw &&
		subh + screen->resh <= winh)
	{
		subw += screen->resw;
		subh += screen->resh;
		scale++;
	}

	subx = winw / 2 - subw / 2;
	suby = winh / 2 - subh / 2;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int diffw;
int diffh;

Image s(4, 4);


#include "util.h"

#include <asmjit/host.h>
#include <stdio.h>
#include <asmtk/asmtk.h>

using namespace asmjit;
using namespace asmtk;

typedef int (*Func)(uint8_t*);

//#include <lpeg/>


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {

	openConsole();


	JitRuntime rt;

	// Holds code and relocation information during code generation.
	CodeHolder code;

	// Code holder must be initialized before it can be used. The simples way to initialize
	// it is to use 'Environment' from JIT runtime, which matches the target architecture,
	// operating system, ABI, and other important properties.
	code.init(rt.environment(), rt.cpuFeatures());

	// Emitters can emit code to CodeHolder - let's create 'x86::Assembler', which can emit
	// either 32-bit (x86) or 64-bit (x86_64) code. The following line also attaches the
	// assembler to CodeHolder, which calls 'code.attach(&a)' implicitly.
	x86::Assembler a(&code);

	AsmParser p(&a);

	// Parse some assembly.
	Error err = p.parse(R"(
		movzx eax, byte [rcx+1]
		ret
	)");

	// Error handling (use asmjit::ErrorHandler for more robust error handling).
	if (err) {
		printf("ERROR: %08x (%s)\n", err, DebugUtils::errorAsString(err));
		return 1;
	}


	// 'x86::Assembler' is no longer needed from here and can be destroyed or explicitly
	// detached via 'code.detach(&a)' - which detaches an attached emitter from code holder.

	// Now add the generated code to JitRuntime via JitRuntime::add(). This function would
	// copy the code from CodeHolder into memory with executable permission and relocate it.
	Func fn;
	err = rt.add(&fn, &code);

	// It's always a good idea to handle errors, especially those returned from the Runtime.
	if (err) {
		printf("AsmJit failed: %s\n", DebugUtils::errorAsString(err));
		return 1;
	}

	// CodeHolder is no longer needed from here and can be safely destroyed. The runtime now
	// holds the relocated function, which we have generated, and controls its lifetime. The
	// function will be freed with the runtime, so it's necessary to keep the runtime around.
	//
	// Use 'code.reset()' to explicitly free CodeHolder's content when necessary.

	auto foo = (uint8_t*)malloc(3);
	foo[0] = 11;
	foo[1] = 22;
	foo[2] = 33;

	// Execute the generated function and print the resulting '1', which it moves to 'eax'.
	int resul = fn(foo);
	printf("asm = %llu\n", resul);

	// All classes use RAII, all resources will be released before `main()` returns, the
	// generated function can be, however, released explicitly if you intend to reuse or
	// keep the runtime alive, which you should in a production-ready code.
	rt.release(fn);



	checkSubWindow();

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dostring(L, R"(

print(package.path)
print(1)
local yes, foo = pcall(require, 'lpeg')
print(2)
print('worked?', yes, foo)

require 'foo'

)");


	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"HRAM Window Class";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0x11, 0x11, 0x11));
	RegisterClass(&wc);

	WNDCLASS wc2 = { };
	wc2.lpfnWndProc = WindowProc2;
	wc2.hInstance = hInstance;
	wc2.lpszClassName = L"HRAM SubWindow Class";
	RegisterClass(&wc2);



	RECT winbox;
	winbox.left = GetSystemMetrics(SM_CXSCREEN) / 2 - winw / 2;
	winbox.top = GetSystemMetrics(SM_CYSCREEN) / 2 - winh / 2;
	winbox.right = winbox.left + winw;
	winbox.bottom = winbox.top + winh;
	AdjustWindowRectEx(&winbox, WS_OVERLAPPEDWINDOW, false, 0);

	diffw = (winbox.right - winbox.left) - winw;
	diffh = (winbox.bottom - winbox.top) - winh;

	HWND hwnd = CreateWindowExW(
		0, L"HRAM Window Class", L"HRAM", WS_OVERLAPPEDWINDOW,
		winbox.left,
		winbox.top,
		winbox.right - winbox.left,
		winbox.bottom - winbox.top,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) { return 0; }


	const BOOL isDarkMode = true;
	HRESULT result = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));


	subwin = CreateWindowExW(
		0, L"HRAM SubWindow Class", L"", WS_CHILD | WS_VISIBLE,
		subx, suby, subw, subh,
		hwnd, 0, 0, NULL);
	if (subwin == NULL) { return 0; }






	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 2;
	swapchaindesc.OutputWindow = subwin;
	swapchaindesc.Windowed = TRUE;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext);


	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);
	device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV);


	ID3D11VertexShader* vertexshader;
	device->CreateVertexShader(MyVertexShader, sizeof(MyVertexShader), 0, &vertexshader);

	ID3D11PixelShader* pixelshader;
	device->CreatePixelShader(MyPixelShader, sizeof(MyPixelShader), 0, &pixelshader);

	D3D11_RASTERIZER_DESC rasterizerdesc = { D3D11_FILL_SOLID, D3D11_CULL_BACK };
	ID3D11RasterizerState* rasterizerstate;
	device->CreateRasterizerState(&rasterizerdesc, &rasterizerstate);

	D3D11_VIEWPORT viewport = { 0, 0, subw, subh, 0, 1 };
	devicecontext->RSSetViewports(1, &viewport);

	screen1.create(device, devicecontext);
	screen2.create(device, devicecontext);





	//D3D11_BLEND_DESC bd = {};
	////bd.AlphaToCoverageEnable = true;
	//bd.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//bd.RenderTarget->BlendEnable = TRUE;

	//bd.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	//bd.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//bd.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	////bd.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	////bd.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	////bd.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;

	//ID3D11BlendState* bstate;
	//device->CreateBlendState(&bd, &bstate);
	//devicecontext->OMSetBlendState(bstate, NULL, 0xffffffff);





	s.create(device, devicecontext);

	//for (int y = 0; y < 4; y++) {
	//	for (int x = 0; x < 4; x++) {
	//		//RGBA

	//		s.pset(x, y, RGB(rand() % 0xff, rand() % 0xff, rand() % 0xff) | 0x7f);

	//		//screen1.pset(3 + x, 10 + y, 0x33'ff'99'00);
	//	}
	//}

	s.fillrect(0, 0, 4, 4, 0xff9900);
	s.fillrect(1, 1, 2, 2, 0xff99ff);

	s.copyTo(screen1, 10, 10);
	s.copyTo(screen1, 0, 0, 1, 1, 2, 2);



	D3D11_SAMPLER_DESC samplerdesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP };
	ID3D11SamplerState* samplerstate;
	device->CreateSamplerState(&samplerdesc, &samplerstate);









	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);






	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {


			devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


			devicecontext->VSSetShader(vertexshader, nullptr, 0);

			devicecontext->RSSetState(rasterizerstate);

			devicecontext->PSSetShader(pixelshader, nullptr, 0);
			devicecontext->PSSetShaderResources(0, 1, &screen->textureSRV);
			devicecontext->PSSetSamplers(0, 1, &samplerstate);

			devicecontext->OMSetRenderTargets(1, &framebufferRTV, nullptr);
			//devicecontext->OMSetBlendState(bstate, NULL, 0xffffffff);

			devicecontext->Draw(4, 0);

			swapchain->Present(1, 0);
		}
	}

	return 0;
}

WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };

int first = 1;

void resetBuffers() {
	D3D11_VIEWPORT viewport = { 0, 0, subw, subh, 0, 1 };
	devicecontext->RSSetViewports(1, &viewport);

	devicecontext->Flush();

	framebufferRTV->Release();
	framebuffer->Release();

	swapchain->ResizeBuffers(0, subw, subh, DXGI_FORMAT_UNKNOWN, 0);

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);
	device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}

	case WM_CHAR: {
		printf("char: %llu\n", wParam);
		break;
	}

	case WM_KEYUP: {
		printf("key up %llu\n", wParam);
		break;
	}

	case WM_SYSKEYDOWN: {
		printf("sys key down %llu\n", wParam);
		return 0;
	}

	case WM_SYSKEYUP: {
		printf("sys key up %llu\n", wParam);
		return 0;
	}

	case WM_KEYDOWN: {

		printf("key down %llu\n", wParam);

		if (wParam == VK_F11) {
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
			if (dwStyle & WS_OVERLAPPEDWINDOW) {
				MONITORINFO mi = { sizeof(mi) };
				if (GetWindowPlacement(hwnd, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
				{
					SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
					SetWindowPos(hwnd, HWND_TOP,
						mi.rcMonitor.left, mi.rcMonitor.top,
						mi.rcMonitor.right - mi.rcMonitor.left,
						mi.rcMonitor.bottom - mi.rcMonitor.top,
						SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				}
			}
			else {
				SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
				SetWindowPlacement(hwnd, &g_wpPrev);
				SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			}
			return 0;
		}



		break;
	}

	case WM_GETMINMAXINFO: {
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		printf("checking min size\n");
		lpMMI->ptMinTrackSize.x = screen->resw + diffw;
		lpMMI->ptMinTrackSize.y = screen->resh + diffh;
		return 0;
	}

	case WM_SIZE:
		winw = LOWORD(lParam);
		winh = HIWORD(lParam);
		checkSubWindow();
		SetWindowPos(subwin, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		if (first) {
			first = 0;
			return 0;
		}

		resetBuffers();

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int mousex;
int mousey;

LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_SETCURSOR: {
		if (LOWORD(lParam) == HTCLIENT) {
			SetCursor(NULL);
			return TRUE;
		}
		break;
	}

	case WM_LBUTTONUP: {
		printf("left button up\n");
		return 0;
	}

	case WM_LBUTTONDOWN: {
		printf("left button down\n");

		if (screen == &screen1)
			screen = &screen2;
		else screen = &screen1;

		checkSubWindow();
		SetWindowPos(subwin, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED);
		resetBuffers();

		return 0;
	}

	case WM_RBUTTONUP: {
		printf("right button up\n");
		return 0;
	}

	case WM_RBUTTONDOWN: {
		printf("right button down\n");
		return 0;
	}

	case WM_MBUTTONUP: {
		printf("middle button up\n");
		return 0;
	}

	case WM_MBUTTONDOWN: {
		printf("middle button down\n");
		return 0;
	}

	case WM_MOUSEWHEEL: {
		auto zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		printf("wheel %d\n", zDelta);
		return 0;
	}

	case WM_MOUSEMOVE: {

		auto xPos = GET_X_LPARAM(lParam) / scale;
		auto yPos = GET_Y_LPARAM(lParam) / scale;

		if (xPos != mousex || yPos != mousey) {
			mousex = xPos;
			mousey = yPos;

			printf("move %d %d\n", mousex, mousey);
			//screen->pset(mousex, mousey, RGB(rand() % 0xff, rand() % 0xff, rand() % 0xff));

			s.copyTo(*screen, mousex, mousey);

			return 0;
		}

	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
