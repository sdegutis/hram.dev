#define SDL_MAIN_USE_CALLBACKS
#include <glad/gl.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include "shaders.h"

int padding = 30;
SDL_Window* window;
SDL_GLContext glcontext;
GLuint prog;
GLuint vao;
GLint resolutionLocation;
GLuint posBuf;

lua_State* L;

SDL_Rect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_Rect destrect;
int scale = 1;

static void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	SDL_GL_SwapWindow(window);
}

static void resized()
{
	destrect = srcrect;
	scale = 1;

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
		scale++;
		destrect.w += srcrect.w;
		destrect.h += srcrect.h;
	}

	destrect.x = w / 2 - destrect.w / 2;
	destrect.y = h / 2 - destrect.h / 2;

	glBindVertexArray(vao);
	glViewport(0, 0, w, h);
	glUniform2f(resolutionLocation, w, h);




	glBindBuffer(GL_ARRAY_BUFFER, posBuf);

	float x1 = destrect.x;
	float x2 = destrect.x + destrect.w;
	float y1 = destrect.y;
	float y2 = destrect.y + destrect.h;
	float xys[12] = { x1,y1,x2,y1,x1,y2,x1,y2,x2,y1,x2,y2 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(xys), xys, GL_STATIC_DRAW);

	draw();
}

static int drawimg(lua_State* L) {
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	uint8_t* data = lua_touserdata(L, 1);
	int sx = lua_tonumber(L, 2);
	int sy = lua_tonumber(L, 3);
	int sw = lua_tonumber(L, 4);
	int sh = lua_tonumber(L, 5);
	int dx = lua_tonumber(L, 6);
	int dy = lua_tonumber(L, 7);

	int i = sy * 320 + sx;
	glTexSubImage2D(GL_TEXTURE_2D, 0, dx, dy, sw, sh, GL_BGRA, GL_UNSIGNED_BYTE, (uint32_t*)data + i);
	return 0;
}

static int blit(lua_State* L) {
	draw();
	return 0;
}

static int opendir(lua_State* L) {
	char* s = lua_tostring(L, -1);
	bool res = SDL_OpenURL(s);
	lua_pushboolean(L, res);
	return 1;
}

static int setfullscreen(lua_State* L) {
	bool b = lua_toboolean(L, -1);
	bool res = SDL_SetWindowFullscreen(window, b);
	if (res) resized();
	lua_pushboolean(L, res);
	return 1;
}

static int memorycopy(lua_State* L) {
	uint32_t* dst = lua_touserdata(L, 1);
	uint64_t dstoff = lua_tointeger(L, 2);
	uint32_t* src = lua_touserdata(L, 3);
	uint64_t srcoff = lua_tointeger(L, 4);
	uint64_t siz = lua_tointeger(L, 5);
	if (dstoff < 0 || dstoff >= 320 * 180 ||
		srcoff < 0 || srcoff >= 320 * 180 ||
		dstoff + siz > 320 * 180 ||
		srcoff + siz > 320 * 180)
	{
		lua_warning(L, "memcpy outside bounds", false);
		lua_pushboolean(L, false);
		return 1;
	}
	memcpy(dst + dstoff, src + srcoff, siz * 4);
	lua_pushboolean(L, true);
	return 1;
}

static int newmem(lua_State* L) {
	uint64_t size = lua_tointeger(L, 1);
	lua_newuserdata(L, size * 4);
	return 1;
}

static int memoryset(lua_State* L) {
	uint32_t* data = lua_touserdata(L, 1);
	uint64_t off = lua_tointeger(L, 2);
	uint64_t val = lua_tointeger(L, 3);
	uint64_t siz = lua_tointeger(L, 4);
	if (off < 0 || off >= 320 * 180 ||
		off + siz > 320 * 180)
	{
		lua_warning(L, "memset outside bounds", false);
		lua_pushboolean(L, false);
		return 1;
	}

	data += off;
	for (uint32_t* done = data + siz; data != done; data++)
		*data = val;

	lua_pushboolean(L, true);
	return 1;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushstring(L, SDL_GetPrefPath("", "progma0xb4"));
	lua_setglobal(L, "userdir");

	luaL_dostring(L, "package.path = userdir .. '?.lua;' .. package.path");

	lua_register(L, "blit", blit);
	lua_register(L, "drawimg", drawimg);
	lua_register(L, "opendir", opendir);
	lua_register(L, "setfullscreen", setfullscreen);
	lua_register(L, "memcpy", memorycopy);
	lua_register(L, "memset", memoryset);
	lua_register(L, "newmem", newmem);



	SDL_SetAppMetadata("PROPIMA 0xB4", "0.1", "com.90sdev.propima0xb4");
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	SDL_HideCursor();

	window = SDL_CreateWindow("PROPIMA 0xB4", 320 * 3 + (padding * 2), 180 * 3 + (padding * 2), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	glcontext = SDL_GL_CreateContext(window);
	gladLoadGL(SDL_GL_GetProcAddress);

	prog = createShaders();
	glUseProgram(prog);

	resolutionLocation = glGetUniformLocation(prog, "u_resolution");
	GLint imageLocation = glGetUniformLocation(prog, "u_image");

	glClearColor(.1f, .1f, .1f, 1.f);

	GLuint texture1;
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	//GLuint texture2;
	//glGenTextures(1, &texture2);
	//glActiveTexture(GL_TEXTURE0 + 1);
	//glBindTexture(GL_TEXTURE_2D, texture2);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	//glActiveTexture(GL_TEXTURE0 + 0);


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUniform1i(imageLocation, 0);

	glGenBuffers(1, &posBuf);
	GLint posAttrLoc = glGetAttribLocation(prog, "a_position");
	glEnableVertexAttribArray(posAttrLoc);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, false, 0, 0);

	GLuint texNormBuf;
	glGenBuffers(1, &texNormBuf);
	glBindBuffer(GL_ARRAY_BUFFER, texNormBuf);
	float texNormData[12] = { 0,0,1,0,0,1,0,1,1,0,1,1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(texNormData), texNormData, GL_STATIC_DRAW);

	GLint texNormAttrLoc = glGetAttribLocation(prog, "a_texCoord");
	glEnableVertexAttribArray(texNormAttrLoc);
	glVertexAttribPointer(texNormAttrLoc, 2, GL_FLOAT, false, 0, 0);

	resized();


	lua_getglobal(L, "require");
	lua_pushstring(L, "boot");
	lua_call(L, 1, 0);


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

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {

	case SDL_EVENT_WINDOW_RESIZED:
		resized();
		break;

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
		int key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
		if (key >= 32 && key <= 126) lua_pushlstring(L, &key, 1); else lua_pushnil(L);
		lua_pcall(L, 2, 0, 0);
		lua_settop(L, 0);
		break;
	}

	case SDL_EVENT_KEY_UP: {
		lua_getglobal(L, "keyup");
		lua_pushinteger(L, e->key.scancode);
		int key = SDL_GetKeyFromScancode(e->key.scancode, e->key.mod, false);
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

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	glDeleteProgram(prog);
	SDL_GL_DestroyContext(glcontext);
	SDL_DestroyWindow(window);
}
