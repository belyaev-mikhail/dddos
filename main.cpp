#include <Theron/Theron.h>

#include <tins/ethernetII.h>
#include <tins/ip.h>
#include <tins/ip_address.h>
#include <tins/ipv6.h>
#include <tins/ipv6_address.h>
#include <tins/tcp.h>
#include <tins/network_interface.h>
#include <tins/sniffer.h>
#include <tins/utils.h>
#include <tins/packet_sender.h>

#include <iostream>
#include <string>

class Logger : public Theron::Actor
{
public:

    explicit Logger(Theron::Framework &framework) : Theron::Actor(framework)
    {
        RegisterHandler(this, &Logger::Handler);
    }

private:

    void Handler(const std::string& message, const Theron::Address)
    {
        std::cerr << message << std::endl;
    }
};

inline bool isDedicatedSyn(const Tins::TCP& tcp) {
    return tcp.flags() == Tins::TCP::SYN;
}

inline bool isSynAck(const Tins::TCP& tcp) {
    return tcp.flags() == (Tins::TCP::SYN | Tins::TCP::ACK);
}

int main(int argc, char* argv[])
{
    Theron::Framework loggerFramework { Theron::Framework::Parameters{ 1U } };
    Logger logger(loggerFramework);

    Tins::Sniffer sniffer(argv[1], 2000, true, "");
    for(auto&& pdu : sniffer) {
        if(auto ip = pdu.find_pdu<Tins::IP>()) {
            loggerFramework.Send("From:" + ip->src_addr().to_string(), Theron::Address::Null(), logger.GetAddress());
            loggerFramework.Send("To:" + ip->dst_addr().to_string(), Theron::Address::Null(), logger.GetAddress());
        }
        if(auto ip = pdu.find_pdu<Tins::IPv6>()) {
            loggerFramework.Send("From:" + ip->src_addr().to_string(), Theron::Address::Null(), logger.GetAddress());
            loggerFramework.Send("To:" + ip->dst_addr().to_string(), Theron::Address::Null(), logger.GetAddress());
        }
        if(auto tcp = pdu.find_pdu<Tins::TCP>()) {
            if(isSynAck(*tcp)) {
                loggerFramework.Send(std::string("SYN-ACK"), Theron::Address::Null(), logger.GetAddress());
            }
            
            if(isDedicatedSyn(*tcp)) {
                loggerFramework.Send(std::string("SYN"), Theron::Address::Null(), logger.GetAddress());
            }
        }
            
    }
 
}