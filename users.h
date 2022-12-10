#pragma once
#include <vector>
#include <string>
#include "books.h"

using namespace std;

class User{
    private:
        bool log_status;
        char name[20];
        vector < pair <int,string> > recommendations;
    public:
        User();
        bool isLogged();
        const char *registerUser(char username[20], char password[20]);
        const char *deleteUser();
        const char *loginUser(char username[20], char password[20]);
        const char *logoutUser();
        const char *getName();
        const char *searchAuthor(char author[100]);

        void updateRec(Book b);
        const char *recommend();
};