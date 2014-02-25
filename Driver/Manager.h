/*
 * Manager.h
 *
 *  Created on: Feb 25, 2014
 *      Author: belyaev
 */

#ifndef MANAGER_H_
#define MANAGER_H_

#include <Theron/Theron.h>

#include <tclap/ValueArg.h>
#include <tclap/SwitchArg.h>
#include <tclap/CmdLine.h>

#include "Driver/Version.h"
#include "Actors/Logger.h"
#include "Remote/HTTPClient.h"

namespace callophrys {

class Manager {
    struct Cmd {
        TCLAP::CmdLine cmd;
        TCLAP::ValueArg<unsigned short> port;
        TCLAP::ValueArg<std::string> configFile;

        Cmd(int argc, char** argv) :
            cmd{ "Callophrys manager", ' ', VERSION },
            port{ "t", "port", "Specify tcp port to listen to", false, 8080U, "port", cmd },
            configFile{ "", "with-config", "Use <file> as a configuration file", false, "", "file", cmd }
        {
            cmd.parse(argc, argv);
        }
    };

    Cmd cmd;

    Theron::Framework loggerFramework;
    Logger logger;

    Theron::Framework serverFramework;
    remote::RestActorImpl serverActor;

public:

    Manager(int argc, char** argv):
        cmd{ argc, argv },
        loggerFramework{ Theron::Framework::Parameters{ 1U } },
        logger{ loggerFramework },
        serverFramework{ Theron::Framework::Parameters{ 16U } },
        serverActor{ serverFramework, cmd.port.getValue() }
    {}

    void start() {
        Theron::Receiver forever;
        forever.Wait();
    }
};

} /* namespace callophrys */

#endif /* MANAGER_H_ */
