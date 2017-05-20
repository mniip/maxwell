#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "config.h"
#include "physics.h"

void fail(char const *format, ...)
{
	va_list list;
	va_start(list, format);
	vfprintf(stderr, format, list);
	va_end(list);
	exit(EXIT_FAILURE);
}

void failProgram(GLuint program, char const *format, ...)
{
	int max_size = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_size);
	char *buffer = malloc(max_size);
	int size;
	glGetProgramInfoLog(program, max_size, &size, buffer);

	va_list list;
	va_start(list, format);
	vfprintf(stderr, format, list);
	va_end(list);
	fprintf(stderr, "%.*s\n", size, buffer);
	exit(EXIT_FAILURE);
}

void failShader(GLuint shader, char const *format, ...)
{
	int max_size = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_size);
	char *buffer = malloc(max_size);
	int size;
	glGetShaderInfoLog(shader, max_size, &size, buffer);

	va_list list;
	va_start(list, format);
	vfprintf(stderr, format, list);
	va_end(list);
	fprintf(stderr, "%.*s\n", size, buffer);
	exit(EXIT_FAILURE);
}

GLuint compileShader(GLenum type, char const *source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint ok = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if(ok != GL_TRUE)
		failShader(shader, "Shader failed\n");
	return shader;
}

extern char vertex_glsl[];
extern char fragment_glsl[];

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		fail("SDL init failed: %s\n", SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	SDL_Window *window = SDL_CreateWindow(argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if(!window)
		fail("Window creation failed: %s\n", SDL_GetError());

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(!context)
		fail("GL context creation failed: %s\n", SDL_GetError());

	glewExperimental = GL_TRUE;

	GLenum glError;
	if((glError = glewInit()) != GLEW_OK)
		fail("GLEW init failed: %s\n", glewGetErrorString(glError));

	GLuint programID = glCreateProgram();

	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertex_glsl);
	glAttachShader(programID, vertexShader);

	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragment_glsl);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	GLint ok = GL_TRUE;
	glGetProgramiv(programID, GL_LINK_STATUS, &ok);
	if(ok != GL_TRUE)
		failProgram(programID, "Program link failed\n");

	GLint vertexCoord = glGetAttribLocation(programID, "vertexCoord");
	GLint texCoord = glGetAttribLocation(programID, "inTexCoord");
	GLint rotation = glGetAttribLocation(programID, "rotation");
	GLint pos = glGetAttribLocation(programID, "pos");

	GLuint VAO, VBO, IBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glClearColor(0.0, 0, 0, 1);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	const int xres = 10, yres = 10, zres = 1000;

	aether *field[2] = {calloc(xres * yres * zres, sizeof(aether)), calloc(xres * yres * zres, sizeof(aether))};
	int buf = 0;

	int done = 0;
	int fr = 0;
	int i, j, k;
	while(!done)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
	
		//xr += 0.008;
		glVertexAttrib3f(rotation, 3.141592/8., 1.3 + fr / 10000.0, 0.0);

		GLfloat (*vertices)[4] = NULL;
		size_t num_vertices = 0;
		GLuint (*indices)[3] = NULL;
		size_t num_indices = 0;

		for(i = 0; i < xres; i++)
			for(j = 0; j < yres; j++)
				for(k = 0; k < zres; k++)
				if(j==yres/2)
				{
					double cx = (i - xres / 2.0) / pow(xres * yres * zres, 0.33);
					double cy = (j - yres / 2.0) / pow(xres * yres * zres, 0.33);
					double cz = (k - zres / 2.0) / pow(xres * yres * zres, 0.33);
					aether *part = &field[buf][(i * yres + j) * zres + k];
					if(hypot(part->E[0], hypot(part->E[1], part->E[2])) > 1.0e-9)
					{
						vertices = realloc(vertices, (num_vertices + 3) * 4 * sizeof(GLfloat));
						vertices[num_vertices][0] = cx - 0.005;
						vertices[num_vertices][1] = cy - 0.005;
						vertices[num_vertices][2] = cz - 0.005;
						vertices[num_vertices][3] = 1.0;
						vertices[num_vertices + 1][0] = cx + 0.005;
						vertices[num_vertices + 1][1] = cy + 0.005;
						vertices[num_vertices + 1][2] = cz + 0.005;
						vertices[num_vertices + 1][3] = 1.0;
						vertices[num_vertices + 2][0] = cx - part->E[0] * 1.0e6;
						vertices[num_vertices + 2][1] = cy - part->E[1] * 1.0e6;
						vertices[num_vertices + 2][2] = cz - part->E[2] * 1.0e6;
						vertices[num_vertices + 2][3] = 2.0;

						indices = realloc(indices, (num_indices + 1) * 3 * sizeof(GLuint));
						indices[num_indices][0] = num_vertices;
						indices[num_indices][1] = num_vertices + 1;
						indices[num_indices][2] = num_vertices + 2;
						num_indices++;

						num_vertices += 3;
					}

					if(hypot(part->B[0], hypot(part->B[1], part->B[2])) > 1.0e-9 * sqrt(MU_0 * EPS_0))
					{
						vertices = realloc(vertices, (num_vertices + 3) * 4 * sizeof(GLfloat));
						vertices[num_vertices][0] = cx - 0.005;
						vertices[num_vertices][1] = cy - 0.005;
						vertices[num_vertices][2] = cz - 0.005;
						vertices[num_vertices][3] = 3.0;
						vertices[num_vertices + 1][0] = cx + 0.005;
						vertices[num_vertices + 1][1] = cy + 0.005;
						vertices[num_vertices + 1][2] = cz + 0.005;
						vertices[num_vertices + 1][3] = 3.0;
						vertices[num_vertices + 2][0] = cx - part->B[0] * 1.0e6 / sqrt(MU_0 * EPS_0);
						vertices[num_vertices + 2][1] = cy - part->B[1] * 1.0e6 / sqrt(MU_0 * EPS_0);
						vertices[num_vertices + 2][2] = cz - part->B[2] * 1.0e6 / sqrt(MU_0 * EPS_0);
						vertices[num_vertices + 2][3] = 4.0;

						indices = realloc(indices, (num_indices + 1) * 3 * sizeof(GLuint));
						indices[num_indices][0] = num_vertices;
						indices[num_indices][1] = num_vertices + 1;
						indices[num_indices][2] = num_vertices + 2;
						num_indices++;

						num_vertices += 3;
					}
				}
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, num_vertices * 4 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexCoord);
		glVertexAttribPointer(vertexCoord, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
		glEnableVertexAttribArray(texCoord);
		glVertexAttribPointer(texCoord, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL + (3 * sizeof(GLfloat)));
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * 3 * sizeof(GLuint), indices, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, num_indices * 3, GL_UNSIGNED_INT, NULL);

		glUseProgram(0);

		free(vertices);
		free(indices);

		SDL_GL_SwapWindow(window);

		for(int s = 0; s < 10; s++)
		{
			fr++;
			update_aether(field[!buf], field[buf], xres, yres, zres, 1.0e-2, 3.336e-13);
			buf = !buf;

			for(i = 0; i < xres; i++)
				for(j = 0; j < yres; j++)
					for(k = 0; k < zres; k++)
						if(i < 2 || j < 2 || k < 2)
						{
							aether *part = &field[buf][(i * yres + j) * zres + k];
							part->E[0] *= 0.8;
							part->E[1] *= 0.8;
							part->E[2] *= 0.8;
							part->B[0] *= 0.8;
							part->B[1] *= 0.8;
							part->B[2] *= 0.8;
						}

			for(int i = 0; i < xres; i++)
				for(int j = 0; j < yres; j++)
				{
					field[buf][(i * yres + j) * zres + zres/2].E[1] = sin(fr / 200.0) * 1e-7;
					field[buf][(i * yres + j) * zres + zres/2 - 1].E[1] = sin(fr / 200.0) * 0.5e-7;
					field[buf][(i * yres + j) * zres + zres/2 + 1].E[1] = sin(fr / 200.0) * 0.5e-7;
				}
		}
