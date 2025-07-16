#include "Window.h"

#include "PixelShader.h"
#include "VertexShader.h"

Window* win;

#include "image.h"
ID3D11Texture2D* img;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {

	win = new Window();
	win->setup(hInstance, nCmdShow);

	auto data = (uint8_t*)malloc(4 * 4 * 4);
	for (int i = 0; i < 4 * 4 * 4; i++) data[i] = rand() % 0xff;
	img = createImage((uint32_t*)data, 4, 4);
	free(data);

	win->devicecontext->CopySubresourceRegion(win->screen->texture, 0, 6, 10, 0, img, 0, NULL);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			win->draw();
		}
	}

	return 0;

}

inline void Window::setup(HINSTANCE hInstance, int nCmdShow) {
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"HRAM Window Class";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(0x11, 0x11, 0x11));
	RegisterClass(&wc);

	WNDCLASS wc2 = {};
	wc2.lpfnWndProc = WindowProc2;
	wc2.hInstance = hInstance;
	wc2.lpszClassName = L"HRAM SubWindow Class";
	RegisterClass(&wc2);

	RECT winbox = getInitialRect();
	hwnd = CreateWindowExW(
		0, L"HRAM Window Class", L"HRAM", WS_OVERLAPPEDWINDOW,
		winbox.left,
		winbox.top,
		winbox.right - winbox.left,
		winbox.bottom - winbox.top,
		NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) { throw std::exception("can't create window"); }

	const BOOL isDarkMode = true;
	HRESULT result = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &isDarkMode, sizeof(isDarkMode));

	moveSubWindow();
	subwin = CreateWindowExW(
		0, L"HRAM SubWindow Class", L"", WS_CHILD | WS_VISIBLE,
		subx, suby, subw, subh,
		hwnd, NULL, hInstance, NULL);
	if (subwin == NULL) { throw std::exception("can't create canvas view"); }

	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchaindesc.BufferCount = 2;
	swapchaindesc.OutputWindow = subwin;
	swapchaindesc.Windowed = TRUE;
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	HR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext));

	HR(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer));
	HR(device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV));

	HR(device->CreateVertexShader(MyVertexShader, sizeof(MyVertexShader), 0, &vertexshader));
	HR(device->CreatePixelShader(MyPixelShader, sizeof(MyPixelShader), 0, &pixelshader));

	D3D11_RASTERIZER_DESC rasterizerdesc = { D3D11_FILL_SOLID, D3D11_CULL_BACK };
	HR(device->CreateRasterizerState(&rasterizerdesc, &rasterizerstate));

	D3D11_SAMPLER_DESC samplerdesc = { D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP };
	HR(device->CreateSamplerState(&samplerdesc, &samplerstate));

	D3D11_VIEWPORT viewport = { 0, 0, (float)subw, (float)subh, 0, 1 };
	devicecontext->RSSetViewports(1, &viewport);

	screen1.setup();
	screen2.setup();

	ShowWindow(hwnd, nCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
}

inline RECT Window::getInitialRect() {
	RECT winbox;
	winbox.left = GetSystemMetrics(SM_CXSCREEN) / 2 - w / 2;
	winbox.top = GetSystemMetrics(SM_CYSCREEN) / 2 - h / 2;
	winbox.right = winbox.left + w;
	winbox.bottom = winbox.top + h;
	AdjustWindowRectEx(&winbox, WS_OVERLAPPEDWINDOW, false, 0);

	padw = (winbox.right - winbox.left) - w;
	padh = (winbox.bottom - winbox.top) - h;

	return winbox;
}

inline void Window::moveSubWindow() {
	subw = screen->w;
	subh = screen->h;
	scale = 1;

	while (
		subw + screen->w <= w &&
		subh + screen->h <= h)
	{
		subw += screen->w;
		subh += screen->h;
		scale++;
	}

	subx = w / 2 - subw / 2;
	suby = h / 2 - subh / 2;
}

inline void Window::draw() {
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	devicecontext->VSSetShader(vertexshader, nullptr, 0);

	devicecontext->RSSetState(rasterizerstate);

	devicecontext->PSSetShader(pixelshader, nullptr, 0);
	devicecontext->PSSetShaderResources(0, 1, &screen->texturesrv);
	devicecontext->PSSetSamplers(0, 1, &samplerstate);

	devicecontext->OMSetRenderTargets(1, &framebufferRTV, nullptr);

	devicecontext->Draw(4, 0);

	swapchain->Present(1, 0);
}

inline void Window::getMinSize(LONG* w, LONG* h) {
	*w = screen->w + padw;
	*h = screen->h + padh;
}

inline void Window::resized(int w, int h) {
	this->w = w;
	this->h = h;
	moveSubWindow();
	SetWindowPos(subwin, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	if (first) {
		first = 0;
	}
	else {
		resetBuffers();
	}
}

inline void Window::toggleFullscreen() {
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (dwStyle & WS_OVERLAPPEDWINDOW) {
		MONITORINFO mi = { sizeof(mi) };
		if (GetWindowPlacement(hwnd, &lastwinpos) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
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
		SetWindowPlacement(hwnd, &lastwinpos);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

inline void Window::useScreen(Screen* s) {
	screen = s;
	moveSubWindow();
	SetWindowPos(subwin, NULL, subx, suby, subw, subh, SWP_FRAMECHANGED);
	resetBuffers();
}

inline void Window::resetBuffers() {
	D3D11_VIEWPORT viewport = { 0, 0, (float)subw, (float)subh, 0, 1 };
	devicecontext->RSSetViewports(1, &viewport);

	devicecontext->Flush();

	framebufferRTV->Release();
	framebuffer->Release();

	HR(swapchain->ResizeBuffers(0, subw, subh, DXGI_FORMAT_UNKNOWN, 0));


	HR(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer));
	HR(device->CreateRenderTargetView(framebuffer, nullptr, &framebufferRTV));
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
			win->toggleFullscreen();
		}
		return 0;
	}

	case WM_GETMINMAXINFO: {
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		win->getMinSize(&lpMMI->ptMinTrackSize.x, &lpMMI->ptMinTrackSize.y);
		return 0;
	}

	case WM_SIZE:
		win->resized(LOWORD(lParam), HIWORD(lParam));
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

		if (win->screen == &win->screen1)
			win->useScreen(&win->screen2);
		else
			win->useScreen(&win->screen1);

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

		auto xPos = GET_X_LPARAM(lParam) / win->scale;
		auto yPos = GET_Y_LPARAM(lParam) / win->scale;

		if (xPos != mousex || yPos != mousey) {
			mousex = xPos;
			mousey = yPos;

			printf("move %d %d\n", mousex, mousey);
			//screen->pset(mousex, mousey, RGB(rand() % 0xff, rand() % 0xff, rand() % 0xff));

			win->devicecontext->CopySubresourceRegion(win->screen->texture, 0, mousex, mousey, 0, img, 0, NULL);

			//s.copyTo(*screen, mousex, mousey);

			return 0;
		}

	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
