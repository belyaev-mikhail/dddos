/*
 * Agent.h
 *
 *  Created on: Feb 19, 2014
 *      Author: belyaev
 */

#ifndef AGENT_H_
#define AGENT_H_

#include <thread>

#include <Theron/Theron.h>

#include <tclap/ValueArg.h>
#include <tclap/SwitchArg.h>
#include <tclap/CmdLine.h>

#include "Actors/Logger.h"

#include "Driver/Version.h"

#include "Checkers/SynFloodChecker.h"

namespace callophrys {

class Agent {

    TCLAP::CmdLine cmd;
    TCLAP::SwitchArg promisc;
    TCLAP::ValueArg<std::string> interface;
    TCLAP::ValueArg<std::string> file;
    TCLAP::ValueArg<std::string> configFile;

public:
    Agent(int argc, char** argv):
            cmd{ "An agent", ' ', VERSION },
            promisc{ "p", "promisc", "Turn on promiscious mode for your network card", false },
            interface{ "i", "interface", "Use network interface", false,
                Tins::NetworkInterface::default_interface().name(), "ethN" },
            file{ "f", "file", "Use a pcap file as a source", false, "", "file" },
            configFile{ "", "with-config", "Use <file> as a configuration file", false, "", "file" }
    {
        cmd.add(promisc);
        cmd.add(interface);
        cmd.add(file);
        cmd.add(configFile);
        cmd.parse(argc, argv);
    }

    void start();
};

} /* namespace borealis */

#endif /* AGENT_H_ */
