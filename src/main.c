#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <stdio.h>
#include <vars.h>

#include <wasm.h>



// 0x0000 <= userd < 0x7000  (28 KiB)
// 0x7000 <= rsrvd < 0x7bb0
// 2304 bits (288 bytes) needed for font
// 0x7bb0 <= mouse < 0x7bbc  (i32 x 3)
// 0x7bbc <= clock < 0x7bc0  (i32 x 1)
// 0x7bc0 <= keybd < 0x7c00  (i32 x 16)
// 0x7c00 <= video < 0x40000 (i32 x 320*180)


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


	wasm_engine_t engine = wasm_engine_new();
	printf("%p\n", engine);



	//wasmtime::Engine engine;

	//auto mod = wasmtime::Module::compile(engine, R"(
	//	(module
 //         (import "os" "foo" (func $foo))
	//	  (memory 1 1)
	//	  (func $boot (export "boot")
	//		(call $foo)
	//		(call $foo)
	//		(call $foo)
	//		)
	//	)
	//)").unwrap();

	//wasmtime::Store store(engine);
	//wasmtime::Func f = wasmtime::Func::wrap(store, []() { std::println("IN here!"); });
	//auto inst = wasmtime::Instance::create(store, mod, { f }).unwrap();
	//auto run = std::get<wasmtime::Func>(*inst.get(store, "boot"));
	//run.call(store, {}).unwrap();

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
		printf("%f,%f\n", e->motion.x, e->motion.y);
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		printf("%d,%d\n", e->button.button, e->button.down);
		break;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
