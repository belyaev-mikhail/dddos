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

<<<<<<< local
namespace ignis {
=======
namespace ignis  {
>>>>>>> other
namespace remote {

struct RestActorRef::Impl {
    Poco::URI uri;
};

RestActorRef::RestActorRef(Theron::Framework &framework, const std::string& uri) : 
    Theron::Actor(framework),
    pImpl(new Impl{ Poco::URI{ uri.c_str() } })
{
    RegisterHandler(this, &RestActorRef::Handler);
}

RestActorRef::~RestActorRef() {}

void RestActorRef::Handler(const util::JsonValue& message, const Theron::Address)
{
    using namespace Poco::Net;
    auto&& uri = pImpl->uri;
    HTTPClientSession session(uri.getHost(), uri.getPort());

    // prepare path
    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";

    // send request
    HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
<<<<<<< local
    req.setContentType("text/json");
=======
>>>>>>> other
    session.sendRequest(req) << message.toStyledString();
<<<<<<< local

=======
>>>>>>> other
    // get response
    HTTPResponse res;
    std::cout << res.getStatus() << " " << res.getReason() << std::endl;

    // print response
    auto&& is = session.receiveResponse(res);
    Poco::StreamCopier::copyStream(is, std::cout);

}

namespace {

class RestActorImplRequestHandler : public Poco::Net::HTTPRequestHandler
{
    RestActorImpl* actor;
public:
    RestActorImplRequestHandler(RestActorImpl* actor): actor(actor) {}

    virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
    {
        using namespace Poco::Net;
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/json");

        auto&& out = resp.send();
        out << "{}";
        out.flush();
    }
};

class RestActorImplRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
    RestActorImpl* actor;

public:
    RestActorImplRequestHandlerFactory(RestActorImpl* actor): actor(actor) {}

    virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &)
    {
        return new RestActorImplRequestHandler(actor);
    }
};

} /* empty namespace */

struct RestActorImpl::Impl { 
    Poco::Net::HTTPServer server;

    Impl(RestActorImpl* actor, uint16_t port): 
        server(        
            new RestActorImplRequestHandlerFactory(actor), 
            Poco::Net::ServerSocket(port), 
            new Poco::Net::HTTPServerParams
        ) {};
};

RestActorImpl::RestActorImpl(Theron::Framework &framework, uint16_t port) : Theron::Actor(framework), pImpl() {
    RegisterHandler(this, &RestActorImpl::Handler);

    pImpl.reset(new Impl(this, port));
    pImpl->server.start();
}

RestActorImpl::~RestActorImpl() {
    pImpl->server.stop();
}

void RestActorImpl::Handler(const util::JsonValue& message, const Theron::Address)
{
    std::cout << "Message received, lol" << std::endl;
}

} /* namespace remote */
} /* namespace ignis */
