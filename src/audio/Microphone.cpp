#include "vector"
#include "cassert"
#include "iostream"
#include "Windows.h"
#include "core/VAD.h"
#include "Microphone.h"
#include "Audioclient.h"
#include "mmdeviceapi.h"
#pragma comment(lib, "Ole32.lib")

void mic_capture_worker_windows(RingBuffer &ring)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    assert(SUCCEEDED(hr));

    IMMDeviceEnumerator *enumerator = NULL;
    IMMDevice *recorder = NULL;
    IAudioClient *recorderClient = NULL;
    IAudioCaptureClient *captureService = NULL;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&enumerator);
    assert(SUCCEEDED(hr));

    hr = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &recorder);
    assert(SUCCEEDED(hr));

    hr = recorder->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void **)&recorderClient);
    assert(SUCCEEDED(hr));

    // Force 16kHz Mono Float32
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wfx.nSamplesPerSec = 16000;
    wfx.nChannels = 1;
    wfx.wBitsPerSample = 32;
    wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    hr = recorderClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
        10000000,
        0,
        &wfx,
        NULL);
    assert(SUCCEEDED(hr));

    hr = recorderClient->GetService(__uuidof(IAudioCaptureClient), (void **)&captureService);
    assert(SUCCEEDED(hr));

    std::vector<float> local_batch;
    local_batch.reserve(4096);
    int hangover_frames = 0;
    UINT32 nFrames;
    DWORD flags;
    BYTE *recorderBuffer;

    recorderClient->Start();
    std::cout << "[Hardware] Microphone capture started..." << std::endl;

    while (true)
    {
        UINT32 packetLength = 0;
        captureService->GetNextPacketSize(&packetLength);

        if (packetLength == 0)
        {
            Sleep(5);
            continue;
        }

        while (packetLength != 0)
        {
            hr = captureService->GetBuffer(&recorderBuffer, &nFrames, &flags, NULL, NULL);
            assert(SUCCEEDED(hr));

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                local_batch.assign(nFrames, 0.0f);
            }
            else
            {
                float *pcm = (float *)recorderBuffer;
                local_batch.assign(pcm, pcm + nFrames);
            }

            bool is_speaking_now = IsSpeech(local_batch.data(), nFrames);

            if (is_speaking_now)
            {
                hangover_frames = 30;
            }

            if (hangover_frames > 0)
            {
                ring.push(local_batch);

                if (!is_speaking_now)
                {
                    hangover_frames--;
                }
            }

            hr = captureService->ReleaseBuffer(nFrames);
            assert(SUCCEEDED(hr));

            hr = captureService->GetNextPacketSize(&packetLength);
            assert(SUCCEEDED(hr));
        }
    }
    recorder->Release();
    enumerator->Release();
    CoUninitialize();
}