#include "app.hpp"

#include <print>

#include <SDL3/SDL.h>

App::App()
{
	win = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE);
}

void App::iterate()
{
	auto ticks = SDL_GetTicks();
}

void App::mouseMoved(int32_t x, int32_t y)
{
	std::println("{},{}", x, y);
}

void App::mouseButton(int button, bool down)
{
	std::println("{},{}", button, down);
}
