#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "app.hpp"
#include <argparse/argparse.hpp>
#include <format>
#include <vars.h>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::filesystem::path userDir(SDL_GetPrefPath("90sdev", "hram"));
	std::filesystem::path appDir(SDL_GetBasePath());
	std::filesystem::path userBootWat = userDir / "boot.wat";

	std::filesystem::copy_file(
		appDir / "boot.wat",
		userBootWat,
		std::filesystem::copy_options::skip_existing);

	argparse::ArgumentParser program("hram", std::format("{}.{}", VERMAJ, VERMIN));
	program.add_argument("-b", "--boot").help("path to boot file").default_value(userBootWat.string()).required();

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl << program;
		return SDL_APP_FAILURE;
	}

	auto app = new App(program.get("-b"));
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
