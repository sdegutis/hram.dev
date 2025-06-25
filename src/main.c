#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <vars.h>

#include <lua.h>
#include <lauxlib.h>

SDL_Window* window;

SDL_Rect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_Rect destrect;
int scale = 1;

SDL_Surface* appsurf;
SDL_Surface* winsurf;

uint8_t rawdata[320 * 180 * 4];

void resize() {
	destrect = srcrect;
	scale = 1;

	int w, h;
	auto winsize = SDL_GetWindowSize(window, &w, &h);

	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
		scale++;
		destrect.w += srcrect.w;
		destrect.h += srcrect.h;
	}

	destrect.x = w / 2 - destrect.w / 2;
	destrect.y = h / 2 - destrect.h / 2;
}

void blit() {
	SDL_BlitSurfaceUncheckedScaled(appsurf, &srcrect, winsurf, &destrect, SDL_SCALEMODE_NEAREST);
	SDL_UpdateWindowSurface(window);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	lua_State* lib = luaL_newstate();
	printf("%p ?\n", lib);

	window = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);
	appsurf = SDL_CreateSurfaceFrom(320, 180, SDL_PIXELFORMAT_XRGB8888, rawdata, 320 * 4);
	winsurf = SDL_GetWindowSurface(window);
	resize();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	Uint64 ticks = SDL_GetTicks();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		printf("%d, %d\n", (int)e->motion.x, (int)e->motion.y);
		*((uint32_t*)rawdata + (SDL_rand(320 * 180))) = SDL_rand(0xffffff);
		blit();
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		winsurf = SDL_GetWindowSurface(window);
		resize();
		blit();
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		printf("%d, %d", e->button.button, e->button.down);
		break;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
