# TO USE THIS:
# cd headless

# for macOS/native:
# cd build-native
# REBUILD MAKEFILES: cmake ..
# BUILD: make ./headless

#for WASM:
# make sure PATH includes emscripten:
# source '/Users/alex/emsdk/emsdk_env.sh'

# cd build-wasm
# emcmake cmake ..
# emmake make

# RUN PYTHON SERVER: python3 serve.py

cmake_minimum_required(VERSION 3.13)
project(headless)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE APP_SOURCES       ${CMAKE_SOURCE_DIR}/../src/*.cpp)
file(GLOB_RECURSE SHIM_SOURCES      ${CMAKE_SOURCE_DIR}/childOS_headless/*.cpp)


# Remove the bitmap examples
list(REMOVE_ITEM SHIM_SOURCES
    ${CMAKE_SOURCE_DIR}/childOS_headless/bitmap/bitmap_test.cpp
)
# remove any cmake .cpp's in case they're accidentally globbed
list(FILTER SHIM_SOURCES EXCLUDE REGEX "/CMakeFiles/.*")

# ignore rtmidi subfolders
list(FILTER SHIM_SOURCES EXCLUDE REGEX "/rtmidi-6\\.0\\.0/[^/]+/.*")
if (EMSCRIPTEN)
    # exclude RtMIDI entirely for rn
    list(FILTER SHIM_SOURCES EXCLUDE REGEX "/RtMidi\\.cpp$")
endif()

# exclude hardware-only files for headless mode
list(REMOVE_ITEM APP_SOURCES
    ${CMAKE_SOURCE_DIR}/../src/StepchildFileSystem.cpp
    ${CMAKE_SOURCE_DIR}/../src/StepchildIO.cpp
    ${CMAKE_SOURCE_DIR}/../src/StepchildMIDI.cpp
)


add_executable(headless
    ${CMAKE_SOURCE_DIR}/childOS_headless/main.cpp
    ${APP_SOURCES}
    ${SHIM_SOURCES}
)

target_include_directories(headless PRIVATE
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../src
    ${CMAKE_SOURCE_DIR}/childOS_headless
)

target_compile_definitions(headless PRIVATE 
    ASSET_ROOT="${CMAKE_SOURCE_DIR}/assets/"
    HEADLESS
    __MACOSX_CORE__
)

if (EMSCRIPTEN)
    # tell CMAKE to listen to the shell file for updates
    set(SHELL_FILE ${CMAKE_SOURCE_DIR}/shell.html)

    target_link_options(headless PRIVATE
        -sUSE_GLFW=3
        -sUSE_WEBGL2=1
        -sFULL_ES3=1
        -sGL_ENABLE_GET_PROC_ADDRESS=1
        -sALLOW_MEMORY_GROWTH=1
        -sASYNCIFY=1
        -pthread
        -sPTHREAD_POOL_SIZE=2
        -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']
        --shell-file ${SHELL_FILE}
    )
    set_target_properties(headless PROPERTIES
        SUFFIX ".html"
        LINK_DEPENDS ${SHELL_FILE}
    )
else()
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(headless PRIVATE
        glfw
        OpenGL::GL
        "-framework CoreMIDI"
        "-framework CoreAudio"
        "-framework CoreFoundation"
    )
endif()