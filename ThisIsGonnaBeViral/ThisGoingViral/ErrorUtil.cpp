#include "ErrorUtil.h"

void log_error(long lineNum, const char* fileName, std::string customMessage = " ") {
	std::cerr << "ERROR at line " << lineNum << " in " << fileName << ": " << customMessage << std::endl;
	std::exit(EXIT_FAILURE);
}
