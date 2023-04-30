#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Fusion
{
	class Renderer
	{
	public:
		void CreateTriangle();
		void CreateSquare();
		void CreateCube();
		void CreateFrameBuffer();
		void BindFrameBuffer();
		void UnbindFrameBuffer();
		void RescaleFrameBuffer(int width, int height);
	public:
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		GLuint cubeVAO;
		GLuint cubeVBO;
		GLuint cubeEBO;
		GLuint sVAO;
		GLuint sVBO;
		GLuint tEBO;
		GLuint FBO;
		GLuint RBO;
		GLuint TID; // texture id
	};
};