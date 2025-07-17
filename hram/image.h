#pragma once

#include <stdint.h>
#include <d3d11.h>

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, int w, int h);
