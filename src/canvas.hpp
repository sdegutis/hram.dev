#include <SDL3/SDL.h>

class Canvas {

	SDL_Window* window;
	SDL_GLContext glcontext;
	GLuint prog;
	GLuint vao;
	GLint resolutionLocation;
	GLint imageLocation;
	GLuint positionBuffer;


public:

	Canvas();
	~Canvas();

	void draw();

};
