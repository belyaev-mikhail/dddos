#ifndef LOGGER_H
#define LOGGER_H

#include <Theron/Theron.h>

namespace ignis {

struct LogRecord {
    enum class Priotity { DBG, INFO, WARN, ERROR };

    Priotity priority;
    std::string message;
};

template<class Actor>
class LoggingActor {
    Theron::Address loggerAddress;

    Actor* self() {
        return static_cast<Actor*>(this);
    }
    const Actor* self() const {
        return static_cast<const Actor*>(this);
    }

protected:
    LoggingActor(Theron::Address loggerAddress) : loggerAddress{loggerAddress} {};
    void log(const std::string& message, LogRecord::Priotity priority = LogRecord::Priotity::INFO) const {
        self() -> send( LogRecord{ priority, message }, loggerAddress );
    }
};

class Logger : public Theron::Actor
{
public:

    explicit Logger(Theron::Framework &framework);

private:
    void RecordHandler(const LogRecord& message, const Theron::Address);
    void StringHandler(const std::string& message, const Theron::Address);
};

}

#endif /* LOGGER_H */