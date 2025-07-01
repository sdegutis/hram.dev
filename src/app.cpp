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

#include <functional>


App::App(std::string bootFileStr)
	: bootFile(bootFileStr)
	, fileWatcher(bootFile)
{
	win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);
	wasm_runtime_init();
	loadFile();
}

void App::iterate()
{
	auto ticks = SDL_GetTicks();
	if (fileWatcher.didUpdate(ticks)) {
		loadFile();
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

void App::loadFile()
{
	std::println("loading file");

	std::ifstream file(bootFile, std::ios::in | std::ios::binary);
	std::stringstream buf;
	buf << file.rdbuf();

	std::string s = buf.str();

	std::vector<uint8_t> file_data(s.begin(), s.end());

	auto out = wat2wasm(bootFile.string(), file_data);
	auto& val = out.value();

	wasm_runtime_init();

	char error_buf[128];
	auto mod = wasm_runtime_load(val.data(), val.size(), error_buf, sizeof(error_buf));
	std::println("mod is null? {}", mod == NULL);

	auto modinst = wasm_runtime_instantiate(mod, 256, 256, error_buf, sizeof(error_buf));

	auto func = wasm_runtime_lookup_function(modinst, "add");
	std::println("func is null? {}", func == NULL);

	//wasm_val_t results;

	//wasm_runtime_call_wasm()
	//wasm_runtime_call_wasm_a()
	//wasm_runtime_call_wasm_v()

	//wasm_memory_get_base_address
	//wasm_runtime_addr_app_to_native

	uint64_t in, o;
	auto worked = wasm_runtime_get_app_addr_range(modinst, 0, &in, &o);
	std::println("worked {}", worked);
	if (worked) {
		std::println("addr {} {}", in, o);
	}

	auto mem = wasm_runtime_get_memory(modinst, 0);
	auto base = (uint8_t*)wasm_memory_get_base_address(mem);
	std::println("base addr {}", base == NULL);

	uint8_t* a, * b;
	auto worked2 = wasm_runtime_get_native_addr_range(modinst, base, &a, &b);
	std::println("worked 2 {}", worked2);
	if (worked2) {
		std::println("addr {}", base == a);
		std::println("addr {}", b == base + o);
		std::println("addr {}", (void*)base);
		std::println("addr {}", (void*)a);
		std::println("addr {}", (void*)b);
	}

	//wasm_runtime_create_exec_env

	wasm_runtime_deinstantiate(modinst);

	//wasm_runtime_destroy_exec_env

	wasm_runtime_unload(mod);

}
