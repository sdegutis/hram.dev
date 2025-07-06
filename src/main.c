#include <glad/gl.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_gpu.h>
#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "shaders.h"

SDL_Window* window;
SDL_GLContext glcontext;
GLuint prog;
GLuint vao;
GLint resolutionLocation;
GLint iResolutionLocation;
GLint iTimeLoc;
GLint iChannelResolutionLocation;
GLuint posBuf;

SDL_FRect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_FRect destrect;
int scale = 1;

lua_State* L;

static void redraw() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	SDL_GL_SwapWindow(window);
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

	glBindVertexArray(vao);
	glViewport(0, 0, w, h);
	glUniform2f(resolutionLocation, w, h);

	glUniform3f(iResolutionLocation, 320 * scale, 180 * scale, 1);

	glBindBuffer(GL_ARRAY_BUFFER, posBuf);

	float x1 = destrect.x;
	float x2 = destrect.x + destrect.w;
	float y1 = destrect.y;
	float y2 = destrect.y + destrect.h;
	float xys[12] = { x1,y1,x2,y1,x1,y2,x1,y2,x2,y1,x2,y2 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(xys), xys, GL_STATIC_DRAW);

	redraw();
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	L = luaL_newstate();
	luaL_openlibs(L);

	//lua_pushstring(L, SDL_GetPrefPath("", "hram"));
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

	SDL_SetAppMetadata("hram", "0.1", "com.90sdev.hram");
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("HRAM", 320 * 3 + (30 * 2), 180 * 3 + (30 * 2), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	//renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetWindowMinimumSize(window, 320, 180);



	glcontext = SDL_GL_CreateContext(window);
	gladLoadGL(SDL_GL_GetProcAddress);

	prog = createShaders();
	glUseProgram(prog);

	resolutionLocation = glGetUniformLocation(prog, "u_resolution");
	iResolutionLocation = glGetUniformLocation(prog, "iResolution");
	iTimeLoc = glGetUniformLocation(prog, "iTime");
	iChannelResolutionLocation = glGetUniformLocation(prog, "iChannelResolution");
	GLint imageLocation = glGetUniformLocation(prog, "iChannel0");

	glUniform3f(iChannelResolutionLocation, 320, 180, 1);

	glClearColor(.1f, .1f, .1f, 1.f);

	uint8_t* data = malloc(320 * 180 * 4);
	if (!data) return SDL_APP_FAILURE;

	memset(data, 0, 320 * 180 * 4);


	uint8_t font[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0,
		1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0,
		1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0,
		1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0,
		0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0,
		1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0,
		1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	for (int y = 0; y < 6 * 6; y++) {
		for (int x = 0; x < 16 * 4; x++) {
			int di = y * 320 + x;
			int fi = y * 64 + x;
			*((uint32_t*)data + di) = font[fi] ? 0xff0000ff : 0;
		}
	}



	GLuint texture1;
	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);


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

	glUniform1f(iTimeLoc, (double)now / 1000.0);
	uint32_t data[1] = { SDL_rand_bits() | 0xff000000 };
	int dx = SDL_rand(320);
	int dy = SDL_rand(180);
	glTexSubImage2D(GL_TEXTURE_2D, 0, dx, dy, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, data);
	redraw();

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

bool cursorHidden = false;

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {

	case SDL_EVENT_MOUSE_MOTION:
		//lua_getglobal(L, "mousemove");
		//lua_pushinteger(L, floor((float)(e->motion.x - destrect.x) / (float)scale));
		//lua_pushinteger(L, floor((float)(e->motion.y - destrect.y) / (float)scale));
		//lua_pcall(L, 2, 0, 0);
		//lua_settop(L, 0);

	{
		//SDL_FRect

		SDL_FPoint p;
		p.x = e->motion.x;
		p.y = e->motion.y;

		bool inr = SDL_PointInRectFloat(&p, &destrect);

		if (inr && !cursorHidden) {
			cursorHidden = true;
			SDL_HideCursor();
		}
		else if (!inr && cursorHidden) {
			cursorHidden = false;
			SDL_ShowCursor();
		}



		uint32_t data[1] = { SDL_rand_bits() | 0xff000000 };
		int dx = floor((float)(e->motion.x - destrect.x) / (float)scale);
		int dy = floor((float)(e->motion.y - destrect.y) / (float)scale);
		printf("%d,%d\n", dx, dy);
		glTexSubImage2D(GL_TEXTURE_2D, 0, dx, dy, 1, 1, GL_BGRA, GL_UNSIGNED_BYTE, data);
		redraw();
	}

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
