#include <Scene.h>

Scene::Scene() {
    createWindow();
    loadOpenGLFuncs();
    
    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.13f, 1.0f);

    // Shader Creation and Use
    std::string fragSrcPath = "src/shaders/FragmentShader.frag";
    std::string vertSrcPath = "src/shaders/VertexShader.vert";
    shaderProgram = nlib::CreateShaderProgram(fragSrcPath, vertSrcPath);
    glUseProgram(shaderProgram);

    // WINDOW DISPLAY BUFFERS
    // Generate Vertex Buffer that holds vertex data
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate Vertex Array that manages vertices and their attributes 
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);     // Tells OpenGL how to read data in Buffer
    glEnableVertexAttribArray(0); // Enables the vertex attributes just created

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Scene::update() {
    frameIterator++;
	processInput();

    glClearColor(0.1f, 0.1f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArrayObject);

    int uni_iteratorLoc = glGetUniformLocation(shaderProgram, "u_fiter");
    glUniform1i(uni_iteratorLoc, frameIterator);

    int uni_camPosX = glGetUniformLocation(shaderProgram, "u_camPos");

    float camPosArray[3] = { cam.pos.x, cam.pos.y, cam.pos.z };
    glUniform3fv(uni_camPosX, 1, camPosArray);

    int uni_camPitch = glGetUniformLocation(shaderProgram, "u_camPitch");
    glUniform1f(uni_camPitch, cam.pitch);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);

    glfwPollEvents();

    if (glfwWindowShouldClose(window) == GLFW_TRUE) {
        glfwTerminate();
        LOG_ERROR("Program Exited Sucessfully");
    }
}

void Scene::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.pos.z += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.pos.x -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.pos.z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.pos.x += 0.1;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cam.pitch += 0.01;
        cam.frontVector = glm::rotate(cam.frontVector, 0.01f, cam.rightVector);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cam.pitch -= 0.01;
        cam.frontVector = glm::rotate(cam.frontVector, -0.01f, cam.rightVector);
    }

}

void Scene::createWindow() {
    // Create window using glfw
    if (!glfwInit()) {
        LOG_ERROR("Couldn't initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "OpenGL BluePrint", NULL, NULL);
    if (!window) {
        LOG_ERROR("Couldn't create GLFW window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
}

void Scene::loadOpenGLFuncs() {
    // Loads OpenGL functions using glad
    gladLoadGL();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
    }
}


