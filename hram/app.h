#pragma once


namespace app {

	void boot();

	void mouseMoved(int x, int y);
	void mouseDown(int b);
	void mouseUp(int b);
	void mouseWheel(int d);

	void keyDown(int vk);
	void keyUp(int vk);
	void keyChar(const char ch);

}
