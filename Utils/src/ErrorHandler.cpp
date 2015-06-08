#include "ErrorHandler.h"
#include "common.h"
#include "Utils.h"
#include <stdio.h>
#include <stdexcept>
#include <sstream>

#define MAX_MESSAGE_SIZE 4096
ErrorHandler::ErrorHandler() {
}

void ErrorHandler::Process(ERROR_TYPE errorType, char* component, char* file, int line, wchar_t* format, ...) {
	component;
	errorType;

    wchar_t message[MAX_MESSAGE_SIZE];
    std::wstringstream messageStream;
    va_list args;
    va_start(args, format);
    vswprintf_s(message, MAX_MESSAGE_SIZE, format, args);
    messageStream << "(" << file << ": " << line << ") " << message;
    ::MessageBoxW(NULL, messageStream.str().c_str(), L"Error", NULL);
    va_end(args);
	throw std::exception("");
}

ErrorHandler& ErrorHandler::Instance() 
{
    return Singleton<ErrorHandler>();
}