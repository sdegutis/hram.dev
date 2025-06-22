#pragma once

#include <SDL3/SDL.h>
#include <filesystem>
#include <argparse/argparse.hpp>

class App {

	SDL_Window* win;
	std::filesystem::path userBootWat;

public:

	App(argparse::ArgumentParser& program);

	void iterate();
	void mouseMoved(int32_t x, int32_t y);
	void mouseButton(int button, bool down);

};
