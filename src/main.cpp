#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <vars.h>

#include <wasm_export.h>
#include "wat2wasm.hpp"

#include <functional>
#include <format>
#include <print>
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



SDL_Window* win;





SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);



	std::println("loading file");

	std::string s = R"(
		(module
		  (memory 1 1)
		  (func $boot (export "boot"))
		)
	)";

	std::vector<uint8_t> file_data(s.begin(), s.end());

	auto out = wat2wasm(s, file_data);
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


	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{


	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{

	switch (e->type) {

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		std::println("{},{}", e->motion.x, e->motion.y);
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		std::println("{},{}", e->button.button, e->button.down);
		break;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
