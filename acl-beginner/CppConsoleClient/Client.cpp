#include "pch.h"

int main() {
    SetConsoleOutputCP(CP_UTF8); // 콘솔의 코드 페이지를 UTF-8로 설정

    const char* addr = "127.0.0.1:8088";
    int conn_timeout = 5, rw_timeout = 10;
    acl::acl_cpp_init();  // ACL 초기화
    acl::socket_stream conn;

    if (!conn.open(addr, conn_timeout, rw_timeout)) {  // 서버에 연결
        std::cerr << "Failed to connect to server at " << addr << std::endl;
        return -1;
    }
    std::cout << "Connected to server at " << addr << std::endl;

    std::string message;
    char buf[256];

    while (true) {
        std::cout << "Enter a message (0 to quit): ";
        std::getline(std::cin, message);

        if (message == "0") {
            std::cout << "Exiting..." << std::endl;
            break;  // 루프 탈출 및 프로그램 종료
        }

        if (message.empty()) {
            std::cerr << "No message entered, please try again." << std::endl;
            continue;
        }

        if (conn.write(message.c_str(), message.size()) == -1) {  // 서버로 데이터 전송
            std::cerr << "Failed to send data to server" << std::endl;
            break;
        }

        int ret = conn.read(buf, sizeof(buf) - 1, false);
        if (ret > 0) {  // 서버로부터 데이터 수신
            buf[ret] = '\0';
            std::cout << "Echo from server: " << buf << std::endl;
        }
        else {
            std::cerr << "Failed to read data from server" << std::endl;
            break;
        }
    }

    return 0;

}
