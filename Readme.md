# DualSTT

A low latency background daemon that captures audio and transcribes it in real-time using Sherpa-ONNX.

## Current Status
- [x] Modular, platform-agnostic C++ Core Architecture
- [x] Thread-safe RingBuffer memory management
- [x] Sherpa-ONNX INT8 streaming inference (Low-latency)
- [x] Hardware Audio Backend: Windows (WASAPI) Microphone Capture
- [ ] Hardware Audio Backend: Windows (WASAPI) System Loopback
- [ ] Cross-platform WebSocket JSON Broadcasting (Pending)

## Prerequisites
* C++ 17 Compiler (MSVC, GCC, or Clang)
* CMake (v3.15+)

## Build Instructions

1. **Clone the repository and submodules:**
   (Ensure you have downloaded the Sherpa-ONNX models into the `models/sherpa/` directory).

2. **Generate the build files:**
   ```bash
   cmake -B build
    ```
3. **Compile the Release build:**
    ```bash
    cmake --build build --config Release
    ```
4. **From the root DualSTT directory:**
    ```bash
    .\build\Release\capture_mic.exe 
    ```