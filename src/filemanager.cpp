#include "filemanager.hpp"
#include <SDL3/SDL.h>

std::filesystem::path userDir{ SDL_GetPrefPath("90sdev", "hram") };
std::filesystem::path appDir{ SDL_GetBasePath() };
std::filesystem::path bootFilePath{ userDir / "boot.lua" };

constexpr auto overwrite = false;

void copyBootFile(std::string name) {
	std::filesystem::copy_file(appDir / name, userDir / name,
		overwrite
		? std::filesystem::copy_options::overwrite_existing
		: std::filesystem::copy_options::skip_existing
	);
}

void copyBootFiles() {
	copyBootFile("boot.lua");
	copyBootFile("editor.lua");
	copyBootFile("prompt.lua");
}
