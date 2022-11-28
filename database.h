#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>

class UserDatabase{
    public: 
        UserDatabase();
        static void openUserDatabase();
        static void closeUserDatabase();
        static int callback(void *NotUsed, int argc, char **argv, char **azColName);
        bool exists(char *username);
        bool login(char *username, char *password);
        bool insertUser(char *username, char *password);
    private:
        int userNumber;
};