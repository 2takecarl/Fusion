#pragma once
#include "Renderer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Fusion
{
	class Application
	{
	public:
		int initialize();
		int run();
	private:
		GLFWwindow* window;
		Renderer renderer;
	};
}