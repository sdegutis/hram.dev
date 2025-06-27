#define SDL_MAIN_USE_CALLBACKS
#include <glad/gl.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <filesystem>
#include <print>
#include <lua.hpp>
#include <battery/embed.hpp>

SDL_Window* window;
SDL_GLContext glcontext;
GLuint prog;
GLuint vao;
GLint resolutionLocation;
GLuint posBuf;

lua_State* L;

SDL_Rect srcrect = { .x = 0, .y = 0, .w = 320, .h = 180 };
SDL_Rect destrect;
int scale = 1;

uint8_t data[320 * 180 * 4];

void resized();
void draw();
void mouseMoved(int x, int y);

GLuint createShaders();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	L = luaL_newstate();
	luaL_openlibs(L);

	std::filesystem::path userDir{ SDL_GetPrefPath("", "progma0x140") };

	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");
	auto oldpath = lua_tostring(L, -1);
	auto newpath = (userDir / "?.lua").string() + ";" + oldpath;
	lua_pop(L, 1);
	lua_pushstring(L, newpath.c_str());
	lua_setfield(L, -2, "path");

	lua_getglobal(L, "require");
	lua_pushstring(L, "boot");
	lua_call(L, 1, 0);

	lua_settop(L, 0);

	SDL_Init(SDL_INIT_VIDEO);



	window = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	glcontext = SDL_GL_CreateContext(window);
	gladLoadGL(SDL_GL_GetProcAddress);

	prog = createShaders();
	glUseProgram(prog);

	resolutionLocation = glGetUniformLocation(prog, "u_resolution");
	GLint imageLocation = glGetUniformLocation(prog, "u_image");

	glClearColor(0.f, 0.f, 0.f, 1.f);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	memset(data, 0, 320 * 180 * 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);



	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUniform1i(imageLocation, 0);

	glGenBuffers(1, &posBuf);
	auto posAttrLoc = glGetAttribLocation(prog, "a_position");
	glEnableVertexAttribArray(posAttrLoc);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf);
	glVertexAttribPointer(posAttrLoc, 2, GL_FLOAT, false, 0, 0);

	GLuint texNormBuf;
	glGenBuffers(1, &texNormBuf);
	glBindBuffer(GL_ARRAY_BUFFER, texNormBuf);
	float texNormData[12] = { 0,0,1,0,0,1,0,1,1,0,1,1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(texNormData), texNormData, GL_STATIC_DRAW);

	auto texNormAttrLoc = glGetAttribLocation(prog, "a_texCoord");
	glEnableVertexAttribArray(texNormAttrLoc);
	glVertexAttribPointer(texNormAttrLoc, 2, GL_FLOAT, false, 0, 0);

	resized();


	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	lua_getglobal(L, "tick");
	if (lua_isfunction(L, -1)) {
		lua_pushinteger(L, SDL_GetTicks());
		lua_call(L, 1, 0);
	}
	lua_settop(L, 0);

	return SDL_APP_CONTINUE;
}

bool fullscreen = false;

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	switch (e->type) {

	case SDL_EVENT_WINDOW_RESIZED:
		resized();
		break;

	case SDL_EVENT_MOUSE_MOTION:
		mouseMoved(e->motion.x, e->motion.y);
		break;

	case SDL_EVENT_KEY_DOWN:
		if (e->key.scancode == SDL_SCANCODE_F11) {
			fullscreen = !fullscreen;
			SDL_SetWindowFullscreen(window, fullscreen);
			resized();
		}
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		printf("%d, %d\n", e->button.button, e->button.down);
		break;

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	glDeleteProgram(prog);
	SDL_GL_DestroyContext(glcontext);
	SDL_DestroyWindow(window);
}

void resized()
{
	destrect = srcrect;
	scale = 1;

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
		scale++;
		destrect.w += srcrect.w;
		destrect.h += srcrect.h;
	}

	destrect.x = w / 2 - destrect.w / 2;
	destrect.y = h / 2 - destrect.h / 2;

	glBindVertexArray(vao);
	glViewport(0, 0, w, h);
	glUniform2f(resolutionLocation, w, h);




	glBindBuffer(GL_ARRAY_BUFFER, posBuf);

	float x1 = destrect.x;
	float x2 = destrect.x + destrect.w;
	float y1 = destrect.y;
	float y2 = destrect.y + destrect.h;
	float xys[12] = { x1,y1,x2,y1,x1,y2,x1,y2,x2,y1,x2,y2 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(xys), xys, GL_STATIC_DRAW);


	draw();
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	SDL_GL_SwapWindow(window);
}

void mouseMoved(int x, int y)
{
	x = SDL_rand(320);
	y = SDL_rand(180);

	int i = y * 320 + x;

	data[i + 0] = 255;
	data[i + 1] = 255;
	data[i + 2] = 255;
	data[i + 3] = 0;

	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data + i);

	draw();

	printf("%d, %d\n", x, y);
}

GLuint createShaders() {
	std::string vertShader = b::embed<"resources/vert.shader">();
	std::string fragShader = b::embed<"resources/frag.shader">();

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	GLint res = GL_FALSE;
	int logi;

	char const* vertsrc = vertShader.c_str();
	glShaderSource(vert, 1, &vertsrc, NULL);
	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &res);
	glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &logi);
	if (logi > 0) {
		std::vector<char> err(logi + 1);
		glGetShaderInfoLog(vert, logi, NULL, &err[0]);
		std::println("{}", &err[0]);
	}

	char const* fragsrc = fragShader.c_str();
	glShaderSource(frag, 1, &fragsrc, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &res);
	glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logi);
	if (logi > 0) {
		std::vector<char> err(logi + 1);
		glGetShaderInfoLog(frag, logi, NULL, &err[0]);
		std::println("{}", &err[0]);
	}

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &res);
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logi);
	if (logi > 0) {
		std::vector<char> err(logi + 1);
		glGetProgramInfoLog(prog, logi, NULL, &err[0]);
		std::println("{}", &err[0]);
	}

	glDetachShader(prog, vert);
	glDetachShader(prog, frag);

	glDeleteShader(vert);
	glDeleteShader(frag);

	return prog;
}
