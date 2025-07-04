#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>


//#include <




SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* screen;

SDL_FRect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_FRect destrect;
int scale = 1;

// lua_State* L;

static void blit() {
	SDL_SetRenderTarget(renderer, NULL);

	SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 0xff);
	SDL_RenderClear(renderer);

	SDL_RenderTexture(renderer, screen, &srcrect, &destrect);
	SDL_RenderPresent(renderer);
}

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

	blit();
}

// static int opendir(lua_State* L) {
// 	const char* s = lua_tostring(L, -1);
// 	bool res = SDL_OpenURL(s);
// 	lua_pushboolean(L, res);
// 	return 1;
// }

// static int setfullscreen(lua_State* L) {
// 	bool b = lua_toboolean(L, -1);
// 	bool res = SDL_SetWindowFullscreen(window, b);
// 	if (res) resized();
// 	lua_pushboolean(L, res);
// 	return 1;
// }

// static int newtexture(lua_State* L) {
// 	SDL_TextureAccess type = lua_tointeger(L, 1);
// 	int w = lua_tointeger(L, 2);
// 	int h = lua_tointeger(L, 3);
// 	SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, type, w, h);
// 	SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
// 	lua_pushlightuserdata(L, tex);
// 	return 1;
// }

// static int settexture(lua_State* L) {
// 	SDL_Texture* tex = lua_touserdata(L, 1);
// 	if (tex == NULL) tex = screen;
// 	bool worked = SDL_SetRenderTarget(renderer, tex);
// 	lua_pushboolean(L, worked);
// 	return 1;
// }

// static int drawtexture(lua_State* L) {
// 	SDL_Texture* tex = lua_touserdata(L, 1);

// 	SDL_FRect dstrect;
// 	dstrect.x = lua_tointeger(L, 2);
// 	dstrect.y = lua_tointeger(L, 3);
// 	dstrect.w = lua_tointeger(L, 4);
// 	dstrect.h = lua_tointeger(L, 5);

// 	SDL_FRect srcrect;
// 	srcrect.x = lua_tointeger(L, 6);
// 	srcrect.y = lua_tointeger(L, 7);
// 	srcrect.w = lua_tointeger(L, 8);
// 	srcrect.h = lua_tointeger(L, 9);

// 	bool worked = SDL_RenderTexture(renderer, tex, &srcrect, &dstrect);
// 	lua_pushboolean(L, worked);
// 	return 1;
// }

// static int updatetexture(lua_State* L) {
// 	SDL_Texture* tex = lua_touserdata(L, 1);

// 	lua_len(L, 2);
// 	size_t len = lua_tointeger(L, -1);
// 	lua_pop(L, 1);

// 	SDL_Rect r;
// 	r.x = lua_tointeger(L, 3);
// 	r.y = lua_tointeger(L, 4);
// 	r.w = lua_tointeger(L, 5);
// 	r.h = lua_tointeger(L, 6);

// 	uint32_t* pixels = malloc(len * 4);
// 	if (!pixels) {
// 		lua_pushboolean(L, false);
// 		return 1;
// 	}

// 	uint32_t* iter = pixels;
// 	lua_pushnil(L);
// 	while (lua_next(L, 2) != 0) {
// 		*iter++ = lua_tonumber(L, -1);
// 		lua_pop(L, 1);
// 	}
// 	bool worked = SDL_UpdateTexture(tex, &r, pixels, r.w * 4);
// 	free(pixels);

// 	lua_pushboolean(L, worked);
// 	return 1;
// }

// static int deltexture(lua_State* L) {
// 	SDL_Texture* tex = lua_touserdata(L, 1);
// 	SDL_DestroyTexture(tex);
// 	return 0;
// }

// static int setcolor(lua_State* L) {
// 	uint64_t r;
// 	uint64_t g;
// 	uint64_t b;
// 	uint64_t a;

// 	if (lua_gettop(L) == 1) {
// 		uint64_t c = lua_tointeger(L, 1);
// 		r = (c >> 24) & 0xff;
// 		g = (c >> 16) & 0xff;
// 		b = (c >> 8) & 0xff;
// 		a = (c) & 0xff;
// 	}
// 	else {
// 		r = lua_tointeger(L, 1);
// 		g = lua_tointeger(L, 2);
// 		b = lua_tointeger(L, 3);
// 		a = lua_tointeger(L, 4);
// 	}

