#pragma once

#include <iostream>

void log_error(long linenum, const char* filename, std::string msg);

#define LOG_ERROR(msg) log_error(__LINE__, __FILE__, msg);