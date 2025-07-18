#include "image.h"

#include "util.h"
#include "window.h"

ID3D11Texture2D* createImage(ID3D11Device* device, void* data, int w, int h, int pw) {
	if (pw == 0) pw = w * 4;

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
	textureSRD.SysMemPitch = pw;

	HR(device->CreateTexture2D(&texturedesc, &textureSRD, &texture));

	return texture;
}

static int newimage(lua_State* L) {
	auto mem = reinterpret_cast<void*>(lua_tointeger(L, 1));
	auto w = luaL_checkinteger(L, 2);
	auto h = luaL_checkinteger(L, 3);
	auto pw = lua_tointeger(L, 4);

	auto img = createImage(device, mem, w, h, pw);
	lua_pushinteger(L, reinterpret_cast<uint64_t>(img));

	return 1;
}

static int delimage(lua_State* L) {
	auto img = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 1));
	img->Release();
	return 0;
}

static int putimage(lua_State* L) {
	auto dst = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 1));
	auto src = reinterpret_cast<ID3D11Texture2D*>(lua_tointeger(L, 2));
	if (dst == nullptr) dst = screen->texture;

	auto dx = lua_tointeger(L, 3);
	auto dy = lua_tointeger(L, 4);

	devicecontext->CopySubresourceRegion(dst, 0, dx, dy, 0, src, 0, NULL);

	return 0;
}

static const struct luaL_Reg imagelib[] = {
	{"create", newimage},
	{"delete", delimage},
	{"copy", putimage},
	{NULL, NULL}
};

int luaopen_image(lua_State* L) {
	luaL_newlib(L, imagelib);
	return 1;
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