// 	SDL_SetRenderDrawColor(renderer, r, g, b, a);
// 	return 0;
// }

// static int rectfill(lua_State* L) {
// 	SDL_FRect r;
// 	r.x = lua_tonumber(L, 1);
// 	r.y = lua_tonumber(L, 2);
// 	r.w = lua_tonumber(L, 3);
// 	r.h = lua_tonumber(L, 4);
// 	SDL_RenderFillRect(renderer, &r);
// 	return 0;
// }

// static int clearout(lua_State* L) {
// 	SDL_RenderClear(renderer);
// 	return 0;
// }



#include <print>
// #include <wasm.h>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{






	//wasm_engine_t* engine = wasm_engine_new();
	//printf("%p\n", engine);

	//const char* src =
	//	"(module\n"
	//	"  (import \"host\" \"video\" (memory 1 1))\n"
	//	"  (func (export \"foo\") (param $a i32) (result i32)\n"
	//	"    local.get $a\n"
	//	"    i32.const 1\n"
	//	"    i32.add\n"

	//	"    i32.const 3\n"
	//	"    i32.load\n"
	//	"    i32.add\n"
	//	"  )\n"
	//	")\n";

	//wasm_byte_vec_t wasm;
	//wasmtime_error_t* err = wasmtime_wat2wasm(src, strlen(src), &wasm);
	//if (err) {
	//	wasm_name_t error;
	//	wasmtime_error_message(err, &error);
	//	wasmtime_error_delete(err);
	//	printf("error: %.*s\n", error.size, error.data);
	//	return SDL_APP_FAILURE;
	//}


	//wasmtime_store_t* store = wasmtime_store_new(engine, NULL, NULL);

	//wasmtime_module_t* mod;
	//err = wasmtime_module_new(engine, (uint8_t*)wasm.data, wasm.size, &mod);
	//wasm_byte_vec_delete(&wasm);
	//if (err) {
	//	wasm_name_t error;
	//	wasmtime_error_message(err, &error);
	//	wasmtime_error_delete(err);
	//	printf("error: %.*s\n", error.size, error.data);
	//	return SDL_APP_FAILURE;
	//}

	//wasmtime_context_t* context = wasmtime_store_context(store);

	//wasm_memorytype_t* memtype = wasmtime_memorytype_new(1, true, 1, false, false);

	//wasmtime_memory_t mem;
	//err = wasmtime_memory_new(context, memtype, &mem);


	//wasm_trap_t* trap;

	//wasmtime_instance_t inst;
	//wasmtime_extern_t imports[1];
	//imports[0].kind = WASMTIME_EXTERN_MEMORY;
	//imports[0].of.memory = mem;
	//err = wasmtime_instance_new(context, mod, imports, 1, &inst, &trap);
	//if (err) {
	//	wasm_name_t error;
	//	wasmtime_error_message(err, &error);
	//	wasmtime_error_delete(err);
	//	printf("error: %.*s\n", error.size, error.data);
	//	return SDL_APP_FAILURE;
	//}

	//uint8_t* data = wasmtime_memory_data(context, &mem);
	//printf("data = %p\n", data);

	//data[3] = 5;

	//const char* name = "foo";
	//wasmtime_extern_t f;
	//bool worked = wasmtime_instance_export_get(context, &inst, name, strlen(name), &f);

	//wasmtime_func_t func = f.of.func;

	//printf("%d\n", worked);

	//wasmtime_val_t args[1];
	//args[0].kind = WASMTIME_I32;
	//args[0].of.i32 = 456;
	//wasmtime_val_t res[1];
	//err = wasmtime_func_call(context, &func, args, 1, res, 1, &trap);
	//if (err) {
	//	wasm_name_t error;
	//	wasmtime_error_message(err, &error);
	//	wasmtime_error_delete(err);
	//	printf("error: %.*s\n", error.size, error.data);
	//	return SDL_APP_FAILURE;
	//}

	//printf("res = %d\n", res[0].of.i32);





	// L = luaL_newstate();
	// luaL_openlibs(L);

	// lua_pushstring(L, SDL_GetPrefPath("", "progma0xb4"));
	// lua_setglobal(L, "userdir");

	// luaL_dostring(L, "package.path = userdir .. '?.lua;' .. package.path");

	// lua_register(L, "blit", blit);
	// lua_register(L, "opendir", opendir);
	// lua_register(L, "setfullscreen", setfullscreen);
	// lua_register(L, "newtexture", newtexture);
	// lua_register(L, "deltexture", deltexture);
	// lua_register(L, "settexture", settexture);
	// lua_register(L, "drawtexture", drawtexture);
	// lua_register(L, "updatetexture", updatetexture);
	// lua_register(L, "rectfill", rectfill);
	// lua_register(L, "clearout", clearout);
	// lua_register(L, "setcolor", setcolor);

	// lua_newtable(L);
	// lua_pushinteger(L, SDL_TEXTUREACCESS_TARGET);    lua_setfield(L, -2, "target");
	// lua_pushinteger(L, SDL_TEXTUREACCESS_STATIC);    lua_setfield(L, -2, "static");
	// lua_pushinteger(L, SDL_TEXTUREACCESS_STREAMING); lua_setfield(L, -2, "streaming");
	// lua_setglobal(L, "texturetype");

	SDL_SetAppMetadata("PROPIMA 0xB4", "0.1", "com.90sdev.propima0xb4");
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_HideCursor();
	window = SDL_CreateWindow("PROPIMA 0xB4", 320 * 3 + (30 * 2), 180 * 3 + (30 * 2), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
	SDL_SetTextureScaleMode(screen, SDL_SCALEMODE_NEAREST);

	SDL_SetRenderTarget(renderer, screen);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_FRect r = { .x = 0, .y = 0, .w = 320, .h = 180 };
	SDL_RenderFillRect(renderer, &r);

	resized();

	// lua_getglobal(L, "require");
	// lua_pushstring(L, "boot");
	// lua_call(L, 1, 0);

	return SDL_APP_CONTINUE;
}

