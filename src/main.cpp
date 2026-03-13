#include "thread"
#include "fstream"
#include "iostream"
#include "audio/Loopback.h"
#include "core/RingBuffer.h"
#include "audio/Microphone.h"
#include "engine/SherpaEngine.h"
#include "server/WebSocketServer.h"
#include <ixwebsocket/IXNetSystem.h>

ModelPaths load_config(const std::string &filename)
{
    ModelPaths paths;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "[Warning] Could not find " << filename << "! Please ensure it exists." << std::endl;
        return paths;
    }

    std::string line;
    while (std::getline(file, line))
    {
        auto pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);

            if (key == "encoder")
                paths.encoder = val;
            if (key == "decoder")
                paths.decoder = val;
            if (key == "joiner")
                paths.joiner = val;
            if (key == "tokens")
                paths.tokens = val;
            if (key == "num_threads")
            {
                try
                {
                    paths.num_threads = std::stoi(val);
                }
                catch (...)
                {
                    std::cerr << "[Config Warning] Invalid num_threads. Defaulting to 2." << std::endl;
                    paths.num_threads = 2;
                }
            }
        }
    }
    return paths;
}

int main()
{
    std::cout << "Starting DualSTT Daemon..." << std::endl;

    ModelPaths paths = load_config("config.txt");

    ix::initNetSystem();
    STTServer server(8080);
    server.start();

    RingBuffer mic_ring(16000 * 5);
    RingBuffer sys_ring(16000 * 5);

    std::thread ai_mic(sherpa_engine, std::ref(mic_ring), "Mic", std::ref(server), paths);
    std::thread ai_sys(sherpa_engine, std::ref(sys_ring), "System", std::ref(server), paths);

    std::thread loopback_thread(loopback_capture_worker_windows, std::ref(sys_ring));
    std::thread microphone_thread(mic_capture_worker_windows, std::ref(mic_ring));

    ai_mic.join();
    ai_sys.join();
    loopback_thread.join();
    microphone_thread.join();

    ix::uninitNetSystem();
    return 0;
}