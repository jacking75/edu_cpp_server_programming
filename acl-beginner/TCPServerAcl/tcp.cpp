#include <thread>
#include <print>
#include "pch.h"
#include "tcp.h"

void handle_client(acl::socket_stream* conn) {
    char buf[256];
    while (true) {
        int ret = conn->read(buf, sizeof(buf), false);  // 클라이언트로부터 데이터 읽기
        if (ret <= 0) break;  // 연결이 종료되거나 오류 발생 시 루프 탈출

        buf[ret] = '\0';  // 문자열로 처리하기 위해 null-terminator 추가
        std::print("Received from client: {}\n", buf);

        conn->write(buf, ret);  // 읽은 데이터를 클라이언트로 에코
    }
    delete conn;  // 연결 종료 후 소켓 삭제
}

void run_tcp_server() {
    const char* addr = "127.0.0.1:8088";
    acl::server_socket server;

    if (!server.open(addr)) {  // 로컬 주소에 바인드하고 리슨
        std::print("Failed to open server socket on {}\n", addr);
        return;
    }

    std::print("Server is running on {}\n", addr);

    while (true) {
        acl::socket_stream* conn = server.accept(); // 클라이언트 연결 대기
        if (conn == nullptr) {
            std::print("Failed to accept connection\n");
            continue;
        }

        std::thread client_thread(handle_client, conn);  // 새 스레드에서 클라이언트 처리
        client_thread.detach();  // 스레드를 분리하여 독립적으로 실행되도록 함
    }
}