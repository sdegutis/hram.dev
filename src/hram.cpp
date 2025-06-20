#include <fmt/core.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <toml++/toml.hpp>

#include <wasm_export.h>

#include "wat.hpp"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	wasm_runtime_init();

	//	auto s_infile = readFile(R"(C:\Users\sbdeg\projects\hram\hello.wat)");
	//	std::vector<uint8_t> file_data;

	auto out = parse_wat("(module)");
	fmt::println("{}", out.has_value());

	auto& val = out.value();

	//char error_buf[128];
	//auto mod = wasm_runtime_load(val.data(), val.size(), error_buf, sizeof(error_buf));

	//fmt::println("{}", mod == NULL);
	//fmt::println("{}", error_buf);

	auto x = toml::parse("foo = 'bar'");
	fmt::print("Hello World! {}\n", x.is_boolean());

	SDL_Window* win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);

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
}
