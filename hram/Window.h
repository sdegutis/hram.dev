#pragma once

#include "framework.h"

#include "Screen.h"

class Window {

	WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };

	HWND hwnd = nullptr;
	HWND subwin = nullptr;

	int padw = 0;
	int padh = 0;

	int subx = 0;
	int suby = 0;
	int subw = 0;
	int subh = 0;

	IDXGISwapChain* swapchain = nullptr;
	ID3D11Texture2D* framebuffer = nullptr;
	ID3D11RenderTargetView* framebufferRTV = nullptr;
	ID3D11RasterizerState* rasterizerstate = nullptr;
	ID3D11SamplerState* samplerstate = nullptr;

	ID3D11VertexShader* vertexshader = nullptr;
	ID3D11PixelShader* pixelshader = nullptr;

	void HR(HRESULT res);

	int first = 1;

	void resetBuffers();
	void moveSubWindow();

public:

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* devicecontext = nullptr;

	Screen screen1{ 320, 180 };
	Screen screen2{ 320, 200 };
	Screen* screen = &screen1;

	int scale = 3;
	int w = screen->w * scale;
	int h = screen->h * scale;

	RECT getInitialRect();
	void setup(HINSTANCE hInstance, int nCmdShow);
	void draw();

	void getMinSize(LONG* w, LONG* h);
	void resized(int w, int h);

	void toggleFullscreen();
	void useScreen(Screen* s);

};

extern Window* win;
