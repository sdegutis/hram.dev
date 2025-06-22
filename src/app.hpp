#pragma once

#include <SDL3/SDL.h>
#include <filesystem>
#include <argparse/argparse.hpp>

class App {

	SDL_Window* win;
	std::filesystem::path userBootWat;

public:

	App(argparse::ArgumentParser& program);

	void update();
	void mouseMoved(int x, int y);
	void mouseButton(int button, bool down);

};
