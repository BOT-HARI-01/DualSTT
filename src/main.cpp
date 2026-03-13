#include "thread"
#include "iostream"
#include "audio/Loopback.h"
#include "core/RingBuffer.h"
#include "audio/Microphone.h"
#include "engine/SherpaEngine.h"
int main(){
    
    std::cout << "Starting DualSTT Deamon" << std::endl;

    RingBuffer mic_ring(16000 * 5);
    RingBuffer sys_ring(16000 * 5);

    std::thread ai_mic(sherpa_engine, std::ref(mic_ring), "Mic");
    std::thread ai_sys(sherpa_engine, std::ref(sys_ring), "System");

    std::thread loopback_thread(loopback_capture_worker_windows, std::ref(sys_ring));
    std::thread microphone_thread(mic_capture_worker_windows, std::ref(mic_ring));
    
    ai_mic.join();
    ai_sys.join();
    loopback_thread.join();
    microphone_thread.join();
    return 0;
}