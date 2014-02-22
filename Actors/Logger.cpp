#include "Actors/Logger.h"

#include <iostream>

namespace callophrys {

Logger::Logger(Theron::Framework &framework) : Theron::Actor(framework)
{
    RegisterHandler(this, &Logger::RecordHandler);
    RegisterHandler(this, &Logger::StringHandler);
}

void Logger::RecordHandler(const LogRecord& message, const Theron::Address)
{
    std::cerr << message.message << std::endl;
}

void Logger::StringHandler(const std::string& message, const Theron::Address)
{
    std::cerr << message << std::endl;
}

} /* namespace callophrys */ 
