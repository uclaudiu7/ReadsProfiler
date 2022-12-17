#include <iostream>
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

Book::Book(string my_isbn){
    isbn = my_isbn;

    sqlite3 *myDatabase;
    int run = sqlite3_open("database.db", &myDatabase);
    if(run != SQLITE_OK){
        printf("SQL error open!\n");
    }

    sqlite3_stmt *stmt;
    string sql = "SELECT title, author, genre, subgenre, year, rating ";
    sql += "FROM books WHERE isbn = '" + my_isbn + "';";
    const char *sql_statement = const_cast<char*>(sql.c_str());
    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &stmt, NULL);
    if(run != SQLITE_OK){
        printf("SQL error prepare!\n");
    }

    if(sqlite3_step(stmt) == SQLITE_ROW){
        char *sql_title = (char*)sqlite3_column_text(stmt, 0);
        this->title = sql_title;

        char *sql_author = (char*)sqlite3_column_text(stmt, 1);
        this->author = sql_author;

        char *sql_genre = (char*)sqlite3_column_text(stmt, 2);
        this->genres = sql_genre;

        char *sql_subgenre = (char*)sqlite3_column_text(stmt, 3);
        this->subgenres = sql_subgenre;

        char *sql_year = (char*)sqlite3_column_text(stmt, 4);
        this->year = sql_year;

        char *sql_rating = (char*)sqlite3_column_text(stmt, 5);
        this->rating = sql_rating;
    }
    else
        printf("SQL error!\n");
    
    sqlite3_finalize(stmt);
    sqlite3_close(myDatabase);
    
}

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