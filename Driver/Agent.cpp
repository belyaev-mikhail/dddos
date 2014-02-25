/*
 * Agent.cpp
 *
 *  Created on: Feb 19, 2014
 *      Author: belyaev
 */

#include "Driver/Agent.h"

static Json::Value inline_json(const char* contents) {
    Json::Reader reader;
    Json::Value ret;
    bool res = reader.parse(contents, ret, true);
    return res ? ret : Json::Value {};
}

#define INLINE_JSON(...) inline_json(#__VA_ARGS__)

namespace callophrys {

static Json::Value sampleJson() {
    Json::Value ret = INLINE_JSON(
        {
            "numbers" : [1, 2, 3, 4, 5],
            "glossary": {
                "title": "example glossary",
                "GlossDiv": {
                    "title": "S",
                    "GlossList": {
                        "GlossEntry": {
                            "ID": "SGML",
                            "SortAs": "SGML",
                            "GlossTerm": "Standard Generalized Markup Language",
                            "Acronym": "SGML",
                            "Abbrev": "ISO 8879:1986",
                            "GlossDef": {
                                "para": "A meta-markup language, used to create markup languages such as DocBook.",
                                "GlossSeeAlso": ["GML", "XML"]
                            },
                            "GlossSee": "markup"
                        }
                    }
                }
            }
        }
    );
    std::cout << "sending: " << ret.toStyledString() << std::endl;
    return ret;
}

void Agent::start() {

    Theron::Framework local { Theron::Framework::Parameters{ 0U } };
    SynFloodChecker checker { &local, logger.GetAddress() };

    Tins::Sniffer sniffer(
        cmd.interface.getValue(),   // interface name
        2000,                   // max pdu size
        cmd.promisc.getValue(),     // is promisc
        ""                      // berkeley packet filter string
    );

    local.Send(sampleJson(), Theron::Address::Null(), serverRef.GetAddress());

    for(auto&& pdu : sniffer) {
        checker(pdu);
    }
}

} /* namespace borealis */
