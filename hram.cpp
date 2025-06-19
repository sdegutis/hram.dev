#include <fmt/core.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <toml++/toml.hpp>

#include <wasm_c_api.h>

using namespace std::literals;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	fmt::print("Hello World!\n");

	wasm_engine_t* engine = wasm_engine_new();
	fmt::print("engine = {}\n", engine != NULL);

	auto x = toml::parse("foo = 'bar'");
	fmt::print("Hello World! {}\n", x.is_boolean());

	SDL_Window* win = SDL_CreateWindow("hram", 320 * 2, 180 * 2, SDL_WINDOW_RESIZABLE);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e) {
	switch (e->type) {

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	fmt::println("quittin");
}
