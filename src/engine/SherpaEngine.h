#pragma once
#include "core/RingBuffer.h"
#include "server/WebSocketServer.h"
void sherpa_engine(RingBuffer &ring, std::string source_name, STTServer &server);