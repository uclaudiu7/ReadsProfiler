#pragma once
#include <vector>
#include <string>
#include "books.h"

using namespace std;

class User{
    private:
        bool log_status;
        bool view_flag;
        char name[20];
        vector < pair <int,string> > recommendations;
        vector < string > last_search;
        string current_book;
    public:
        User();
        bool isLogged();
        bool canView();

        string registerUser(char username[20], char password[20]);
        string deleteUser();
        string loginUser(char username[20], char password[20]);
        string logoutUser();
        string getName();
        string searchBook(Book b);
        string viewBook(int search_index);
        string recommend();

        void updateRec(Book b);
};