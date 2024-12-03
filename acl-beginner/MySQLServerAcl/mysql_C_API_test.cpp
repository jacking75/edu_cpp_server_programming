//#include <iostream>
//#include <mysql.h>  // MySQL C API 헤더 파일
//#include <cstdlib>
//#include <stdio.h>
//#include <stdlib.h>
//#include "pch.h"
//
//// MySQL C API 헤더 파일을 통해 mysql 실습 진행하는 예제 (acl 이용 X)
//
//
//void finish_with_error(MYSQL* conn) {
//    std::cerr << "MySQL error: " << mysql_error(conn) << std::endl;
//    mysql_close(conn);  // MySQL 연결 종료
//    exit(EXIT_FAILURE); // 프로그램 종료
//}
//
//
//int main_mysql_cAPI() {
//    SetConsoleOutputCP(CP_UTF8);
//    // MySQL 핸들러 선언
//    MYSQL* conn;
//
//    // MySQL 핸들러 초기화
//    conn = mysql_init(NULL);
//
//    // MySQL 서버 연결 정보
//    const char* server = "127.0.0.1";  // MySQL 서버 주소
//    const char* user = "root";         // MySQL 사용자 이름
//    const char* password = "000930"; // MySQL 사용자 비밀번호
//    const char* database = "testdb";   // 연결할 데이터베이스 이름
//    unsigned int port = 3306;          // MySQL 포트 번호
//
//    // MySQL 서버에 연결
//    if (conn == NULL) {
//        std::cerr << "mysql_init() failed\n";
//        return EXIT_FAILURE;
//    }
//
//    if (mysql_real_connect(conn, server, user, password, database, port, NULL, 0) == NULL) {
//        std::cerr << "mysql_real_connect() failed\n";
//        std::cerr << "Error: " << mysql_error(conn) << std::endl;
//        mysql_close(conn);
//        return EXIT_FAILURE;
//    }
//
//    std::cout << "Successfully connected to MySQL server!" << std::endl;
//
//    // 테이블 생성 (만약 존재하지 않는다면)
//    if (mysql_query(conn, "CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), age INT)")) {
//        finish_with_error(conn);
//    }
//
//    // 데이터 삽입
//    if (mysql_query(conn, "INSERT INTO users (name, age) VALUES('John Doe', 30)")) {
//        finish_with_error(conn);
//    }
//
//    // 데이터 조회
//    if (mysql_query(conn, "SELECT * FROM users")) {
//        finish_with_error(conn);
//    }
//
//    MYSQL_RES* result = mysql_store_result(conn);
//    if (result == NULL) {
//        finish_with_error(conn);
//    }
//
//    int num_fields = mysql_num_fields(result);
//    MYSQL_ROW row;
//
//    std::cout << "ID | Name      | Age\n";
//    std::cout << "----------------------\n";
//
//    while ((row = mysql_fetch_row(result))) {
//        for (int i = 0; i < num_fields; i++) {
//            std::cout << (row[i] ? row[i] : "NULL") << " | ";
//        }
//        std::cout << std::endl;
//    }
//
//    mysql_free_result(result);
//
//    // 데이터 업데이트
//    if (mysql_query(conn, "UPDATE users SET age = 35 WHERE name = 'John Doe'")) {
//        finish_with_error(conn);
//    }
//
//    // 데이터 삭제
//    /*if (mysql_query(conn, "DELETE FROM users WHERE name = 'John Doe'")) {
//        finish_with_error(conn);
//    }*/
//
//    // MySQL 연결 종료
//    mysql_close(conn);
//    std::cout << "CRUD 작업이 완료되었습니다." << std::endl;
//
//    return EXIT_SUCCESS;
//}
//
////
////// MySQL 관련 설정 및 초기화
////const char* DB_HOST = "127.0.0.1:3306";
////const char* DB_NAME = "testdb";
////const char* DB_USER = "root";
////const char* DB_PASS = "000930";
////
//////int main() {
//////    MYSQL* conn;
//////    MYSQL_RES* res;
//////    MYSQL_ROW row;
//////
//////    const char* server = "localhost";
//////    const char* user = "root";
//////    const char* password = "000930";  // MySQL root 계정 비밀번호
//////    const char* database = "testdb";  // 사용할 데이터베이스 이름
//////
//////    conn = mysql_init(NULL);
//////
//////    // MySQL 연결
//////    if (!mysql_real_connect(conn, server, user, password, database, 3306, NULL, 0)) {
//////        std::cerr << "MySQL 연결 실패: " << mysql_error(conn) << std::endl;
//////        return EXIT_FAILURE;
//////    }
//////
//////    // 데이터 삽입
//////    if (mysql_query(conn, "INSERT INTO test_table (name, age) VALUES('John', 25)")) {
//////        std::cerr << "INSERT 실패: " << mysql_error(conn) << std::endl;
//////        return EXIT_FAILURE;
//////    }
//////
//////    // 데이터 조회
//////    if (mysql_query(conn, "SELECT * FROM test_table")) {
//////        std::cerr << "SELECT 실패: " << mysql_error(conn) << std::endl;
//////        return EXIT_FAILURE;
//////    }
//////
//////    res = mysql_store_result(conn);
//////    int num_fields = mysql_num_fields(res);
//////
//////    while ((row = mysql_fetch_row(res))) {
//////        for (int i = 0; i < num_fields; i++) {
//////            std::cout << (row[i] ? row[i] : "NULL") << " ";
//////        }
//////        std::cout << std::endl;
//////    }
//////
//////    // 리소스 정리
//////    mysql_free_result(res);
//////    mysql_close(conn);
//////
//////    return EXIT_SUCCESS;
//////}
////
////
////class mysql_fiber : public acl::fiber
////{
////public:
////    mysql_fiber(acl::db_pool& dbpool) : dbpool_(dbpool) {}
////    ~mysql_fiber() {}
////
////protected:
////    void run(void) override
////    {
////        acl::db_handle* db = dbpool_.peek_open();
////        if (!db) {
////            std::cerr << "DB Connection Failed!" << std::endl;
////            return;
////        }
////
////        // Create Table
////        const char* create_table_sql = "CREATE TABLE IF NOT EXISTS users ("
////            "id INT AUTO_INCREMENT PRIMARY KEY, "
////            "name VARCHAR(100), "
////            "age INT);";
////        db->sql_update(create_table_sql);
////
////        // Insert Data
////        const char* insert_sql = "INSERT INTO users (name, age) VALUES ('John Doe', 30);";
////        db->sql_update(insert_sql);
////
////        // Select Data
////        const char* select_sql = "SELECT * FROM users;";
////        if (db->sql_select(select_sql)) {
////            const acl::db_rows* result = db->get_result();
////            const auto& rows = result->get_rows();
////            for (size_t i = 0; i < rows.size(); ++i) {
////                const acl::db_row* row = rows[i];
////                std::cout << "ID: " << (*row)["id"] << ", Name: " << (*row)["name"] << ", Age: " << (*row)["age"] << std::endl;
////            }
////            db->free_result();
////        }
////
////        // Update Data
////        const char* update_sql = "UPDATE users SET age = 31 WHERE name = 'John Doe';";
////        db->sql_update(update_sql);
////
////        // Delete Data
////        const char* delete_sql = "DELETE FROM users WHERE name = 'John Doe';";
////        db->sql_update(delete_sql);
////
////        dbpool_.put(db);
////
////        // 코루틴 종료
////        acl::fiber::schedule_stop();
////    }
////
////private:
////    acl::db_pool& dbpool_;
////};
////
////int main(int argc, char* argv[])
////{
////    SetConsoleOutputCP(CP_UTF8);
////
////    // 라이브러리 초기화
////    acl::acl_cpp_init();
////    acl::log::stdout_open(true);
////
////    // MySQL 연결 설정
////    acl::mysql_conf dbconf(DB_HOST, DB_NAME);
////    dbconf.set_dbuser(DB_USER).set_dbpass(DB_PASS).set_dblimit(2).set_charset("utf8");
////
////    // MySQL 연결 풀 생성
////    acl::mysql_pool dbpool(dbconf);
////
////    // 코루틴 시작
////    acl::fiber* f = new mysql_fiber(dbpool);
////    f->start();
////
////    // 스케줄러 실행
////    acl::fiber::schedule();
////
////    return 0;
////}
