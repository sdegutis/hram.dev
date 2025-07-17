#include "image.h"

#include "util.h"

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, int w, int h) {
	ID3D11Texture2D* texture = nullptr;

	D3D11_TEXTURE2D_DESC texturedesc = {};
	texturedesc.Width = w;
	texturedesc.Height = h;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSRD = {};
	textureSRD.pSysMem = data;
	textureSRD.SysMemPitch = w * 4;
	textureSRD.SysMemSlicePitch = w * 4;

	HR(device->CreateTexture2D(&texturedesc, &textureSRD, &texture));

	return texture;
}
