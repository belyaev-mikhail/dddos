#include "Actors/Logger.h"

namespace ignis {

Logger::Logger(Theron::Framework &framework) : Theron::Actor(framework)
{
    RegisterHandler(this, &Logger::RecordHandler);
    RegisterHandler(this, &Logger::StringHandler);
}

void RecordHandler(const LogRecord& message, const Theron::Address)
{
    std::cerr << message.message << std::endl;
}

void StringHandler(const std::string& message, const Theron::Address)
{
    std::cerr << message << std::endl;
}

} /* namespace ignis */ 
