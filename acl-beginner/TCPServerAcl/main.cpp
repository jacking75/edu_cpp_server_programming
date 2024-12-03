#include "pch.h"
#include "tcp.h"

int main() {
    try {
        SetConsoleOutputCP(CP_UTF8); // 콘솔의 코드 페이지를 UTF-8로 설정

        acl::acl_cpp_init();  // ACL 초기화
        run_tcp_server();  // TCP 서버 실행 함수 호출
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught" << std::endl;
    }

    return 0;
}
