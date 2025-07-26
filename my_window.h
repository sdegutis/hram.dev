#pragma once

#include <Windows.h>
#include <d3d11.h>

int setupWindow(HINSTANCE hInstance, int nCmdShow);
void toggleFullscreen();
void runLoop();
void draw();

extern ID3D11Device* device;
extern ID3D11DeviceContext* devicecontext;
extern struct Screen screen;