/*
		double energy = 0.0;
		for(i = 0; i < xres; i++)
			for(j = 0; j < yres; j++)
				for(k = 0; k < zres; k++)
				{
					aether *part = &field[buf][(i * yres + j) * zres + k];
					energy += 0.5 * EPS_0 * (pow(part->E[0], 2.0) + pow(part->E[1], 2.0) + pow(part->E[1], 2.0));
					energy += 0.5 / MU_0 * (pow(part->B[0], 2.0) + pow(part->B[1], 2.0) + pow(part->B[1], 2.0));
				}
		printf("%g\n", energy);
*/
		
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0)
		{
			if(e.type == SDL_QUIT)
				done = 1;
			if(e.type == SDL_KEYDOWN)
				if(e.key.keysym.sym == '.')
				{
					int i, j, k;
					for(i = 0; i < xres; i++)
						for(j = 0; j < yres; j++)
							for(k = 0; k < zres; k++)
							{
								field[buf][(i * yres + j) * zres + k].B[0] = 0.0;
								field[buf][(i * yres + j) * zres + k].E[1] = pow(2.467-pow(atan(hypot(i-xres/2, hypot(j-yres/2, k-zres/2))), 2.0), 4.0) * 1e-8;
								field[buf][(i * yres + j) * zres + k].E[2] = 0.0;
								field[buf][(i * yres + j) * zres + k].B[0] = 0.0;
								field[buf][(i * yres + j) * zres + k].B[1] = 0.0;
								field[buf][(i * yres + j) * zres + k].B[2] = 0.0;
							}
				}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
