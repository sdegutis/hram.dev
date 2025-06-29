#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_gpu.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

int padding = 30;
SDL_Window* window;
SDL_GLContext glcontext;

lua_State* L;

//SDL_Rect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
//SDL_Rect destrect;
//int scale = 1;
//
//static void draw()
//{
//	glClear(GL_COLOR_BUFFER_BIT);
//	glBindVertexArray(vao);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//	SDL_GL_SwapWindow(window);
//}
//
//static void resized()
//{
//	destrect = srcrect;
//	scale = 1;
//
//	int w, h;
//	SDL_GetWindowSize(window, &w, &h);
//
//	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
//		scale++;
//		destrect.w += srcrect.w;
//		destrect.h += srcrect.h;
//	}
//
//	destrect.x = w / 2 - destrect.w / 2;
//	destrect.y = h / 2 - destrect.h / 2;
//
//	glBindVertexArray(vao);
//	glViewport(0, 0, w, h);
//	glUniform2f(resolutionLocation, w, h);
//
//
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, posBuf);
//
//	float x1 = destrect.x;
//	float x2 = destrect.x + destrect.w;
//	float y1 = destrect.y;
//	float y2 = destrect.y + destrect.h;
//	float xys[12] = { x1,y1,x2,y1,x1,y2,x1,y2,x2,y1,x2,y2 };
//	glBufferData(GL_ARRAY_BUFFER, sizeof(xys), xys, GL_STATIC_DRAW);
//
//	draw();
//}
//
//static int drawimg(lua_State* L) {
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
//
//	uint8_t* data = lua_touserdata(L, 1);
//	int sx = lua_tonumber(L, 2);
//	int sy = lua_tonumber(L, 3);
//	int sw = lua_tonumber(L, 4);
//	int sh = lua_tonumber(L, 5);
//	int dx = lua_tonumber(L, 6);
//	int dy = lua_tonumber(L, 7);
//
//	int i = sy * 320 + sx;
//	glTexSubImage2D(GL_TEXTURE_2D, 0, dx, dy, sw, sh, GL_BGRA, GL_UNSIGNED_BYTE, (uint32_t*)data + i);
//	return 0;
//}
//
//static int blit(lua_State* L) {
//	draw();
//	return 0;
//}
//
//static int opendir(lua_State* L) {
//	char* s = lua_tostring(L, -1);
//	bool res = SDL_OpenURL(s);
//	lua_pushboolean(L, res);
//	return 1;
//}
//
//static int setfullscreen(lua_State* L) {
//	bool b = lua_toboolean(L, -1);
//	bool res = SDL_SetWindowFullscreen(window, b);
//	if (res) resized();
//	lua_pushboolean(L, res);
//	return 1;
//}
//
//static int memorycopy(lua_State* L) {
//	uint32_t* dst = lua_touserdata(L, 1);
//	uint64_t dstoff = lua_tointeger(L, 2);
//	uint32_t* src = lua_touserdata(L, 3);
//	uint64_t srcoff = lua_tointeger(L, 4);
//	uint64_t siz = lua_tointeger(L, 5);
//	if (dstoff < 0 || dstoff >= 320 * 180 ||
//		srcoff < 0 || srcoff >= 320 * 180 ||
//		dstoff + siz > 320 * 180 ||
//		srcoff + siz > 320 * 180)
//	{
//		lua_warning(L, "memcpy outside bounds", false);
//		lua_pushboolean(L, false);
//		return 1;
//	}
//	memcpy(dst + dstoff, src + srcoff, siz * 4);
//	lua_pushboolean(L, true);
//	return 1;
//}
//
//static int newmem(lua_State* L) {
//	uint64_t size = lua_tointeger(L, 1);
//	lua_newuserdata(L, size * 4);
//	return 1;
//}
//
//static int memoryset(lua_State* L) {
//	uint32_t* data = lua_touserdata(L, 1);
//	uint64_t off = lua_tointeger(L, 2);
//	uint64_t val = lua_tointeger(L, 3);
//	uint64_t siz = lua_tointeger(L, 4);
//	if (off < 0 || off >= 320 * 180 ||
//		off + siz > 320 * 180)
//	{
//		lua_warning(L, "memset outside bounds", false);
//		lua_pushboolean(L, false);
//		return 1;
//	}
//
//	data += off;
//	for (uint32_t* done = data + siz; data != done; data++)
//		*data = val;
//
//	lua_pushboolean(L, true);
//	return 1;
//}

