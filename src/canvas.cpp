#include <string>
#include <print>
#include <vector>

#include "canvas.hpp"

GLuint createShaders();

std::string vertShader = R"(#version 300 es

// an attribute is an input (in) to a vertex shader.
// It will receive data from a buffer
in vec2 a_position;
in vec2 a_texCoord;

// Used to pass in the resolution of the canvas
uniform vec2 u_resolution;

// Used to pass the texture coordinates to the fragment shader
out vec2 v_texCoord;

// all shaders have a main function
void main() {

  // convert the position from pixels to 0.0 to 1.0
  vec2 zeroToOne = a_position / u_resolution;

  // convert from 0->1 to 0->2
  vec2 zeroToTwo = zeroToOne * 2.0;

  // convert from 0->2 to -1->+1 (clipspace)
  vec2 clipSpace = zeroToTwo - 1.0;

  gl_Position = vec4(clipSpace * vec2(1, -1), 0, 1);

  // pass the texCoord to the fragment shader
  // The GPU will interpolate this value between points.
  v_texCoord = a_texCoord;
}

)";

std::string fragShader = R"(#version 300 es

// fragment shaders don't have a default precision so we need
// to pick one. highp is a good default. It means "high precision"
precision highp float;

// our texture
uniform sampler2D u_image;

// the texCoords passed in from the vertex shader.
in vec2 v_texCoord;

// we need to declare an output for the fragment shader
out vec4 outColor;

void main() {
  outColor = texture(u_image, v_texCoord);
}

)";


Canvas::Canvas() {

	window = SDL_CreateWindow("H-RAM", 320 * 3, 180 * 3, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_SetWindowMinimumSize(window, 320, 180);

	glcontext = SDL_GL_CreateContext(window);

	prog = createShaders();
	resolutionLocation = glGetUniformLocation(prog, "u_resolution");
	imageLocation = glGetUniformLocation(prog, "u_image");

	glClearColor(0, 0, .4, 1);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	unsigned char data[2 * 4 * 4] = {
		255, 0, 0, 255,
		255, 0, 255, 255,
		255, 255, 0, 255,
		255, 0, 255, 255,
		255, 255, 0, 255,
		255, 0, 255, 255,
		255, 0, 255, 255,
		255, 255, 0, 255,
	};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);






	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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
	glViewport(0, 0, w, h);

	while (destrect.w + srcrect.w <= w && destrect.h + srcrect.h <= h) {
		scale++;
		destrect.w += srcrect.w;
		destrect.h += srcrect.h;
	}

	destrect.x = w / 2 - destrect.w / 2;
	destrect.y = h / 2 - destrect.h / 2;

	draw();
}

void Canvas::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(prog);

	glBindVertexArray(vao);

	glUniform2f(resolutionLocation, 320, 180);
	glUniform1i(imageLocation, 0);

	glBindBuffer(GL_ARRAY_BUFFER, posBuf);

	float x1 = 100;
	float x2 = 120;
	float y1 = 100;
	float y2 = 120;
	float xys[12] = { x1,y1,x2,y1,x1,y2,x1,y2,x2,y1,x2,y2 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(xys), xys, GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, 6);




	SDL_GL_SwapWindow(window);

}

void Canvas::mouseMoved(int x, int y)
{
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
