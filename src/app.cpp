#include "app.hpp"

#include <wasm_export.h>
#include "wat2wasm.hpp"

#include <print>
#include <format>

#include <SDL3/SDL.h>

App::App(argparse::ArgumentParser& program) :
	userBootWat(program.get("-b"))
{

	wasm_runtime_init();

	//wasm_runtime_register_natives



	//	auto s_infile = readFile(R"(C:\Users\sbdeg\projects\hram\hello.wat)");
	//	std::vector<uint8_t> file_data;

	auto out = wat2wasm(R"(

(module
  (func $add)
  (export "add" (func $add))
  (func $foo)
  (start $foo)
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

	//auto x = toml::parse("foo = 'bar'");
	//std::print("Hello World! {}\n", x.is_boolean());

	win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);

}

void App::update()
{
}

void App::mouseMoved(int x, int y)
{
}

void App::mouseButton(int button, bool down)
{
}
