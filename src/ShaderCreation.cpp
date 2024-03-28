#include <ShaderCreation.h>

unsigned int nlib::CreateSingleShader(unsigned int type, std::string& srcPath) {
    // Converting Source string to const char*
    std::string src = nlib::fileReading::shaderSource(srcPath);
    const char* src_const = src.c_str();
    // Create the shader, add the source code, and compile
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src_const, nullptr);
    glCompileShader(shader);
    // Error handling
    int statusResult;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &statusResult);
    if (statusResult == GL_FALSE) {
        // Retrieve error info
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, message);
        // Print Error
        std::string leadingErrorMessage = type == GL_VERTEX_SHADER ? "vertex shader: " : "fragment shader: ";
        LOG_ERROR("Failed to compile " + leadingErrorMessage + message);
    }

    return shader;
}

unsigned int nlib::CreateShaderProgram(std::string& fragSrcPath, std::string& vertSrcPath) {
    // Create Shaders to attach to program
    unsigned int fragShader = nlib::CreateSingleShader(GL_FRAGMENT_SHADER, fragSrcPath);
    unsigned int vertShader = nlib::CreateSingleShader(GL_VERTEX_SHADER, vertSrcPath);
    // Create shader program and attach shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragShader);
    glAttachShader(shaderProgram, vertShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    // Clean up
    glDeleteShader(fragShader);
    glDeleteShader(vertShader);

    return shaderProgram;
}

