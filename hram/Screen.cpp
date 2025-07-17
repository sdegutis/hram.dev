#include "Screen.h"

#include <stdint.h>
#include <exception>

#include "image.h"
#include "util.h"

void Screen::setup(ID3D11Device* device) {
	auto data = new uint32_t[w * h * 4];
	if (data == NULL) throw std::exception("out of memory");
	memset(data, 0x00, w * h * 4);
	texture = createImage(device, data, w, h);
	delete[] data;

	HR(device->CreateShaderResourceView(texture, nullptr, &texturesrv));
}
