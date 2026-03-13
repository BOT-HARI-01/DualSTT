cmake_minimum_required(VERSION 3.15)
project(DualSTT)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_definitions(WIN32_LEAN_AND_MEAN)
endif()

# CONFIGURE SHERPA-ONNX
# Force static linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Turn off things we don't need to speed up compilation
set(SHERPA_ONNX_ENABLE_PYTHON OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SHERPA_ONNX_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add the Sherpa-ONNX repository
add_subdirectory(sherpa-onnx)

include(FetchContent)
set(USE_TLS OFF CACHE BOOL "" FORCE)
set(USE_ZLIB OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  ixwebsocket
  GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(ixwebsocket)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(capture_mic ${SRC_FILES})

target_include_directories(capture_mic PRIVATE src ${ixwebsocket_SOURCE_DIR})

# Linking code to the Sherpa-ONNX C++ API
target_link_libraries(capture_mic PRIVATE 
    sherpa-onnx-cxx-api 
    ixwebsocket 
    ws2_32 
    crypt32 
    bcrypt)