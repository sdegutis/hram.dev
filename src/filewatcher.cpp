#include "filewatcher.hpp"

#include <efsw/efsw.hpp>
#include <SDL3/SDL.h>

class Updater : public efsw::FileWatchListener {
public:

	uint64_t* lastBootFileUpdate;
	std::filesystem::path basename;

	Updater(std::filesystem::path basename, uint64_t* lastBootFileUpdate)
		: basename(basename)
		, lastBootFileUpdate(lastBootFileUpdate)
	{
	}

	void handleFileAction(
		efsw::WatchID watchid,
		const std::string& dir,
		const std::string& filename,
		efsw::Action action,
		std::string oldFilename)
	{
		if (filename == basename) {
			*lastBootFileUpdate = SDL_GetTicks();
		}
	}

};

FileWatcher::FileWatcher(std::filesystem::path file)
	: file(file)
{
	auto fw = new efsw::FileWatcher();
	fw->addWatch(file.parent_path().string(), new Updater(file.filename(), &lastBootFileUpdate));
	fw->watch();
}

bool FileWatcher::didUpdate(uint64_t ticks)
{
	if (lastBootFileUpdate && ticks - lastBootFileUpdate > 200) {
		lastBootFileUpdate = 0;
		return true;
	}
	return false;
}
