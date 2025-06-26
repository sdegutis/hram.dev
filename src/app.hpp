#include "filemanager.hpp"
#include "canvas.hpp"

class App {

	FileManager fileManager;
	Canvas canvas;

public:

	App();

	void iterate();
	void resized();
	void mouseMoved(int x, int y);
	void mouseButton(int button, bool down);

};
