#include "app.hpp"

#include <wasm_export.h>
#include "wat2wasm.hpp"

#include <print>
#include <format>
#include <fstream>

#include <SDL3/SDL.h>


/**
*
* keybd = (512 bytes) i8 * 512
* mouse = (12 bytes) i32 * 3
* fontd = (96 bytes) i8 * 96 (16 * 6)
* clock = (8 bytes) i64
* gampd = (152 bytes) i16 * 6 + 26 * i8 * 4 controllers
*
* rand(i32) => (i32)
* blit(insaddr: i32)
* resize(i32,i32)
*
* start()
* loop()
*
!*/


App::App(std::string bootFileStr)
	: bootFile(bootFileStr)
	, fileWatcher(bootFile)
{



	wasm_runtime_init();


	std::ifstream file(bootFile);
	std::stringstream buf;
	buf << file.rdbuf();

	auto out = wat2wasm(buf.str());
	auto& val = out.value();

	char error_buf[128];
	auto mod = wasm_runtime_load(val.data(), val.size(), error_buf, sizeof(error_buf));
	std::println("mod is null? {}", mod == NULL);

	auto modinst = wasm_runtime_instantiate(mod, 8092, 8092, error_buf, sizeof(error_buf));

	auto func = wasm_runtime_lookup_function(modinst, "add");
	std::println("func is null? {}", func == NULL);

	//fmt::println("{}", error_buf);

	//auto x = toml::parse("foo = 'bar'");
	//std::print("Hello World! {}\n", x.is_boolean());

	win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);
}

void App::iterate()
{
	auto ticks = SDL_GetTicks();
	if (fileWatcher.didUpdate(ticks)) {
		std::println("now udpating");
	}
}

void App::mouseMoved(int32_t x, int32_t y)
{
	std::println("{},{}", x, y);
}

void App::mouseButton(int button, bool down)
{
	std::println("{},{}", button, down);
}
