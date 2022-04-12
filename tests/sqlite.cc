#include <iostream>
#include <memory>
#include <string>

#include "sqlite/sqlite3.hpp"

int main()
{
    std::cout << "BEGIN SQLITE TEST" << std::endl;
    
    sqlite::database_manager sdb("sqlite_test.db", SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE);

    if (!sdb) {
        std::cout << "open database failure" << std::endl;
    }

    auto transaction = sdb.begin_transaction();

    if (!transaction) {
        std::cout << "begin_transaction failure" << std::endl;
    }

    if(transaction.exec("CREATE TABLE IF NOT EXISTS user(id INT, name VARCHAR, height DOUBLE)"))
    {
        std::cout << "CREATE TABLE SUCCESS" << std::endl;
    } 
    else 
    {
        std::cout << "CREATE TABLE FAILURE" << std::endl;
    }

    transaction.single_step("INSERT INTO user(id, name, height) VALUES(@id, @name, @height)", 1, "ZhanSan", 1.71);
    sdb.single_step("INSERT INTO user(id, name, height) VALUES(@id, @name, @height)", 2, std::string("LiSi"), 1.51);

    std::cout << "end_transaction" << std::endl;
    if (!transaction.end_transaction()) {
        std::cout << "end_transaction faiure" << std::endl;
    }

    auto stmt = sdb.query("user");

    while(stmt.step() == SQLITE_ROW)
    {
        auto count = stmt.column_count();
        for(size_t i = 0; i < count; ++i)
        {
            std::cout << stmt.column_name(i) << ":" << stmt.column_text(i) << " ";
        }
        std::cout << std::endl;
    }

    sdb.begin_transaction().exec("DELETE FROM user");

    return 0;
}
