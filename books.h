#pragma once
#include <string>

using namespace std;

class Book{
    private:
        string isbn;
        string title;
        string author;
        string year;
        string genres;
        string rating;
    public:
        Book(const string &isbn, const string &author, const string &title,
             const string &genres, const string &year, const string &rating);
        
        char *insertBook();
        const char *getISBN();
        const char *getAuthor();
        const char *getTitle();
        const char *getGenres();
        const char *getYear();
        const char *getRating();
        void print();
};