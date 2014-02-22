#include <memory>

#include <Theron/Theron.h>

#include "Util/Json.hpp"
<<<<<<< local

namespace ignis {
=======
>>>>>>> other

<<<<<<< local
=======
namespace ignis  {
>>>>>>> other
namespace remote {

class RestActorRef : public Theron::Actor {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    explicit RestActorRef(Theron::Framework &framework, const std::string& uri);
    ~RestActorRef();
private:
    void Handler(const util::JsonValue& message, const Theron::Address);
};

class RestActorImpl : public Theron::Actor {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    explicit RestActorImpl(Theron::Framework &framework, uint16_t port);
    ~RestActorImpl();
private:
    void Handler(const util::JsonValue& message, const Theron::Address);
};

} /* namespace remote */
} /* namespace ignis */
