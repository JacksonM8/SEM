#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "sqlite3.h"

class SQLiteDatabase{
    public:
        SQLiteDatabase(std::string databaseFilepath);
        virtual ~SQLiteDatabase();
        
        void QueueSqlStatement(sqlite3_stmt * statement);
        void Flush();
    protected:
        sqlite3 *database_ = 0;
    private:
        void ProcessQueue();

        std::thread* writer_thread_;

        std::queue<sqlite3_stmt*> sql_queue_;
        std::mutex queue_mutex_;
        std::condition_variable queue_lock_condition_;
        bool terminate_ = false;
};
#endif
