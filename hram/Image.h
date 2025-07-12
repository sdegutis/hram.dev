#pragma once

#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <d3d11.h>
#include <exception>

class Image
{

	uint32_t* texturedata;
	ID3D11DeviceContext* devicecontext;

public:

	int resw;
	int resh;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureSRV;

	Image(int resw, int resh)
		: resw(resw)
		, resh(resh)
	{
	}

	~Image() {
		free(texturedata);
		texture->Release();
		textureSRV->Release();
	}

	void create(ID3D11Device* device, ID3D11DeviceContext* devicecontext);

	void pset(int x, int y, uint32_t c);

};
