#include <FileUtil.h>

std::string nlib::fileReading::shaderSource(std::string& shaderSourcePath)
{
    std::string sourceCode;
    // Open shader source code
    std::fstream file;
    file.open(shaderSourcePath);
    if (!file.is_open()) {
        LOG_ERROR("couldn't open shader source file: " + shaderSourcePath);
    }
    // Add each line to shader source code string
    std::string line;
    while (std::getline(file, line)) {
       sourceCode += line + "\n";
    }
    file.close();
    return sourceCode;
}

void nlib::fileReading::objData() {

}