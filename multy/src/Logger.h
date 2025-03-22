#pragma once
#include <iostream>
#include <string>


class Logger
{
public:
    Logger(const std::string& className)
        : className_(className)
    {
        std::cout << "Constructing " << className_ << std::endl;
    }

    ~Logger()
    {
        std::cout << "Destructing " << className_ << std::endl;
    }

private:
    std::string className_;
};