static uint64_t last = 0;

SDL_AppResult SDL_AppIterate(void* appstate)
{
	uint64_t now = SDL_GetTicks();
	uint64_t diff = now - last;

	if (diff >= 33) {
		last = now;

		// lua_getglobal(L, "tick");
		// if (!lua_isnil(L, -1)) {
		// 	lua_pushinteger(L, diff);
		// 	lua_pcall(L, 1, 0, 0);
		// }
		// lua_settop(L, 0);
	}

	SDL_Delay(10);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {

		// case SDL_EVENT_MOUSE_MOTION:
		// 	lua_getglobal(L, "mousemove");
		// 	lua_pushinteger(L, floor((float)(e->motion.x - destrect.x) / (float)scale));
		// 	lua_pushinteger(L, floor((float)(e->motion.y - destrect.y) / (float)scale));
		// 	lua_pcall(L, 2, 0, 0);
		// 	lua_settop(L, 0);
		// 	break;

		// case SDL_EVENT_KEY_DOWN:
		// {
		// 	lua_getglobal(L, "keydown");
		// 	lua_pushinteger(L, e->key.scancode);
		// 	const char key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		// 	if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		// 	lua_pcall(L, 2, 0, 0);
		// 	lua_settop(L, 0);
		// 	break;
		// }

		// case SDL_EVENT_KEY_UP: {
		// 	lua_getglobal(L, "keyup");
		// 	lua_pushinteger(L, e->key.scancode);
		// 	const char key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		// 	if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		// 	lua_pcall(L, 2, 0, 0);
		// 	lua_settop(L, 0);
		// 	break;
		// }

		// case SDL_EVENT_MOUSE_BUTTON_DOWN:
		// 	lua_getglobal(L, "mousedown");
		// 	lua_pushinteger(L, e->button.button);
		// 	lua_pcall(L, 1, 0, 0);
		// 	lua_settop(L, 0);
		// 	break;

		// case SDL_EVENT_MOUSE_BUTTON_UP:
		// 	lua_getglobal(L, "mouseup");
		// 	lua_pushinteger(L, e->button.button);
		// 	lua_pcall(L, 1, 0, 0);
		// 	lua_settop(L, 0);
		// 	break;

		// case SDL_EVENT_MOUSE_WHEEL:
		// 	lua_getglobal(L, "mousewheel");
		// 	lua_pushnumber(L, e->wheel.x);
		// 	lua_pushnumber(L, e->wheel.y);
		// 	lua_pcall(L, 2, 0, 0);
		// 	lua_settop(L, 0);
		// 	break;

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
	// lua_close(L);
	SDL_DestroyTexture(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}
