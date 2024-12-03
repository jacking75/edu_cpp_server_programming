#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"
#include "mysql_functions.h"
#include <print>

int main()
{
    acl::acl_cpp_init();
    acl::log::stdout_open(true);

    const char* dbaddr = "127.0.0.1:3306";
    const char* dbname = "testdb";
    const char* dbuser = "root";
    const char* dbpass = "000930";

    acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);

    if (db.open() == false) {
        std::print("Failed to open database '{}'.\n", dbname);
        return 1;
    }

    std::print("Connected to database '{}'.\n", dbname);

    // 테이블 생성
    if (!tbl_create(db)) {
        return 1;
    }

    // 사용자 입력을 통해 CRUD 작업 선택
    char action;
    std::string name;
    int id, age;

    while (true) {
        std::print("\nChoose an action: [i]nsert, [s]elect, [u]pdate, [d]elete, [q]uit: ");
        std::cin >> action;

        switch (action) {
        case 'i':  // 데이터 삽입
            std::print("Enter name: ");
            std::cin >> name;
            std::print("Enter age: ");
            std::cin >> age;
            tbl_insert(db, name, age);
            break;

        case 's':  // 데이터 조회
            std::print("Select all data:\n");
            tbl_select(db);
            break;

        case 'u':  // 데이터 업데이트
            std::print("Enter ID to update: ");
            std::cin >> id;
            std::print("Enter new name: ");
            std::cin >> name;
            std::print("Enter new age: ");
            std::cin >> age;
            tbl_update(db, id, name, age);
            break;

        case 'd':  // 데이터 삭제
            std::print("Enter ID to delete: ");
            std::cin >> id;
            tbl_delete(db, id);
            break;

        case 'q':  // 종료
            std::print("Exiting...\n");
            return 0;

        default:
            std::print("Invalid action. Try again.\n");
            break;
        }
    }

    return 0;
}