SDL_Renderer* renderer;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushstring(L, SDL_GetPrefPath("", "progma0xb4"));
	lua_setglobal(L, "userdir");

	luaL_dostring(L, "package.path = userdir .. '?.lua;' .. package.path");

	//lua_register(L, "blit", blit);
	//lua_register(L, "drawimg", drawimg);
	//lua_register(L, "opendir", opendir);
	//lua_register(L, "setfullscreen", setfullscreen);
	//lua_register(L, "memcpy", memorycopy);
	//lua_register(L, "memset", memoryset);
	//lua_register(L, "newmem", newmem);



	SDL_SetAppMetadata("PROPIMA 0xB4", "0.1", "com.90sdev.propima0xb4");
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	SDL_HideCursor();

	window = SDL_CreateWindow("PROPIMA 0xB4", 320 * 3 + (padding * 2), 180 * 3 + (padding * 2), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);





	renderer = SDL_CreateRenderer(window, NULL);



	SDL_SetRenderScale(renderer, 3, 3);


	SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 10, 10);
	SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);

	printf("tex = %p\n", tex);

	SDL_SetRenderTarget(renderer, tex);

	SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);

	SDL_FRect r2 = { .x = 0, .y = 0, .w = 10, .h = 10 };
	SDL_RenderRect(renderer, &r2);

	SDL_SetRenderTarget(renderer, NULL);


	SDL_FRect r3 = { .x = 0, .y = 0, .w = 10, .h = 10 };
	SDL_RenderTexture(renderer, tex, NULL, &r3);


	SDL_RenderPresent(renderer);





	return SDL_APP_CONTINUE;
}

static uint64_t last = 0;

SDL_AppResult SDL_AppIterate(void* appstate)
{

	//SDL_SetRenderDrawColor(renderer, SDL_rand(256), 0, 0, 255);

	//SDL_FRect rect = { .x = SDL_rand(100), .y = SDL_rand(100), .w = SDL_rand(220), .h = SDL_rand(80) };
	//SDL_RenderFillRect(renderer, &rect);

	//SDL_RenderPresent(renderer);


	return SDL_APP_CONTINUE;






	uint64_t now = SDL_GetTicks();
	uint64_t diff = now - last;

	if (diff >= 33) {
		last = now;
		lua_getglobal(L, "tick");
		if (!lua_isnil(L, -1)) {
			lua_pushinteger(L, diff);
			lua_pcall(L, 1, 0, 0);
		}
		lua_settop(L, 0);
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	return SDL_APP_CONTINUE;




	switch (e->type) {

		//case SDL_EVENT_WINDOW_RESIZED:
		//	resized();
		//	break;

		//case SDL_EVENT_MOUSE_MOTION:
		//	lua_getglobal(L, "mousemove");
		//	lua_pushinteger(L, floor((float)(e->motion.x - destrect.x) / (float)scale));
		//	lua_pushinteger(L, floor((float)(e->motion.y - destrect.y) / (float)scale));
		//	lua_pcall(L, 2, 0, 0);
		//	lua_settop(L, 0);
		//	break;

		//case SDL_EVENT_KEY_DOWN:
		//{
		//	lua_getglobal(L, "keydown");
		//	lua_pushinteger(L, e->key.scancode);
		//	int key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		//	if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		//	lua_pcall(L, 2, 0, 0);
		//	lua_settop(L, 0);
		//	break;
		//}

		//case SDL_EVENT_KEY_UP: {
		//	lua_getglobal(L, "keyup");
		//	lua_pushinteger(L, e->key.scancode);
		//	int key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		//	if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		//	lua_pcall(L, 2, 0, 0);
		//	lua_settop(L, 0);
		//	break;
		//}

		//case SDL_EVENT_MOUSE_BUTTON_DOWN:
		//	lua_getglobal(L, "mousedown");
		//	lua_pushinteger(L, e->button.button);
		//	lua_pcall(L, 1, 0, 0);
		//	lua_settop(L, 0);
		//	break;

		//case SDL_EVENT_MOUSE_BUTTON_UP:
		//	lua_getglobal(L, "mouseup");
		//	lua_pushinteger(L, e->button.button);
		//	lua_pcall(L, 1, 0, 0);
		//	lua_settop(L, 0);
		//	break;

		//case SDL_EVENT_MOUSE_WHEEL:
		//	lua_getglobal(L, "mousewheel");
		//	lua_pushnumber(L, e->wheel.x);
		//	lua_pushnumber(L, e->wheel.y);
		//	lua_pcall(L, 2, 0, 0);
		//	lua_settop(L, 0);
		//	break;

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	//glDeleteProgram(prog);
	//SDL_GL_DestroyContext(glcontext);
	SDL_DestroyWindow(window);
}
