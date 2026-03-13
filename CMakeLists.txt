cmake_minimum_required(VERSION 3.15)
project(DualSTT)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(capture_mic ${SRC_FILES})

target_include_directories(capture_mic PRIVATE src)
# Link your code to the Sherpa-ONNX C++ API
target_link_libraries(capture_mic PRIVATE sherpa-onnx-cxx-api)