/*
 * Agent.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: belyaev
 */

#include "Driver/Agent.h"

namespace ignis {

void Agent::start() {
    using namespace ignis;
    Theron::Framework loggerFramework { Theron::Framework::Parameters{ 1U } };
    Logger logger(loggerFramework);

    auto sniffer = std::thread{
        [](Theron::Address loggerAddress, bool promisc, const std::string& iface){
            Theron::Framework local { Theron::Framework::Parameters{ 0U } };
            SynFloodChecker checker { &local, loggerAddress };

            Tins::Sniffer sniffer(iface, 2000, promisc, "");
            for(auto&& pdu : sniffer) {
                checker(pdu);
            }
        },
        logger.GetAddress(),
        promisc.getValue(),
        interface.getValue()
    };

    sniffer.join();
}

} /* namespace borealis */
