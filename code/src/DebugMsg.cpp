#include "DebugMsg.h"

#include <iostream>
#include <string>



Messenger::Messenger() {}



void Messenger::message(msgType header, std::string message) {
    std::string headerStr = "";

    switch (header) {
        case Note:
            headerStr = "NOTE";
            break;
        case Warning:
            headerStr = "WARNING";
            break;
        case Error:
            headerStr = "ERROR";
            break;
        case Fatal:
            headerStr = "FATAL";
            break;
        case Debug:
        default:
            headerStr = "DEBUG";
            break;
    }

    std::cout << "[" << headerStr << "] -- " << message << std::endl;
}