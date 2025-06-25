#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <format>
#include <vars.h>
#include <print>

#include <lua.hpp>
#include <filesystem>

class App
{

	SDL_Window* win;

public:

	App();

	void iterate();
	void mouseMoved(int32_t x, int32_t y);
	void mouseButton(int button, bool down);

};

App::App()
{
	win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);
}

void App::iterate()
{
	auto ticks = SDL_GetTicks();
}

void App::mouseMoved(int32_t x, int32_t y)
{
	std::println("{},{}", x, y);
}

void App::mouseButton(int button, bool down)
{
	std::println("{},{}", button, down);
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	auto lib = luaL_newstate();
	printf("%p ?\n", lib);

	//std::filesystem::path userDir(SDL_GetPrefPath("90sdev", "hram"));
	//std::filesystem::path appDir(SDL_GetBasePath());
	//std::filesystem::path userBootWat = userDir / "boot.wat";

	//std::filesystem::copy_file(
	//	appDir / "boot.wat",
	//	userBootWat,
	//	std::filesystem::copy_options::skip_existing);

	auto app = new App();
	*appstate = app;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	auto app = (App*)appstate;
	app->iterate();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	auto app = (App*)appstate;

	switch (e->type) {

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		app->mouseMoved(e->motion.x, e->motion.y);
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		app->mouseButton(e->button.button, e->button.down);
		break;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
