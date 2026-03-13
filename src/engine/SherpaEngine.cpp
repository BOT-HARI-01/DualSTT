#include "vector"
#include "Windows.h"
#include "iostream"
#include "vector"
#include "SherpaEngine.h"
#include "sherpa-onnx/c-api/c-api.h"

void sherpa_engine(RingBuffer &ring)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

    SherpaOnnxOnlineRecognizerConfig config = {};

    config.model_config.transducer.encoder = "models/sherpa/encoder-epoch-99-avg-1-chunk-16-left-128.int8.onnx";
    config.model_config.transducer.decoder = "models/sherpa/decoder-epoch-99-avg-1-chunk-16-left-128.int8.onnx";
    config.model_config.transducer.joiner = "models/sherpa/joiner-epoch-99-avg-1-chunk-16-left-128.int8.onnx";
    config.model_config.tokens = "models/sherpa/tokens.txt";

    config.model_config.num_threads = 2;
    config.model_config.provider = "cpu";

    config.feat_config.sample_rate = 16000;
    config.feat_config.feature_dim = 80;

    config.enable_endpoint = 1;
    // config.rule1_min_trailing_silence = 1.2f;
    config.decoding_method = "greedy_search";

    const SherpaOnnxOnlineRecognizer *recognizer = SherpaOnnxCreateOnlineRecognizer(&config);
    if (!recognizer)
    {
        std::cout << "[Error] Failed to create Sherpa Recognizer." << std::endl;
        return;
    }
    const SherpaOnnxOnlineStream *stream = SherpaOnnxCreateOnlineStream(recognizer);

    const int CHUNK_SIZE = 4800;
    std::vector<float> stt_buffer(CHUNK_SIZE);
    std::string last_text = "";

    std::cout << "[AI Worker] Streaming Engine Ready. Start speaking...\n"<< std::endl;

    while (true)
    {
        // Wait until we have 300ms of audio
        if (ring.available() < CHUNK_SIZE)
        {
            Sleep(10);
            continue;
        }

        size_t n = ring.pop(stt_buffer.data(), CHUNK_SIZE);

        SherpaOnnxOnlineStreamAcceptWaveform(stream, 16000, stt_buffer.data(), n);

        while (SherpaOnnxIsOnlineStreamReady(recognizer, stream))
        {
            SherpaOnnxDecodeOnlineStream(recognizer, stream);
        }

        const SherpaOnnxOnlineRecognizerResult *result = SherpaOnnxGetOnlineStreamResult(recognizer, stream);

        if (result && result->text)
        {
            std::string current_text = result->text;
            if (current_text.length() > 0 && current_text != last_text)
            {
                std::cout << "\r--> " << current_text << "          " << std::flush;
                last_text = current_text;
            }
        }
        SherpaOnnxDestroyOnlineRecognizerResult(result);

        if (SherpaOnnxOnlineStreamIsEndpoint(recognizer, stream))
        {
            if (last_text.length() > 0)
            {
                std::cout << "\n";
            }
            SherpaOnnxOnlineStreamReset(recognizer, stream);
            last_text = "";
        }
    }

    SherpaOnnxDestroyOnlineStream(stream);
    SherpaOnnxDestroyOnlineRecognizer(recognizer);
}
