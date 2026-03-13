#include "iostream"
#include "WebSocketServer.h"

STTServer::STTServer(int port) : server(port, "0.0.0.0") {
    // Listen for new connections
    server.setOnClientMessageCallback([](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "[Network] Frontend client connected!" << std::endl;
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "[Network] Frontend client disconnected." << std::endl;
        }
    });
}

void STTServer::start() {
    auto res = server.listen();
    if (!res.first) {
        std::cout << "[Network Error] Could not start server: " << res.second << std::endl;
        return;
    }
    server.start();
    std::cout << "[Network] WebSocket server broadcasting on ws://localhost:8080" << std::endl;
}

void STTServer::stop() {
    server.stop();
}

void STTServer::broadcast(const std::string& source, const std::string& text) {
    // Format the payload as a clean JSON string
    std::string payload = R"({"source": ")" + source + R"(", "text": ")" + text + R"("})";
    
    // Broadcast to every connected web/app client
    for (auto client : server.getClients()) {
        client->sendText(payload);
    }
}