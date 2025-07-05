#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

SDL_Window* window;
SDL_FRect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_FRect destrect;
int scale = 1;

lua_State* L;

static void resized()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	destrect = srcrect;
	scale = 1;

	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
		scale++;
		destrect.w += srcrect.w;
		destrect.h += srcrect.h;
	}

	destrect.x = w / 2 - destrect.w / 2;
	destrect.y = h / 2 - destrect.h / 2;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	L = luaL_newstate();
	luaL_openlibs(L);

	//lua_pushstring(L, SDL_GetPrefPath("", "os91"));
	//lua_setglobal(L, "userdir");

	//luaL_dostring(L, "package.path = userdir .. '?.lua;' .. package.path");

	//lua_register(L, "blit", blit);
	//lua_register(L, "opendir", opendir);
	//lua_register(L, "setfullscreen", setfullscreen);
	//lua_register(L, "newtexture", newtexture);
	//lua_register(L, "deltexture", deltexture);
	//lua_register(L, "settexture", settexture);
	//lua_register(L, "drawtexture", drawtexture);
	//lua_register(L, "updatetexture", updatetexture);
	//lua_register(L, "rectfill", rectfill);
	//lua_register(L, "clearout", clearout);
	//lua_register(L, "setcolor", setcolor);

	//lua_newtable(L);
	//lua_pushinteger(L, SDL_TEXTUREACCESS_TARGET);    lua_setfield(L, -2, "target");
	//lua_pushinteger(L, SDL_TEXTUREACCESS_STATIC);    lua_setfield(L, -2, "static");
	//lua_pushinteger(L, SDL_TEXTUREACCESS_STREAMING); lua_setfield(L, -2, "streaming");
	//lua_setglobal(L, "texturetype");

	SDL_SetAppMetadata("os91", "0.1", "com.90sdev.os91");
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_HideCursor();
	window = SDL_CreateWindow("os91", 320 * 3 + (30 * 2), 180 * 3 + (30 * 2), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	//renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	//screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
	//SDL_SetTextureScaleMode(screen, SDL_SCALEMODE_NEAREST);

	//SDL_SetRenderTarget(renderer, screen);
	//SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
	//SDL_FRect r = { .x = 0, .y = 0, .w = 320, .h = 180 };
	//SDL_RenderFillRect(renderer, &r);

	resized();

	//lua_getglobal(L, "require");
	//lua_pushstring(L, "boot");
	//lua_call(L, 1, 0);

	return SDL_APP_CONTINUE;
}

static uint64_t last = 0;

SDL_AppResult SDL_AppIterate(void* appstate)
{
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

	SDL_Delay(10);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {

	case SDL_EVENT_MOUSE_MOTION:
		lua_getglobal(L, "mousemove");
		lua_pushinteger(L, floor((float)(e->motion.x - destrect.x) / (float)scale));
		lua_pushinteger(L, floor((float)(e->motion.y - destrect.y) / (float)scale));
		lua_pcall(L, 2, 0, 0);
		lua_settop(L, 0);
		break;

	case SDL_EVENT_KEY_DOWN:
	{
		lua_getglobal(L, "keydown");
		lua_pushinteger(L, e->key.scancode);
		const char key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		lua_pcall(L, 2, 0, 0);
		lua_settop(L, 0);
		break;
	}

	case SDL_EVENT_KEY_UP: {
		lua_getglobal(L, "keyup");
		lua_pushinteger(L, e->key.scancode);
		const char key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		lua_pcall(L, 2, 0, 0);
		lua_settop(L, 0);
		break;
	}

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		lua_getglobal(L, "mousedown");
		lua_pushinteger(L, e->button.button);
		lua_pcall(L, 1, 0, 0);
		lua_settop(L, 0);
		break;

	case SDL_EVENT_MOUSE_BUTTON_UP:
		lua_getglobal(L, "mouseup");
		lua_pushinteger(L, e->button.button);
		lua_pcall(L, 1, 0, 0);
		lua_settop(L, 0);
		break;

	case SDL_EVENT_MOUSE_WHEEL:
		lua_getglobal(L, "mousewheel");
		lua_pushnumber(L, e->wheel.x);
		lua_pushnumber(L, e->wheel.y);
		lua_pcall(L, 2, 0, 0);
		lua_settop(L, 0);
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		resized();
		break;

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	lua_close(L);
	SDL_DestroyWindow(window);
}
