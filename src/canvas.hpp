#include <epoxy/gl.h>
#include <SDL3/SDL.h>

class Canvas {

	SDL_Window* window;
	SDL_GLContext glcontext;
	GLuint prog;
	GLuint vao;
	GLint resolutionLocation;
	GLint imageLocation;
	GLuint posBuf;

	SDL_Rect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
	SDL_Rect destrect;
	int scale = 1;

	unsigned char data[320 * 180 * 4];


public:

	Canvas();
	~Canvas();

	void resized();
	void draw();
	void mouseMoved(int x, int y);
	void iterate();

};
