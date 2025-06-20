#include <fmt/core.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <toml++/toml.hpp>

#include <wasm_export.h>

// using namespace std::literals;
//#include <cassert>
//#include <cstdarg>
//#include <cstdint>
//#include <cstdio>
//#include <cstdlib>
//#include <string>
//
//#include <wabt/config.h>
//
//#include <wabt/binary-writer.h>
//#include <wabt/common.h>
//#include <wabt/error-formatter.h>
//#include <wabt/feature.h>
//#include <wabt/filenames.h>
//#include <wabt/ir.h>
//#include <wabt/option-parser.h>
//#include <wabt/resolve-names.h>
//#include <wabt/stream.h>
//#include <wabt/validator.h>
//#include <wabt/wast-parser.h>

std::string readFile(const char* name) {
	std::ifstream watFile;
	watFile.open(name);
	std::stringstream strStream;
	strStream << watFile.rdbuf();

	int _;
	_ = 0;

	return strStream.str();
}

void domod() {
	//auto s_infile = readFile(R"(C:\Users\sbdeg\projects\hram\hello.wat)");
	//std::vector<uint8_t> file_data;

	//wabt::Errors errors;
	//std::unique_ptr<wabt::WastLexer> lexer = wabt::WastLexer::CreateBufferLexer(
	//	s_infile.c_str(), file_data.data(), file_data.size(), &errors);


	//std::unique_ptr<wabt::Module> module;
	//wabt::Features s_features;
	//wabt::WastParseOptions parse_wast_options(s_features);
	//auto result = wabt::ParseWatModule(lexer.get(), &module, &errors, &parse_wast_options);
	//fmt::println("{}", wabt::Succeeded(result));

	//char error_buf[128];
	//auto mod = wasm_runtime_load((uint8_t*)file.c_str(), file.size(), error_buf, sizeof(error_buf));

	//fmt::println("{}", mod == NULL);
	//fmt::println("{}", error_buf);

}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	fmt::print("Hello World!\n");


	wasm_runtime_init();
	domod();

	//wasm_engine_t* engine = wasm_engine_new();
	//fmt::print("engine = {}\n", engine != NULL);

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
