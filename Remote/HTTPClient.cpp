#include <iostream>

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include <Theron/Theron.h>

#include "Remote/HTTPClient.h"

namespace callophrys {
namespace remote {

struct RestActorRefBase::Impl {
    Poco::URI uri;

    explicit Impl(const Poco::URI& uri): uri{uri} {};
};

RestActorRefBase::RestActorRefBase(Theron::Framework &framework, const std::string& uri) :
    Theron::Actor(framework),
    pImpl(new Impl{ Poco::URI{ uri.c_str() } })
{
    RegisterHandler(this, &RestActorRefBase::Handler);
}

RestActorRefBase::~RestActorRefBase() {}

void RestActorRefBase::Handler(const util::JsonValue& message, const Theron::Address)
{
    using namespace Poco::Net;
    auto&& uri = pImpl->uri;
    HTTPClientSession session(uri.getHost(), uri.getPort());

    // prepare path
    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";

    // send request
    HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
    req.setContentType("text/json");
    session.sendRequest(req) << message.toStyledString();

    // get response
    HTTPResponse res;
    // print response
    auto&& is = session.receiveResponse(res);

    std::cout << res.getStatus() << " " << res.getReason() << std::endl;
    Poco::StreamCopier::copyStream(is, std::cout);
    std::cout << std::endl;

}

namespace {

class RestActorImplRequestHandler : public Poco::Net::HTTPRequestHandler
{
    RestActorImplBase* actor;
public:
    RestActorImplRequestHandler(RestActorImplBase* actor): actor(actor) {}

    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
    {
        Json::Value v;
        Json::Reader reader;
        auto res = reader.parse(req.stream(), v, false);

        if(res) std::cout << v.toStyledString();

        using namespace Poco::Net;
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/json");

        auto&& out = resp.send();
        out << "{ \"result\": 0, \"id\": \"0xdeadbeef\" }";
        out.flush();

        std::cout << "Yer wish is granted, lol" << std::endl;
    }
};

class RestActorImplRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
    RestActorImplBase* actor;

public:
    RestActorImplRequestHandlerFactory(RestActorImplBase* actor): actor(actor) {}

    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &)
    {
        return new RestActorImplRequestHandler(actor);
    }
};

} /* empty namespace */

struct RestActorImplBase::Impl {
    Poco::Net::HTTPServer server;

    Impl(RestActorImplBase* actor, uint16_t port):
        server(        
            new RestActorImplRequestHandlerFactory(actor), 
            Poco::Net::ServerSocket(port), 
            new Poco::Net::HTTPServerParams
        ) {};
};

RestActorImplBase::RestActorImplBase(Theron::Framework &framework, uint16_t port) : Theron::Actor(framework), pImpl() {
    RegisterHandler(this, &RestActorImplBase::Handler);

    pImpl.reset(new Impl(this, port));
    pImpl->server.start();
}

RestActorImplBase::~RestActorImplBase() {
    pImpl->server.stop();
}

void RestActorImplBase::Handler(const util::JsonValue& message, const Theron::Address)
{
    std::cout << "Message received, lol" << std::endl;
    std::cout << message.toStyledString() << std::endl;
}

} /* namespace remote */
} /* namespace callophrys */
