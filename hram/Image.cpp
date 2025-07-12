#include "Image.h"

void Image::create(ID3D11Device* device, ID3D11DeviceContext* devicecontext)
{
	this->devicecontext = devicecontext;
	texturedata = (uint32_t*)malloc(resw * resh * 4);
	if (texturedata == NULL) throw std::exception("out of memory");

	memset(texturedata, 0, resw * resh * 4);

	D3D11_TEXTURE2D_DESC texturedesc = {};
	texturedesc.Width = resw;
	texturedesc.Height = resh;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSRD = {};
	textureSRD.pSysMem = texturedata;
	textureSRD.SysMemPitch = resw * 4;
	textureSRD.SysMemSlicePitch = resw * 4;

	HRESULT code = device->CreateTexture2D(&texturedesc, &textureSRD, &texture);
	if (S_OK != code) { throw std::exception(); }

	code = device->CreateShaderResourceView(texture, nullptr, &textureSRV);
	if (S_OK != code) { throw std::exception(); }
}

void Image::pset(int x, int y, uint32_t c) {
	D3D11_BOX box;
	box.top = y;
	box.bottom = y + 1;
	box.left = x;
	box.right = x + 1;
	box.front = 0;
	box.back = 1;
	devicecontext->UpdateSubresource(texture, 0, &box, &c, 4, 4);
}
