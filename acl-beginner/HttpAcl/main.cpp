#include "acl_cpp/lib_acl.hpp"  // Must before http_server.hpp
#include "fiber/http_server.hpp"
#include "http_controller.hpp"
#include <print>

// 서버의 설정 변수
static char* var_cfg_debug_msg;
static acl::master_str_tbl var_conf_str_tab[] = {
  { "debug_msg", "test_msg", &var_cfg_debug_msg },
  { 0, 0, 0 }
};

static int var_cfg_io_timeout;
static acl::master_int_tbl var_conf_int_tab[] = {
  { "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },
  { 0, 0 , 0 , 0, 0 }
};

int main() {
    const char* addr = "0.0.0.0|8194";

    acl::acl_cpp_init();

    acl::http_server server;

    // 서버 설정값 초기화
    server.set_cfg_int(var_conf_int_tab).set_cfg_str(var_conf_str_tab);

    // 라우팅 설정
    HttpController controller;
    controller.configure_routes(server);

    // 서버 실행
    server.on_proc_init([&addr] {
        std::print("---> after process init: addr={}, io_timeout={}\n", addr, var_cfg_io_timeout);
    }).on_proc_exit([] {
        std::print("---> before process exit\n");
    }).on_proc_sighup([](acl::string& s) {
        s = "+ok";
        std::print("---> process got sighup\n");
        return true;
    }).on_thread_accept([](acl::socket_stream& conn) {
        std::print("---> accept {}\n", conn.sock_handle());
        return true;
    }).run_alone(addr);

    return 0;
}
