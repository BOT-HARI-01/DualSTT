#pragma once
#include "string"
#include "core/RingBuffer.h"
#include "server/WebSocketServer.h"

struct ModelPaths {
    std::string encoder;
    std::string decoder;
    std::string joiner;
    std::string tokens;
    int num_threads = 2;
};

void sherpa_engine(RingBuffer &ring, std::string source_name, STTServer &server, ModelPaths paths);