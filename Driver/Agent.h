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

#include "Remote/HTTPClient.h"

namespace callophrys {

class Agent {

    struct Cmd {
        TCLAP::CmdLine cmd;
        TCLAP::SwitchArg promisc;
        TCLAP::ValueArg<std::string> interface;
        TCLAP::ValueArg<std::string> file;
        TCLAP::ValueArg<std::string> configFile;
        TCLAP::ValueArg<std::string> serverUrl;

        Cmd(int argc, char ** argv) :
            cmd{ "Callophrys agent", ' ', VERSION },
            promisc{ "p", "promisc", "Turn on promiscious mode for your network card", false },
            interface{ "i", "interface", "Use network interface", false,
                Tins::NetworkInterface::default_interface().name(), "ethN" },
            file{ "f", "file", "Use a pcap file as a source", false, "", "file" },
            configFile{ "", "with-config", "Use <file> as a configuration file", false, "", "file" },
            serverUrl { "s", "server-url", "Supply a url to connect to", false, "http://localhost:8080", "url" }
        {
            cmd.add(promisc);
            cmd.add(interface);
            cmd.add(file);
            cmd.add(configFile);
            cmd.add(serverUrl);
            cmd.parse(argc, argv);
        }
    };

    Cmd cmd;

    Theron::Framework loggerFramework;
    Logger logger;

    Theron::Framework clientFramework;
    remote::RestActorRef<Json::Value> serverRef;

public:
    Agent(int argc, char** argv):
            cmd{ argc, argv },
            loggerFramework{ Theron::Framework::Parameters{ 1U } },
            logger{ loggerFramework },
            clientFramework{ Theron::Framework::Parameters{ 1U } },
            serverRef{ clientFramework, cmd.serverUrl.getValue() }
    { }

    void start();
};

} /* namespace borealis */

#endif /* AGENT_H_ */
