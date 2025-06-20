#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <toml++/toml.hpp>

#include <wasm_export.h>

#include "wat.hpp"

#include <print>
#include <format>
#include <filesystem>
#include <string>
#include <config.h>


//#include <fstream>
//#include <sstream>
//
//std::string readFile(const char* name) {
//	std::ifstream watFile;
//	watFile.open(name);
//	std::stringstream strStream;
//	strStream << watFile.rdbuf();
//	return strStream.str();
//}

/**
*
* keybd = i8 * 512
* mouse = i32 * 3
*
*
*/

#include <argparse/argparse.hpp>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	std::filesystem::path configpath(SDL_GetPrefPath("90sdev", "hram"));
	configpath.append("boot.wat");
	//std::println("path {}", configpath.generic_string());
	std::println("maj {} min {}", VERMAJ, VERMIN);

	auto exists = std::filesystem::exists(configpath);
	std::println("path {}", exists);

	argparse::ArgumentParser program("hram");
	program.add_argument("--boot").nargs(argparse::nargs_pattern::optional).help("boot.wasm path").default_value(configpath.generic_string());

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return SDL_APP_FAILURE;
	}


	wasm_runtime_init();

	//wasm_runtime_register_natives



	//	auto s_infile = readFile(R"(C:\Users\sbdeg\projects\hram\hello.wat)");
	//	std::vector<uint8_t> file_data;

	auto out = parse_wat(R"(

(module
  (func $add)
  (export "add" (func $add))
)

)");
	auto& val = out.value();

	char error_buf[128];
	auto mod = wasm_runtime_load(val.data(), val.size(), error_buf, sizeof(error_buf));
	std::println("mod is null? {}", mod == NULL);

	auto modinst = wasm_runtime_instantiate(mod, 8092, 8092, error_buf, sizeof(error_buf));

	auto func = wasm_runtime_lookup_function(modinst, "add");
	std::println("func is null? {}", func == NULL);

	//fmt::println("{}", error_buf);

	auto x = toml::parse("foo = 'bar'");
	std::print("Hello World! {}\n", x.is_boolean());

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

	case SDL_EVENT_MOUSE_MOTION:
		std::println("{},{}", e->motion.x, e->motion.y);
		break;

	}

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
}
