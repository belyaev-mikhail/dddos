/*
 * SynFloodChecker.h
 *
 *  Created on: Feb 19, 2014
 *      Author: belyaev
 */

#ifndef SYNFLOODCHECKER_H_
#define SYNFLOODCHECKER_H_

#include <unordered_map>

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

namespace ignis {

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

} /* namespace ignis */

#endif /* SYNFLOODCHECKER_H_ */
