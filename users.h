#pragma once
#include <vector>
#include <string>
#include "books.h"

using namespace std;

class User{
    private:
        bool log_status;
        bool download_flag;
        string view_type;
        string name;
        vector < pair <int,string> > recommendations;
        vector < string > last_search;
        vector < Book > downloads;
        Book last_view;
    public:
        User();
        User(string my_name);
        bool isLogged();

        string registerUser(char username[20], char password[20]);
        string deleteUser();
        string loginUser(char username[20], char password[20]);
        string logoutUser();
        string getName();
        string searchBook(Book b);
        string getViewType();
        string viewBook(int search_index);
        string viewRecommend(int recom_index);
        string downloadBook();
        string getDownloads();
        string recommend();

        void findSimilarBooks(Book b, int recom_strength);
        void findSimilarUsers();
        void updateRec(Book b, int recom_strength);
        void initializeDownloads();
        void initializeRecommendations();
};