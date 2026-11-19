#pragma once
#include <sqlite3.h>
#include <string>
#include <stdexcept>

namespace eims {
class Database {
public:
    explicit Database(const std::string& path);
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    sqlite3* handle() const noexcept { return db_; }
    void initializeSchema();
private:
    sqlite3* db_{nullptr};
};

class Statement {
public:
    Statement(sqlite3* db, const std::string& sql);
    ~Statement();
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    sqlite3_stmt* get() const noexcept { return stmt_; }
    void bind(int index, const std::string& value);
    void bind(int index, int value);
    void bind(int index, double value);
    bool step();
    void execute();
private:
    sqlite3_stmt* stmt_{nullptr};
};
}
