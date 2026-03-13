#pragma once
#include "string"
#include "ixwebsocket/IXWebSocketServer.h"

class STTServer {
private:
    ix::WebSocketServer server;
public:
    STTServer(int port = 8080);
    void start();
    void stop();
    void broadcast(const std::string& source, const std::string& text);
};