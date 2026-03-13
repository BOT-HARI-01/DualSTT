#include "thread"
#include "iostream"
#include "core/RingBuffer.h"
#include "audio/Microphone.h"
#include "engine/SherpaEngine.h"
int main(){
    
    std::cout << "Starting DualSTT Deamon" << std::endl;

    RingBuffer ring(16000 * 5);

    std::thread ai_thread(sherpa_engine, std::ref(ring));
    mic_capture_worker_windows(ring);
    
    ai_thread.join();

    return 0;
}