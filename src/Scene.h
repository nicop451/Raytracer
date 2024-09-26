#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <ErrorUtility.h>
#include <DefaultSettings.h>
#include <stb_image.h>
#include <ShaderCreation.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

struct Camera {
	float viewPlaneDist = 60;
	float pitch = 0.1;
	float yaw = 0;
	glm::vec3 frontVector = glm::vec3(0, 0, 1);
	glm::vec3 rightVector = glm::vec3(1, 0, 0);
	glm::vec3 pos = glm::vec3(0, -2, -1500);
};

struct TextureData {
	float* pixelData;
	int width = 0;
	int height = 0;
};

class Scene {
public:
	// Window Data
	GLFWwindow* window;
	Camera cam;

	int frameIterator = 0;
	float elapsedTime;

	int skyboxSelectionNum = 0;
	TextureData skyboxTextureDataArr[10];

	Scene();	
	void processInput();
	void update();

private:

	// Window setup
	unsigned int vertexBufferObject, vertexArrayObject;
	unsigned int shaderProgram;
	unsigned int skyboxTexture;
	unsigned int prevFrameBuffer;
	unsigned int FrameText1;
	unsigned int FrameText2;
	int frameProgCount = 0;
	bool swapStateVar;


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
	void resetFrameCounter() { frameIterator = 0; }
};