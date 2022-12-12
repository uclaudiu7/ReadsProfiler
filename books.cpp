#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "books.h"

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

Book::Book() {}

Book::Book(string my_isbn) : isbn(my_isbn) {}

Book::Book(string my_isbn, string my_author, string my_title,
           string my_genres, string my_year, string my_rating)
    : isbn(my_isbn), author(my_author), title(my_title),
      genres(my_genres), year(my_year), rating(my_rating) {}

string Book::getISBN() { return this->isbn; }
string Book::getAuthor() { return this->author; }
string Book::getTitle() { return this->title; }
string Book::getGenres() { return this->genres; }
string Book::getYear() { return this->year; }
string Book::getRating() { return this->rating; }

bool Book::isEmpty(){ return this->isbn.empty(); }

void Book::print()
{
    printf("-->*%s*\n", isbn.c_str());
    printf("-->*%s*\n", author.c_str());
    printf("-->*%s*\n", title.c_str());
    printf("-->*%s*\n", genres.c_str());
    printf("-->*%s*\n", year.c_str());
    printf("-->*%s*\n", rating.c_str());
}