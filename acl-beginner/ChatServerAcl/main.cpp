#include "pch.h"
#include "tcpCoroutine.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);

    acl::acl_cpp_init();

    run_tcp_coroutine_server_with_redis_and_rooms();

    return 0;
}
