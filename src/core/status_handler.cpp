#include "core/status_handler.hpp"
#include <iostream>

void StatusHandler::log(LogStatus status, const char* status_log)
{
    switch (status)
    {
        case ERROR:
            std::cerr << "ERROR: " << status_log << '.' << std::endl;
            break;
        case WARNING:
            std::cout << "WARNING: " << status_log << '.' <<  std::endl;
            break;
        case SUCCESS:
            std::cout << "SUCCESS: " << status_log << '.' <<  std::endl;
            break;
    }
}

void StatusHandler::log(LogStatus status, const std::wstring& status_log)
{
    switch (status)
    {
        case ERROR:
            std::wcerr << "ERROR: " << status_log << '.' << std::endl;
            break;
        case WARNING:
            std::wcout << "WARNING: " << status_log << '.' <<  std::endl;
            break;
        case SUCCESS:
            std::wcout << "SUCCESS: " << status_log << '.' <<  std::endl;
            break;
    }
}
