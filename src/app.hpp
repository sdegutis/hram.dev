#pragma once

#include <SDL3/SDL.h>
#include <filesystem>

class App {

	SDL_Window* win;
	std::filesystem::path bootFile;

public:

	App(std::string bootFile);

	void iterate();
	void mouseMoved(int32_t x, int32_t y);
	void mouseButton(int button, bool down);

};
