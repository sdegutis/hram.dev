#include <epoxy/gl.h>

#include <string>
#include <vector>

#include "canvas.hpp"

GLuint createShaders();


// Read the Vertex Shader code from the file
std::string VertexShaderCode = R"(#version 300 es
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

// Read the Fragment Shader code from the file
std::string FragmentShaderCode = R"(#version 300 es
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

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);




	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	glActiveTexture(GL_TEXTURE0 + 0);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

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

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);




	// look up where the vertex data needs to go.
	auto positionAttributeLocation = glGetAttribLocation(prog, "a_position");
	auto texCoordAttributeLocation = glGetAttribLocation(prog, "a_texCoord");

	resolutionLocation = glGetUniformLocation(prog, "u_resolution");
	imageLocation = glGetUniformLocation(prog, "u_image");

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	positionBuffer;
	glGenBuffers(1, &positionBuffer);

	glEnableVertexAttribArray(positionAttributeLocation);

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);

	glVertexAttribPointer(positionAttributeLocation, 2, GL_FLOAT, false, 0, 0);

	GLuint texCoordBuffer;
	glGenBuffers(1, &texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	float coordinates[12] = { 0,0,1,0,0,1,0,1,1,0,1,1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(coordinates), coordinates, GL_STATIC_DRAW);

	glEnableVertexAttribArray(texCoordAttributeLocation);

	glVertexAttribPointer(texCoordAttributeLocation, 2, GL_FLOAT, false, 0, 0);

	draw();

}

void Canvas::draw()
{


	glViewport(0, 0, 320, 180);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	glUseProgram(prog);

	glBindVertexArray(vao);

	glUniform2f(resolutionLocation, 320, 180);
	glUniform1i(imageLocation, 0);

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);

	float x1 = 100;
	float x2 = 120;
	float y1 = 100;
	float y2 = 120;
	float xys[12] = { x1,y1,x2,y1,x1,y2,x1,y2,x2,y1,x2,y2 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(xys), xys, GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, 6);




	SDL_GL_SwapWindow(window);

}



Canvas::~Canvas() {
	glDeleteProgram(prog);
	SDL_GL_DestroyContext(glcontext);
	SDL_DestroyWindow(window);
}

GLuint createShaders() {


	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

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
	auto program = ProgramID;
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

	return ProgramID;
}
