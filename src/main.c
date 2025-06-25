#include <stdio.h>

#include <epoxy/gl.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <vars.h>

#include <lua.h>
#include <lauxlib.h>

SDL_Window* window;

SDL_Rect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_Rect destrect;
int scale = 1;


static int WIDTH = 320 * 2;
static int HEIGHT = 180 * 2;



static const GLchar* vertex_shader_source =
"#version 120\n"
"attribute vec2 coord2d;\n"
"void main() {\n"
"    gl_Position = vec4(coord2d, 0.0, 1.0);\n"
"}\n";

static const GLchar* fragment_shader_source =
"#version 120\n"
"void main() {\n"
"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

static GLfloat vertices[] = {
	 0.0,  0.8,
	-0.8, -0.8,
	 0.8, -0.8,
};







//SDL_Surface* appsurf;
//SDL_Surface* winsurf;

uint8_t rawdata[320 * 180 * 4];

void resize() {
	destrect = srcrect;
	scale = 1;

	int w, h;
	auto winsize = SDL_GetWindowSize(window, &w, &h);

	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
		scale++;
		destrect.w += srcrect.w;
		destrect.h += srcrect.h;
	}

	destrect.x = w / 2 - destrect.w / 2;
	destrect.y = h / 2 - destrect.h / 2;
}

void blit() {
	//SDL_BlitSurfaceUncheckedScaled(appsurf, &srcrect, winsurf, &destrect, SDL_SCALEMODE_NEAREST);
	//SDL_UpdateWindowSurface(window);
}

GLint attribute_coord2d;
GLuint program;
GLuint vbo;
SDL_GLContext gl_context;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	lua_State* lib = luaL_newstate();
	printf("%p ?\n", lib);

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("H-RAM", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);




	gl_context = SDL_GL_CreateContext(window);

	//GLv2 screen = glVertex2f(320 * 3, 180 * 3);

	/* Shader setup. */

	GLchar* log = NULL;
	GLint log_length, success;

	/* Vertex shader */
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
	log = (GLchar*)malloc(log_length);
	if (log_length > 0) {
		glGetShaderInfoLog(vertex_shader, log_length, NULL, log);
		printf("vertex shader log:\n\n%s\n", log);
	}
	if (!success) {
		printf("vertex shader compile error\n");
		exit(1);
	}

	/* Fragment shader */
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 0) {
		log = (GLchar*)realloc(log, log_length);
		glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
		printf("fragment shader log:\n\n%s\n", log);
	}
	if (!success) {
		printf("fragment shader compile error\n");
		exit(1);
	}

	/* Link shaders */
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length > 0) {
		log = (GLchar*)realloc(log, log_length);
		glGetProgramInfoLog(program, log_length, NULL, log);
		printf("shader link log:\n\n%s\n", log);
	}
	if (!success) {
		printf("shader link error");
		exit(1);
	}

	/* Cleanup. */
	free(log);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);





	attribute_coord2d = glGetAttribLocation(program, "coord2d");

	/* Buffer setup. */
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Global draw state */
	glUseProgram(program);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);




	//appsurf = SDL_CreateSurfaceFrom(320, 180, SDL_PIXELFORMAT_XRGB8888, rawdata, 320 * 4);
	//winsurf = SDL_GetWindowSurface(window);
	resize();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	Uint64 ticks = SDL_GetTicks();



	glClear(GL_COLOR_BUFFER_BIT);
	glEnableVertexAttribArray(attribute_coord2d);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);
	SDL_GL_SwapWindow(window);



	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {


	case SDL_EVENT_WINDOW_RESIZED:
		SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
		glViewport(0, 0, WIDTH, HEIGHT);
		//resize();
		//blit();
		break;


	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		printf("%d, %d\n", (int)e->motion.x, (int)e->motion.y);
		*((uint32_t*)rawdata + (SDL_rand(320 * 180))) = SDL_rand(0xffffff);
		blit();
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		printf("%d, %d", e->button.button, e->button.down);
		break;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(program);
	SDL_GL_DestroyContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
