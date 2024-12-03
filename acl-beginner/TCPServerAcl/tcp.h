#pragma once
void run_tcp_server();  // TCP 서버 실행 함수 선언
void handle_client(acl::socket_stream* conn);  // 클라이언트 처리 함수 선언