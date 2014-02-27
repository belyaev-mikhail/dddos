#include <array>
#include <memory>

#include <Theron/Theron.h>

#include "Util/Json.hpp"

namespace callophrys  {
namespace remote {

class RestActorRefBase : public Theron::Actor {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    explicit RestActorRefBase(Theron::Framework &framework, const std::string& uri);
    ~RestActorRefBase();
protected:
    void Handler(const util::JsonValue& message, const Theron::Address);
};

class RestActorImplBase : public Theron::Actor {
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    explicit RestActorImplBase(Theron::Framework &framework, uint16_t port);
    ~RestActorImplBase();
private:
    void Handler(const util::JsonValue& message, const Theron::Address);
};

template<class ...PossibleArgs>
class RestActorRef : public RestActorRefBase {
public:
    explicit RestActorRef(Theron::Framework &framework, const std::string& uri):
        RestActorRefBase{framework, uri}{
        std::array<bool, sizeof...(PossibleArgs)> rets {
            { RegisterHandler(this, &RestActorRef::ConcreteHandler<PossibleArgs>) ... }
        };
    };

private:
    template<class T>
    void ConcreteHandler(const T& value, const Theron::Address addr) {
        Handler(util::toJson(value), addr);
    }
};

} /* namespace remote */
} /* namespace callophrys */
