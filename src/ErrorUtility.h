#pragma once

#include <iostream>
#include <sstream>
#include <GLFW\glfw3.h>	
#include <fstream>

void log_error(long lineNum, const char* fileName, std::string customMessage);

#define LOG_ERROR(msg) log_error(__LINE__, __FILE__, msg);