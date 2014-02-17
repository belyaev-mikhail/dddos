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
#include <unordered_map>
#include <thread>

#include "Actors/Logger.h"

inline bool isDedicatedSyn(const Tins::TCP& tcp) {
    return tcp.flags() == Tins::TCP::SYN;
}

inline bool isSynAck(const Tins::TCP& tcp) {
    return tcp.flags() == (Tins::TCP::SYN | Tins::TCP::ACK);
}

struct SynFloodChecker {
    Theron::Framework* framework;
    Theron::Address logger;
    std::unordered_map<Tins::IPv4Address, size_t> factors;

    SynFloodChecker(Theron::Framework* framework, Theron::Address logger): 
        framework(framework), logger(logger), factors{} {};

    void operator()(Tins::PDU& pdu) {
        if(auto tcp = pdu.find_pdu<Tins::TCP>()) {
            if(auto ip = pdu.find_pdu<Tins::IP>()) {
                if(isSynAck(*tcp)) {
                    framework->Send(std::string("SYN-ACK"), Theron::Address::Null(), logger);
                    factors[ip->dst_addr()]--;
                }
            
                if(isDedicatedSyn(*tcp)) {
                    framework->Send(std::string("SYN"), Theron::Address::Null(), logger);
                    factors[ip->src_addr()]++;
                }
            }
        }
    }
};

int main(int argc, char* argv[])
{
    using namespace ignis;
    Theron::Framework loggerFramework { Theron::Framework::Parameters{ 1U } };
    Logger logger(loggerFramework);

    auto sniffer = std::thread{
        [argv](Theron::Address loggerAddress){
            Theron::Framework local { Theron::Framework::Parameters{ 0U } };
            SynFloodChecker checker { &local, loggerAddress };

            Tins::Sniffer sniffer(argv[1], 2000, true, "");
            for(auto&& pdu : sniffer) {
                checker(pdu);
            }
        },
        logger.GetAddress()
    };


    sniffer.join();


}