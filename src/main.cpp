#include <stdio.h>

#include <epoxy/gl.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <vars.h>

#include <iostream>
#include <string>
#include <vector>

#include <lua.hpp>

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


GLuint tex;





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

GLint attribute_coord2d;
GLuint program;
GLuint vbo;
SDL_GLContext gl_context;




// helper to check and display for shader compiler errors
bool check_shader_compile_status(GLuint obj) {
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(obj, length, &length, &log[0]);
		std::cerr << &log[0];
		return false;
	}
	return true;
}

// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj) {
	GLint status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(obj, length, &length, &log[0]);
		std::cerr << &log[0];
		return false;
	}
	return true;
}





//void blit() {
//	glClear(GL_COLOR_BUFFER_BIT);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//	SDL_GL_SwapWindow(window);
//
//	//SDL_BlitSurfaceUncheckedScaled(appsurf, &srcrect, winsurf, &destrect, SDL_SCALEMODE_NEAREST);
//	//SDL_UpdateWindowSurface(window);
//}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	lua_State* lib = luaL_newstate();
	printf("%p ?\n", lib);

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("H-RAM", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	gl_context = SDL_GL_CreateContext(window);








	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//GLuint VertexArrayID;
	//glGenVertexArrays(1, &VertexArrayID);
	//glBindVertexArray(VertexArrayID);












	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode = R"(#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
void main(){
  gl_Position.xyz = vertexPosition_modelspace;
  gl_Position.w = 1.0;
}
)";

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode = R"(#version 330 core
out vec3 color;
void main(){
  color = vec3(1,0,0);
}
)";

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);












	static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);





	glClear(GL_COLOR_BUFFER_BIT);

	// Use our shader
	glUseProgram(ProgramID);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);


	SDL_GL_SwapWindow(window);


	//gl_context = SDL_GL_CreateContext(window);

	////GLv2 screen = glVertex2f(320 * 3, 180 * 3);

	///* Shader setup. */

	//GLchar* log = NULL;
	//GLint log_length, success;

	///* Vertex shader */
	//GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	//glCompileShader(vertex_shader);
	//glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	//glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
	//log = (GLchar*)malloc(log_length);
	//if (log_length > 0) {
	//	glGetShaderInfoLog(vertex_shader, log_length, NULL, log);
	//	printf("vertex shader log:\n\n%s\n", log);
	//}
	//if (!success) {
	//	printf("vertex shader compile error\n");
	//	exit(1);
	//}

	///* Fragment shader */
	//GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	//glCompileShader(fragment_shader);
	//glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	//glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
	//if (log_length > 0) {
	//	log = (GLchar*)realloc(log, log_length);
	//	glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
	//	printf("fragment shader log:\n\n%s\n", log);
	//}
	//if (!success) {
	//	printf("fragment shader compile error\n");
	//	exit(1);
	//}

	///* Link shaders */
	//GLuint program = glCreateProgram();
	//glAttachShader(program, vertex_shader);
	//glAttachShader(program, fragment_shader);
	//glLinkProgram(program);
	//glGetProgramiv(program, GL_LINK_STATUS, &success);
	//glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
	//if (log_length > 0) {
	//	log = (GLchar*)realloc(log, log_length);
	//	glGetProgramInfoLog(program, log_length, NULL, log);
	//	printf("shader link log:\n\n%s\n", log);
	//}
	//if (!success) {
	//	printf("shader link error");
	//	exit(1);
	//}

	///* Cleanup. */
	//free(log);
	//glDeleteShader(vertex_shader);
	//glDeleteShader(fragment_shader);


	//unsigned char texDat[8 * 8 * 4];
	//for (int i = 0; i < 64; ++i)
	//	texDat[i] = 1;

	//glGenTextures(1, &tex);
	//glBindTexture(GL_TEXTURE_2D, tex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, texDat);
	//glBindTexture(GL_TEXTURE_2D, 0);


	//attribute_coord2d = glGetAttribLocation(program, "coord2d");

	///* Buffer setup. */
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	///* Global draw state */
	//glUseProgram(program);
	//glViewport(0, 0, WIDTH, HEIGHT);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//glEnableVertexAttribArray(attribute_coord2d);



	//appsurf = SDL_CreateSurfaceFrom(320, 180, SDL_PIXELFORMAT_XRGB8888, rawdata, 320 * 4);
	//winsurf = SDL_GetWindowSurface(window);
	//resize();
	//blit();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	//Uint64 ticks = SDL_GetTicks();






	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {


	case SDL_EVENT_WINDOW_RESIZED:
		//SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
		//glViewport(0, 0, WIDTH, HEIGHT);
		////resize();
		//blit();
		break;


	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	case SDL_EVENT_MOUSE_MOTION:
		printf("%d, %d\n", (int)e->motion.x, (int)e->motion.y);
		*((uint32_t*)rawdata + (SDL_rand(320 * 180))) = SDL_rand(0xffffff);
		//blit();
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
	//glDeleteBuffers(1, &vbo);
	//glDeleteProgram(program);
	//SDL_GL_DestroyContext(gl_context);
	//SDL_DestroyWindow(window);
	//SDL_Quit();
}
