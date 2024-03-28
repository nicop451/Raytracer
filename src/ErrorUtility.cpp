#include <ErrorUtility.h>

void log_error(long lineNum, const char* fileName, std::string customMessage = " ") {
	std::cerr << "ERROR at line " << lineNum << " in " << fileName << ": " << customMessage << std::endl;
	glfwTerminate();
	std::exit(EXIT_FAILURE);
}

