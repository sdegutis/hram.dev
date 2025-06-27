#include "shaders.h"
#include <battery/embed.hpp>
#include <print>

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
