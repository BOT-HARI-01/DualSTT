#include "thread"
#include "iostream"
#include "audio/Loopback.h"
#include "core/RingBuffer.h"
#include "audio/Microphone.h"
#include "engine/SherpaEngine.h"
#include "server/WebSocketServer.h"
#include <ixwebsocket/IXNetSystem.h>

int main() {
    std::cout << "Starting DualSTT Daemon..." << std::endl;

    ix::initNetSystem();

    STTServer server(8080);
    server.start();

    RingBuffer mic_ring(16000 * 5);
    RingBuffer sys_ring(16000 * 5);

    std::thread ai_mic(sherpa_engine, std::ref(mic_ring), "Mic", std::ref(server));
    std::thread ai_sys(sherpa_engine, std::ref(sys_ring), "System", std::ref(server));

    std::thread loopback_thread(loopback_capture_worker_windows, std::ref(sys_ring));
    std::thread microphone_thread(mic_capture_worker_windows, std::ref(mic_ring));
    
    ai_mic.join();
    ai_sys.join();
    loopback_thread.join();
    microphone_thread.join();

    return 0;
}