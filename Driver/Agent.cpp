/*
 * Agent.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: belyaev
 */

#include "Driver/Agent.h"

namespace callophrys {

void Agent::start() {

    Theron::Framework local { Theron::Framework::Parameters{ 0U } };
    SynFloodChecker checker { &local, logger.GetAddress() };

    Tins::Sniffer sniffer(
        interface.getValue(),   // interface name
        2000,                   // max pdu size
        promisc.getValue(),     // is promisc
        ""                      // berkeley packet filter string
    );
    for(auto&& pdu : sniffer) {
        checker(pdu);
    }
}

} /* namespace borealis */
