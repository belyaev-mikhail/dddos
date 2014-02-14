#include <memory>

#include <Theron/Theron.h>

namespace ignis {

struct JsonObject {
    std::string to_string() const { return "{}"; };
};

namespace remote {

class RestEndpoint{
    uint16_t port;
};

class RestActorRef : public Theron::Actor {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    explicit RestActorRef(Theron::Framework &framework, const std::string& uri);
    ~RestActorRef();
private:
    void Handler(const JsonObject& message, const Theron::Address);
};

class RestActorImpl : public Theron::Actor {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    explicit RestActorImpl(Theron::Framework &framework, uint16_t port);
    ~RestActorImpl();
private:
    void Handler(const JsonObject& message, const Theron::Address);
};

} /* namespace remote */
} /* namespace ignis */
