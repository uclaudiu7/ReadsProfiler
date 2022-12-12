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
        Book();
        Book(string my_isbn);
        Book(string my_isbn, string my_author, string my_title, 
             string my_genres, string my_year, string my_rating);

        string getISBN();
        string getAuthor();
        string getTitle();
        string getGenres();
        string getYear();
        string getRating();
        
        bool isEmpty();
        void print();
};