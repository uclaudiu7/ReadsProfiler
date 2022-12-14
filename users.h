#pragma once
#include <vector>
#include <string>
#include "books.h"

using namespace std;

class User{
    private:
        bool log_status;
        bool view_flag;
        bool download_flag;
        string name;
        vector < pair <int,string> > recommendations;
        vector < string > last_search;
        vector < Book > downloads;
        Book last_view;
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
        string getLastView();
        string downloadBook();
        string getDownloads();
        string recommend();

        void updateRec(Book b);
        void initializeDownloads();
};