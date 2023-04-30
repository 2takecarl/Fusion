#include "Application.h"
#include "Shader.h"
#include <iostream>
#include "GameObject.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Camera.h"
#include <conio.h>

namespace Fusion
{
	const GLint WIDTH = 1920;
	const GLint HEIGHT = 1080;
	int width = WIDTH;
	int height = HEIGHT;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = WIDTH / 2.0f;
	float lastY = HEIGHT / 2.0f;
	bool firstMouse = true;
	bool rightMouse = false;

	bool sceneHovered = false;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	std::vector<GameObject::Object> obj;

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);
	void addObject(glm::vec3 pos, int objectIndex);

	int Application::initialize()
	{
		if (!glfwInit())
			return -1;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "test", nullptr, nullptr);
		if (window == nullptr) {
			std::cout << "failed to create window" << std::endl;
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		int bufferWidth, bufferHeight;
		glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}

		glViewport(0, 0, bufferWidth, bufferHeight);

		glEnable(GL_DEPTH_TEST);

		return 0;
	}
	int Application::run()
	{
		if (initialize() != 0)
			return -1;
		
		//INITIALIZE IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
		renderer.CreateTriangle();
		renderer.CreateSquare();
		renderer.CreateCube();
		Shader shader("C:\\dev\\Fusion\\Fusion\\src\\shaders\\shader.vert", "C:\\dev\\Fusion\\Fusion\\src\\shaders\\shader.frag");
		renderer.CreateFrameBuffer();
		glm::mat4 model = glm::mat4(1.0f);

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();

			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			processInput(window);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();

			ImGui::NewFrame();
			ImGui::DockSpaceOverViewport();
			ImGui::Begin("Game");
			ImGui::End();
			ImGui::Begin("Scene");
			if(!rightMouse)
				sceneHovered = ImGui::IsWindowHovered();
			const float window_width = ImGui::GetContentRegionAvail().x;
			const float window_height = ImGui::GetContentRegionAvail().y;

			renderer.RescaleFrameBuffer(window_width, window_height);
			glViewport(0, 0, window_width, window_height);

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddImage(
				(void*)renderer.TID,
				ImVec2(pos.x, pos.y),
				ImVec2(pos.x + window_width, pos.y + window_height),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			ImGui::End();
			ImGui::Begin("Hierarchy");

			if (ImGui::BeginMenu("Spawn Object"))
			{
				if (ImGui::MenuItem("Triangle"))
					obj.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 0, false, "Triangle"});

				if (ImGui::MenuItem("Square"))
					obj.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 1, false, "Square"});

				if (ImGui::MenuItem("Cube"))
					obj.push_back({ glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 2, false, "Cube" });
				
				ImGui::EndMenu();
			}

			for (int i = 0; i < obj.size(); i++)
			{
				ImGui::PushID(i);
				if (ImGui::Selectable(obj[i].name, &obj[i].selected))
				{
					for (int i = 0; i < obj.size(); i++) 
						obj[i].selected = false;
					obj[i].selected = true;
				}
				ImGui::PopID();
			}
			ImGui::End();
			ImGui::Begin("Inspector");
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].selected)
				{
					ImGui::Separator();
					ImGui::Text("Transform");
					ImGui::Separator();

						ImGui::PushID(1);
						ImGui::Text("Position");
						ImGui::DragFloat3("", &obj[i].Position.x, 0.01f);
						ImGui::PopID();

						ImGui::PushID(2);
						ImGui::Text("Rotation");
						ImGui::DragFloat3("", &obj[i].Rotation.x, 0.1f);
						ImGui::PopID();

						ImGui::PushID(3);
						ImGui::Text("Scale");
						ImGui::DragFloat3("", &obj[i].Scale.x, 0.01f);
						ImGui::PopID();
					
					ImGui::Separator();
					ImGui::Text("Name");
					ImGui::Separator();

						ImGui::LabelText("", obj[i].name);
				}
			}
			ImGui::End();
			ImGui::Begin("Console");
			ImGui::End();
			ImGui::Begin("Project");
			ImGui::End();
			ImGui::Render();

			renderer.BindFrameBuffer();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.use();

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			shader.setMat4("projection", projection);

			glm::mat4 view = camera.GetViewMatrix();
			shader.setMat4("view", view);

			for(int i = 0; i < obj.size(); i++)
			{
				glm::vec3 pos = obj[i].Position;
				glm::vec3 rot = obj[i].Rotation;
				glm::vec3 scl = obj[i].Scale;
				model = glm::mat4(1.0f);
				if (obj[i].objIndex == 0)
				{
					model = glm::translate(model, glm::vec3(pos));
					model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
					model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
					model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
					model = glm::scale(model, glm::vec3(scl));
					shader.setMat4("model", model);
					glBindVertexArray(renderer.VAO);
					glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
				}
				if (obj[i].objIndex == 1)
				{
					model = glm::translate(model, glm::vec3(pos));
					model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
					model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
					model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
					model = glm::scale(model, glm::vec3(scl));
					shader.setMat4("model", model);
					glBindVertexArray(renderer.sVAO);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}
				if (obj[i].objIndex == 2)
				{
					model = glm::translate(model, glm::vec3(pos));
					model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1, 0, 0));
					model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0, 1, 0));
					model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0, 0, 1));
					model = glm::scale(model, glm::vec3(scl));
					shader.setMat4("model", model);
					glBindVertexArray(renderer.cubeVAO);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				}
			}

			glBindVertexArray(0);
			glUseProgram(0);
			renderer.UnbindFrameBuffer();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}
			glfwSwapBuffers(window);
		}

		glfwDestroyWindow(window);
		glfwTerminate();

		return 0;
	}
	void addObject(glm::vec3 pos, int objectIndex)
	{
	}
	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (rightMouse && sceneHovered)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProcessKeyboard(FORWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProcessKeyboard(LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProcessKeyboard(RIGHT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				camera.ProcessKeyboard(UP, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				camera.ProcessKeyboard(DOWN, deltaTime);
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			rightMouse = true;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
			rightMouse = false;
	}
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;
		if(rightMouse && sceneHovered)
			camera.ProcessMouseMovement(xoffset, yoffset);
	}
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
	}
}