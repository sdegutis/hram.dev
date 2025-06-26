#include <filesystem>

class FileManager {

public:

	FileManager();

	void copyBootFiles();
	std::filesystem::path bootFilePath;

};
