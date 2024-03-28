#pragma once
#include <glad\glad.h>
#include <ErrorUtility.h>    
#include <FileUtil.h>
#include <iostream>
#include <string>
#include <cmath>

namespace nlib {
    unsigned int CreateSingleShader(unsigned int type, std::string& srcPath);
    unsigned int CreateShaderProgram(std::string& fragSrcPath, std::string& vertSrcPath);
}