#define UNICODE
#define _UNICODE

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

#include <asmjit/host.h>
#include <asmtk/asmtk.h>


#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")


HCURSOR hCursor;
HWND subwin;

int resw = 320;
int resh = 180;

int scale = 3;
int winw = resw * scale;
int winh = resh * scale;

ID3D11Device* device;
ID3D11DeviceContext* devicecontext;
IDXGISwapChain* swapchain;

ID3D11Texture2D* texture;

ID3D11Texture2D* framebuffer;
ID3D11RenderTargetView* framebufferRTV;

int subx = 0;
int suby = 0;
int subw = 0;
int subh = 0;

void checkSubWindow() {
	subw = resw;
	subh = resh;
	scale = 1;

	while (
		subw + resw <= winw &&
		subh + resh <= winh)
	{
		subw += resw;
		subh += resh;
		scale++;
	}

	subx = winw / 2 - subw / 2;
	suby = winh / 2 - subh / 2;
}

uint32_t texturedata[320 * 180];

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int diffw;
int diffh;

std::string getUserDir() {
	PWSTR thepath;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &thepath);
	auto s = std::string(CW2A(thepath, CP_UTF8));
	CoTaskMemFree(thepath);
	return s;
}

std::string getExeName() {
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	return std::string(CW2A(szFileName, CP_UTF8));
}

void openConsole() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}


using namespace asmjit;
using namespace asmtk;
typedef int (*Func)(int, int);


int luaopen_foo(lua_State* L) {
	printf("called!\n");
	return 0;
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {

	lua_State* L = luaL_newstate();

	openConsole();
	{

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

		// Use the x86::Assembler to emit some code to .text section in CodeHolder:
		a.mov(x86::eax, 123);  // Emits 'mov eax, 1' - moves one to 'eax' register.
		a.ret();             // Emits 'ret'        - returns from a function.

		// 'x86::Assembler' is no longer needed from here and can be destroyed or explicitly
		// detached via 'code.detach(&a)' - which detaches an attached emitter from code holder.

		// Now add the generated code to JitRuntime via JitRuntime::add(). This function would
		// copy the code from CodeHolder into memory with executable permission and relocate it.
		Func fn;
		Error err = rt.add(&fn, &code);

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

		// Execute the generated function and print the resulting '1', which it moves to 'eax'.
		int result = fn(4, 9);
		printf("%d\n", result);

		rt.release(fn);
	}

	lua_getglobal(L, "require");
	lua_pushliteral(L, "foo");
	lua_pcall(L, 1, 0, 0);


	{
		JitRuntime rt;

		CodeHolder code;
		code.init(rt.environment(), rt.cpuFeatures());

		// Attach x86::Assembler to `code`.
		host::Assembler a(&code);

		// Create AsmParser that will emit to x86::Assembler.
		AsmParser p(&a);

		// Parse some assembly.
		Error err = p.parse(
			"mov rax, rcx\n"
			"add rax, rdx\n"
			"ret\n");

		code.detach(&a);

		// Error handling (use asmjit::ErrorHandler for more robust error handling).
		if (err) {
			printf("ERROR: %08x (%s)\n", err, DebugUtils::errorAsString(err));
			return 1;
		}

		Func fn;
		Error err2 = rt.add(&fn, &code);

		code.reset();

		int result = fn(28, 12);
		printf("%u\n", result);

	}






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

	HWND hwnd = CreateWindowExW(
		0, L"HRAM Window Class", L"HRAM", WS_OVERLAPPEDWINDOW,
		winbox.left,
		winbox.top,
		winbox.right - winbox.left,
		winbox.bottom - winbox.top,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) { return 0; }

	diffw = (winbox.right - winbox.left) - winw;
	diffh = (winbox.bottom - winbox.top) - winh;


	const BOOL isDarkMode = true;
	HRESULT result = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));


	checkSubWindow();
	subwin = CreateWindowExW(
		0, L"HRAM SubWindow Class", L"", WS_CHILD | WS_VISIBLE,
		subx, suby, subw, subh,
		hwnd, 0, 0, NULL);
	if (subwin == NULL) { return 0; }






	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // non-srgb for simplicity here. see other minimal gists for srgb setup
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 2;
	swapchaindesc.OutputWindow = subwin;
	swapchaindesc.Windowed = TRUE;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext);


	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer); // get the swapchain's frame buffer

	device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV); // make a render target [view] from it


	ID3D11VertexShader* vertexshader;
	device->CreateVertexShader(MyVertexShader, sizeof(MyVertexShader), 0, &vertexshader);

	ID3D11PixelShader* pixelshader;
	device->CreatePixelShader(MyPixelShader, sizeof(MyPixelShader), 0, &pixelshader);

	D3D11_RASTERIZER_DESC rasterizerdesc = { D3D11_FILL_SOLID, D3D11_CULL_NONE }; // CULL_NONE to be agnostic of triangle winding order
	ID3D11RasterizerState* rasterizerstate;
	device->CreateRasterizerState(&rasterizerdesc, &rasterizerstate);

	D3D11_VIEWPORT viewport = { 0, 0, subw, subh, 0, 1 };
	devicecontext->RSSetViewports(1, &viewport);







	D3D11_SAMPLER_DESC samplerdesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP };
	//samplerdesc.MinLOD = 0;
	//samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;
	//samplerdesc.MipLODBias = 0.f;
	//samplerdesc.MaxAnisotropy = 1;
	//samplerdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11SamplerState* samplerstate;
	device->CreateSamplerState(&samplerdesc, &samplerstate);

	memset(texturedata, 0, 320 * 180 * 4);
	//for (int i = 0; i < 320 * 180; i++) texturedata[i] = (int)(((float)rand() / (float)RAND_MAX) * 0xffffffff) | 0xff000000;
	for (int i = 0; i < 320 * 180 * 4; i++) {
		((uint8_t*)texturedata)[i] = rand() % 0xff;
	}



	D3D11_TEXTURE2D_DESC texturedesc = {};
	texturedesc.Width = 320;
	texturedesc.Height = 180;
	//texturedesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	//texturedesc.MipLevels = 0;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.Usage = D3D11_USAGE_DYNAMIC;
	texturedesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE /*| D3D11_BIND_RENDER_TARGET*/;

	D3D11_SUBRESOURCE_DATA textureSRD = {};
	textureSRD.pSysMem = texturedata;
	textureSRD.SysMemPitch = 320 * 4;

	//ID3D11Texture2D* texture;
	HRESULT code = device->CreateTexture2D(&texturedesc, &textureSRD, &texture);
	if (S_OK != code) { return 0; }

	ID3D11ShaderResourceView* textureSRV;
	device->CreateShaderResourceView(texture, nullptr, &textureSRV);




	//IDirect3DDevice9_SetSamplerState(data->device, index, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	//IDirect3DDevice9_SetSamplerState(data->device, index, D3DSAMP_MAGFILTER, D3DTEXF_POINT);


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
			devicecontext->PSSetShaderResources(0, 1, &textureSRV);
			devicecontext->PSSetSamplers(0, 1, &samplerstate);

			devicecontext->OMSetRenderTargets(1, &framebufferRTV, nullptr);

			devicecontext->Draw(4, 0);

			swapchain->Present(1, 0);
		}
	}

	return 0;
}

WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };

int first = 1;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}

	case WM_CHAR: {
		printf("char: %d\n", wParam);
		break;
	}

	case WM_KEYUP: {
		printf("key up %d\n", wParam);
		break;
	}

	case WM_SYSKEYDOWN: {
		printf("sys key down %d\n", wParam);
		return 0;
	}

	case WM_SYSKEYUP: {
		printf("sys key up %d\n", wParam);
		return 0;
	}

	case WM_KEYDOWN: {

		printf("key down %d\n", wParam);

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
		lpMMI->ptMinTrackSize.x = resw + diffw;
		lpMMI->ptMinTrackSize.y = resh + diffh;
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

		//printf("%d by %d", winw, winh);

		D3D11_VIEWPORT viewport = { 0, 0, subw, subh, 0, 1 };
		devicecontext->RSSetViewports(1, &viewport);

		devicecontext->Flush();

		framebufferRTV->Release();
		framebuffer->Release();

		swapchain->ResizeBuffers(0, subw, subh, DXGI_FORMAT_UNKNOWN, 0);

		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer); // get the swapchain's buffer
		device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV); // and make it a render target [view]


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

			printf("move %d %d\n", xPos, yPos);

			int i = yPos * 320 + xPos;
			texturedata[i] = 0xffffffff;

			D3D11_MAPPED_SUBRESOURCE subres;
			devicecontext->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
			memcpy(subres.pData, texturedata, 320 * 180 * 4);
			devicecontext->Unmap(texture, 0);

			return 0;
		}

	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
