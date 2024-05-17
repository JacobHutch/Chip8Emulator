#pragma once

#include <string>



class Messenger {
    public:
    Messenger();

    enum msgType { Debug, Note, Warning, Error, Fatal };

    static void message(msgType header, std::string message);
};