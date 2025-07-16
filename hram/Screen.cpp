#include "Screen.h"

#include "Window.h"
#include "image.h"

void Screen::setup() {
	auto data = (uint32_t*)malloc(w * h * 4);
	if (data == NULL) throw std::exception("out of memory");
	memset(data, 0x00, w * h * 4);
	texture = createImage(data, w, h);
	free(data);

	HR(win->device->CreateShaderResourceView(texture, nullptr, &texturesrv));
}
