//#include "pch.h"
#include "tcpCoroutine.h"
#include <print>
#include "acl_cpp/lib_acl.hpp"
#include "fiber/go_fiber.hpp"


void run_tcp_coroutine_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;
    if (!server.open(addr)) {
        std::cerr << "Failed to open server socket on " << addr << std::endl;
        return;
    }

    std::print("Coroutine TCP Server is running on {}\n", addr);

    go[&]{
        while (true) {
            acl::socket_stream* conn = server.accept();
            if (conn) {
                go[=] {
                    char buf[256];
                    while (true) {
                        int ret = conn->read(buf, sizeof(buf), false);
                        if (ret <= 0) {  // 데이터 읽기 실패 또는 연결 종료 시
                            std::cerr << "Failed to read data from client or connection closed." << std::endl;
                            break;
                        }

                        buf[ret] = '\0';  // 문자열로 처리하기 위해 null-terminator 추가
                        std::cout << "Received from client: " << buf << std::endl;

                        if (conn->write(buf, ret) != ret) {
                            std::cerr << "Failed to send data to client." << std::endl;
                            break;
                        }
                    }
                    delete conn;  // 연결 종료 시 소켓 스트림 삭제
                };
            }
        }
    };

    acl::fiber::schedule();  // 코루틴 스케줄러 시작
}
