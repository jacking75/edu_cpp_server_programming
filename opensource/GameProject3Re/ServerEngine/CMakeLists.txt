cmake_minimum_required(VERSION 2.8.9)
project(ServerEngine)
SET(PROJECT_ROOT_PATH ".")

#header file code
set(HEADER_CODE_FILE HeaderFiles)
FILE(GLOB HEADER_CODE_FILE "./*.h")


#source file code
set(SOURCE_CODE_FILE SourceFiles)
FILE(GLOB SOURCE_CODE_FILE "./*.cpp")

source_group(HeaderFiles FILES ${HEADER_CODE_FILE} )
source_group(SourceFiles FILES ${SOURCE_CODE_FILE} )

# Target
add_library(ServerEngine
    ${HEADER_CODE_FILE} 
    ${SOURCE_CODE_FILE} 
)

if(MSVC)
    target_link_libraries(ServerEngine libmysql ws2_32 )
    ADD_DEFINITIONS(-DSERVER_ENGINE -D_CONSOLE -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    ADD_DEFINITIONS(-DSERVER_ENGINE -D_CONSOLE -D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pthread -std=c++11")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -pthread -std=c++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O3 -std=c++11")
endif()
