#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <ErrorUtility.h>
#include <DefaultSettings.h>
#include <ShaderCreation.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <iostream>
#include <cmath>
#include <vector>

struct Camera {
	float viewPlaneDist = 60;
	float pitch = 0;
	float yaw = 0;
	glm::vec3 frontVector = glm::vec3(0, 0, 1);
	glm::vec3 rightVector = glm::vec3(1, 0, 0);
	glm::vec3 pos = glm::vec3(0, 0, -200);
};

class Scene {
public:
	// Window Data
	GLFWwindow* window;
	int frameIterator = 0;
	Camera cam;

	Scene();	
	void processInput();
	void update();

private:

	// Window setup
	unsigned int vertexBufferObject, vertexArrayObject;
	unsigned int shaderProgram;

	float vertices[18] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};

	// Functions to keep code clean
	void createWindow();
	void loadOpenGLFuncs();
};