#include <string>
#include <print>
#include <vector>

#include "battery/embed.hpp"

#include "canvas.hpp"

GLuint createShaders();

Canvas::Canvas() {

	window = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	glcontext = SDL_GL_CreateContext(window);
	gladLoadGL(SDL_GL_GetProcAddress);

	prog = createShaders();
	glUseProgram(prog);

	resolutionLocation = glGetUniformLocation(prog, "u_resolution");
	imageLocation = glGetUniformLocation(prog, "u_image");

	glClearColor(0.f, 0.f, .4f, 1.f);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	for (int i = 0; i < 320 * 180 * 4; i++) data[i] = SDL_rand(256);
	for (int i = 0; i < 320 * 180 * 4; i += 4) {
		data[i + 0] = SDL_rand(256);
		data[i + 1] = 0;
		data[i + 2] = 0;
		data[i + 3] = 0;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 180, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);






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

}

void Canvas::resized()
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

void Canvas::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	SDL_GL_SwapWindow(window);
}

void Canvas::mouseMoved(int x, int y)
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

void Canvas::iterate()
{
	//Uint64 ticks = SDL_GetTicks();


}



Canvas::~Canvas() {
	glDeleteProgram(prog);
	SDL_GL_DestroyContext(glcontext);
	SDL_DestroyWindow(window);
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
