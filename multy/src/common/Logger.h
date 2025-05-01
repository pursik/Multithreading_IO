#pragma once
#include <iostream>
#include <string>

/**
 * @class Logger
 * @brief A simple logger class that logs the construction and destruction of objects.
 */
class Logger
{
public:
    /**
     * @brief Constructs a Logger object.
     * @param className The name of the class to be logged.
     */
    Logger(const std::string& className)
        : className_(className)
    {
        std::cout << "Constructing " << className_ << std::endl;
    }

    /**
     * @brief Destructs the Logger object.
     */
    ~Logger()
    {
        std::cout << "Destructing " << className_ << std::endl;
    }

private:
    std::string className_; ///< The name of the class being logged
};