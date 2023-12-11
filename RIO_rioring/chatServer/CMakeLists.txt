cmake_minimum_required(VERSION 3.15)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

if (WIN32)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(chatserver)
set(CMAKE_CXX_STANDARD 17)

include_directories(${RIORING}/include)

if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNDEBUG /O2 /Oi /Oy /GL")
    add_definitions(/DWIN32_LEAN_AND_MEAN /DNOMINMAX /D_AMD64_ /D_LIB /D_WIN32_WINNT=0x0A00)
    add_compile_options(/MP)
else()
    add_compile_options(-g -fthreadsafe-statics -fno-omit-frame-pointer -Wno-format-security)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

add_executable(chatserver
        main.cpp
        client.cpp
        pool.cpp
        receiver.cpp)

target_link_directories(chatserver PRIVATE ${RIORING}/lib)
if (WIN32)
    target_link_options(chatserver PRIVATE /LTCG)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_link_libraries(chatserver PRIVATE rioringd)
    else()
        target_link_libraries(chatserver PRIVATE rioring)
    endif()
else()
    # Must cmake --install to PATH directory
    # If you do not, configure target_link_directories like WIN32
    target_link_libraries(chatserver PRIVATE rioring uring)
endif()