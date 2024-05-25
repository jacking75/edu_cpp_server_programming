#include "EchoServer.h"

#include <CrashReport.h>
#include <CommandLine.h>

#include <iostream>

int main(int argc, char* argv[])
{
    hbServerEngine::SetCrashReport("EchoServer");

    hbServerEngine::CommandLine cmdLine(argc, argv);

    EchoServer server;
    server.Init();
    server.Run();

    server.Uninit();

    std::cout << "Hello World!\n";


    hbServerEngine::UnSetCrashReport();

    return 0;
}

//TODO: 모든 경고 제거하기

//TODO: 코딩룰 따르기