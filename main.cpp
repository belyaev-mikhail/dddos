#include <Theron/Theron.h>

#include <iostream>

class Actor : public Theron::Actor
{
public:

    explicit Actor(Theron::Framework &framework) : Theron::Actor(framework)
    {
        RegisterHandler(this, &Actor::Handler);
    }

private:

    void Handler(const int &message, const Theron::Address from)
    {
        std::cerr << message << std::endl;
        Send(message, from);
    }
};

int main()
{
    Theron::Receiver receiver;
    Theron::Framework framework;
    Actor actor(framework);

    framework.Send(int(0), receiver.GetAddress(), actor.GetAddress());
    framework.Send(int(32), receiver.GetAddress(), actor.GetAddress());

    receiver.Wait();
} 