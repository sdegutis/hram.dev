#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
//#include <d3d11.h>
#include <shlwapi.h>
#include <shlobj_core.h>
#include <stdint.h>
#include <atlstr.h>

#include <string>

#include "PixelShader.h"
#include "VertexShader.h"
#include <lua/lua.hpp>

#pragma comment(lib, "shlwapi")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "user32")
//#pragma comment(lib, "d3d11")


HCURSOR hCursor;
HWND subwin;




class Image
{

public:

	int resw;
	int resh;

	Image(int resw, int resh)
		: resw(resw)
		, resh(resh)
	{
	}

	~Image() {
	}

};




Image screen1{ 320,180 };
Image screen2{ 320 * 2,180 * 2 };
Image* screen = &screen1;




int scale = 3;
int winw = screen->resw * scale;
int winh = screen->resh * scale;



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

#include <thread>

#include "util.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {

	openConsole();


	checkSubWindow();


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

		}
	}

	return 0;
}

WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };

int first = 1;

void resetBuffers() {
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

			//s.copyTo(*screen, mousex, mousey);

			return 0;
		}

	}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
