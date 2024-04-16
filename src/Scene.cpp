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

    // Frame Buffer Progressive Sampling ----------------------------------------------
    // Texture Setup
    glGenTextures(1, &FrameText1);
    glBindTexture(GL_TEXTURE_2D, FrameText1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::vector<GLubyte> data1(WIN_WIDTH * WIN_HEIGHT * 4, 255); // White color (RGBA: 255, 255, 255, 255)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


    glBindTexture(GL_TEXTURE_2D, 0);


    // Frame Buffer
    glGenFramebuffers(1, &prevFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FrameText1, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // WINDOW DISPLAY BUFFERS-------------------------------------------------------
    // Generate Vertex Buffer that holds vertex data
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate Vertex Array that manages vertices and their attributes 
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(float) * 3, (void*)0);     // Tells OpenGL how to read data in Buffer
    glEnableVertexAttribArray(0); // Enables the vertex attributes just created

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // SKYBOX TEXTURE SETUP ---------------------------------------------------
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    float* data = stbi_loadf("Images/skybox15.hdr", &width, &height, &nrChannels, 0);
    if (data)
    {
        //GLenum format = nrChannels == 3 ? GL_RGB : GL_RGB32F;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG_ERROR("Failed to load texture");
    }
    stbi_image_free(data);
}

void Scene::update() {
	processInput();

    auto ct = std::chrono::high_resolution_clock::now();

    auto cd = ct.time_since_epoch();

    elapsedTime = std::chrono::duration<float>(cd).count();

    glClearColor(0.1f, 0.1f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    int uni_iteratorLoc = glGetUniformLocation(shaderProgram, "u_fiter");
    glUniform1i(uni_iteratorLoc, frameIterator);

    float camPosArray[3] = { cam.pos.x, cam.pos.y, cam.pos.z };
    int uni_camPos = glad_glGetUniformLocation(shaderProgram, "u_camPos");
    glUniform3fv(uni_camPos, 1, camPosArray);

    int uni_camPitch = glGetUniformLocation(shaderProgram, "u_camPitch");
    glUniform1f(uni_camPitch, cam.pitch);

    int uni_camYaw = glGetUniformLocation(shaderProgram, "u_camYaw");
    glUniform1f(uni_camYaw, cam.yaw);

    int uni_vpDist = glGetUniformLocation(shaderProgram, "u_vpDist");
    glUniform1f(uni_vpDist, VIEW_PLANE_DIST);

    int uni_frameProgCount = glGetUniformLocation(shaderProgram, "u_frameProgCount");
    glUniform1f(uni_frameProgCount, frameProgCount);

    int uni_screenWidth = glGetUniformLocation(shaderProgram, "u_WIN_WIDTH");
    glUniform1f(uni_screenWidth, WIN_WIDTH);

    int uni_screenHeight = glGetUniformLocation(shaderProgram, "u_WIN_HEIGHT");
    glUniform1f(uni_screenHeight, WIN_HEIGHT);

    int uni_elapsedTime = glGetUniformLocation(shaderProgram, "elapsedTime");
    glUniform1f(uni_elapsedTime, elapsedTime);
    
    // Skybox
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    int skyboxTextureUniLoc = glGetUniformLocation(shaderProgram, "u_skyboxTexture");
    glUniform1i(skyboxTextureUniLoc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, FrameText1);

    int prevFrameTextureUniLoc = glGetUniformLocation(shaderProgram, "u_prevFrameTexture");
    glUniform1i(prevFrameTextureUniLoc, 1);


    // Render Screen
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // TODO Render to default buffer and then blit default buffer to prevFrameBuffer and use it to sample prev frame
    glfwSwapBuffers(window);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevFrameBuffer);
    

    glBlitFramebuffer(
        0, 0, WIN_WIDTH, WIN_HEIGHT,
        0, 0, WIN_WIDTH, WIN_HEIGHT,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, prevFrameBuffer);




    glfwPollEvents();

    if (glfwWindowShouldClose(window) == GLFW_TRUE) {
        glfwTerminate();
        LOG_ERROR("Program Exited Sucessfully");
    }
    

    frameIterator++;
    frameProgCount++;
}

void Scene::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        resetFrameCounter();
		glfwSetWindowShouldClose(window, true);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pos += CAM_SPEED * cam.frontVector;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pos -= CAM_SPEED * cam.rightVector;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pos -= CAM_SPEED * cam.frontVector;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pos += CAM_SPEED * cam.rightVector;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pos += CAM_SPEED * glm::vec3(0, 1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pos -= CAM_SPEED * glm::vec3(0, 1, 0);
    }


    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        resetFrameCounter();
        cam.yaw += -CAM_ROT_SPEED;
        cam.rightVector = glm::rotate(cam.rightVector, -CAM_ROT_SPEED, glm::vec3(0, 1, 0));
        cam.frontVector = glm::rotate(cam.frontVector, -CAM_ROT_SPEED, glm::vec3(0, 1, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        resetFrameCounter();
        cam.yaw += CAM_ROT_SPEED;
        cam.rightVector = glm::rotate(cam.rightVector, CAM_ROT_SPEED, glm::vec3(0, 1, 0));
        cam.frontVector = glm::rotate(cam.frontVector, CAM_ROT_SPEED, glm::vec3(0, 1, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pitch += -CAM_ROT_SPEED;
        cam.frontVector = glm::rotate(cam.frontVector, -CAM_ROT_SPEED, cam.rightVector);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        resetFrameCounter();
        cam.pitch += CAM_ROT_SPEED;
        cam.frontVector = glm::rotate(cam.frontVector, CAM_ROT_SPEED, cam.rightVector);
    }


}



// Functions to make code cleaner

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


