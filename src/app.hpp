#pragma once

#include <SDL3/SDL.h>

class App
{

	SDL_Window* win;

public:

	App();

	void iterate();
	void mouseMoved(int32_t x, int32_t y);
	void mouseButton(int button, bool down);

};
