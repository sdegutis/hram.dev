#include <filesystem>

class FileWatcher {

	std::filesystem::path file;
	uint64_t lastBootFileUpdate = 0;

public:

	FileWatcher(std::filesystem::path file);

	bool didUpdate(uint64_t ticks);

};
