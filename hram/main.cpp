#include <Windows.h>

#include "window.h"
#include "app.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
	setupWindow(hInstance, nCmdShow);
	app::boot();
	runLoop();
	return 0;
}

//	void pset(int x, int y, uint32_t c)
//	{
//		D3D11_BOX box;
//		box.top = y;
//		box.bottom = y + 1;
//		box.left = x;
//		box.right = x + 1;
//		box.front = 0;
//		box.back = 1;
//		devicecontext->UpdateSubresource(texture, 0, &box, &c, 4, 4);
//	}
//
//	void fillrect(int x, int y, int w, int h, int c)
//	{
//		auto buf = (uint32_t*)malloc(w * h * 4);
//		if (buf == 0) throw std::exception("oom");
//		for (int i = 0; i < w * h; i++) buf[i] = c;
//
//		D3D11_BOX box;
//		box.top = y;
//		box.bottom = y + h;
//		box.left = x;
//		box.right = x + w;
//		box.front = 0;
//		box.back = 1;
//		devicecontext->UpdateSubresource(texture, 0, &box, buf, w * 4, w * h * 4);
//
//		free(buf);
//	}
//
//
//	void copyTo(Image& dst, int dx, int dy)
//	{
//		devicecontext->CopySubresourceRegion(dst.texture, 0, dx, dy, 0, texture, 0, NULL);
//	}
//
//	void copyTo(Image& dst, int dx, int dy, int sx, int sy, int sw, int sh)
//	{
//		D3D11_BOX box;
//		box.left = sx;
//		box.right = sx + sw;
//		box.top = sy;
//		box.bottom = sy + sh;
//		box.front = 0;
//		box.back = 1;
//		devicecontext->CopySubresourceRegion(dst.texture, 0, dx, dy, 0, texture, 0, &box);
//	}
//
//
//};
//
//	s.create(device, devicecontext);
//
//	s.fillrect(0, 0, 4, 4, 0xff9900);
//	s.fillrect(1, 1, 2, 2, 0xff99ff);
//
//	s.copyTo(screen1, 10, 10);
//	s.copyTo(screen1, 0, 0, 1, 1, 2, 2);
//
