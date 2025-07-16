#pragma once

#include "framework.h"

struct Screen
{

	int w;
	int h;
	ID3D11ShaderResourceView* texturesrv = nullptr;
	ID3D11Texture2D* texture = nullptr;

	Screen(int w, int h) : w(w), h(h) {}

	void setup(ID3D11Device* device);

};
