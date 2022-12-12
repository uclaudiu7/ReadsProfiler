#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "books.h"

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

Book::Book(const string &isbn, const string &author, const string &title, 
           const string &genres, const string &year, const string &rating)
    : isbn(isbn), author(author), title(title), genres(genres), year(year), rating(rating){}

const char *Book::getISBN(){ return this->isbn.c_str(); }
const char *Book::getAuthor() { return this->author.c_str(); }
const char *Book::getTitle(){ return this->title.c_str(); }
const char *Book::getGenres(){ return this->genres.c_str(); }
const char *Book::getYear(){ return this->year.c_str(); }
const char *Book::getRating(){ return this->rating.c_str(); }

void Book::print(){
    printf("-->*%s*\n", isbn.c_str());
    printf("-->*%s*\n", author.c_str());
    printf("-->*%s*\n", title.c_str());
    printf("-->*%s*\n", genres.c_str());
    printf("-->*%s*\n", year.c_str());
    printf("-->*%s*\n", rating.c_str());
}