#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "users.h"
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

User::User(){ this->log_status = false; this->view_flag = false;}

string User::getName(){ return this->name; }

bool User::isLogged(){ return this->log_status; }

bool User::canView(){ return this->view_flag; }

string User::registerUser(char username[20], char password[20]){
    sqlite3* myDatabase;
    char *ErrMsg = 0;
    char sqlChar[100];
    char responseChar[200];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }
    
    strcpy(sqlChar, "INSERT INTO USERS VALUES (");
    strcat(sqlChar, "'");
    strcat(sqlChar, username);
    strcat(sqlChar, "', '");
    strcat(sqlChar, password);
    strcat(sqlChar, "');");

    char *sqlStatement = new char[100];
    strcpy(sqlStatement, sqlChar);

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        strcpy(responseChar, "User '");
        strcat(responseChar, username);
        strcat(responseChar, "' already exists. Try a different username!\n");
        
        fprintf(stderr, "SQL Error: %s\n", ErrMsg);
        printf("User '%s' already exists. Try a different username!\n", username);
        sqlite3_free(ErrMsg);
    }
    else{
        strcpy(responseChar, "User '");
        strcat(responseChar, username);
        strcat(responseChar, "' registered successfully!\n");
        printf("User '%s' registered successfully!\n", username);
        this->log_status = true;
        strcpy(this->name, username);
    }
    sqlite3_close(myDatabase);

    char *response = new char[strlen(responseChar)];
    strcpy(response, responseChar);
    printf("AICI: '%s'\n", response);
    return response;
}

string User::deleteUser(){
    if(log_status == false)
        return "You must be logged in first!\n";
    
    sqlite3* myDatabase;
    char *ErrMsg = 0;
    char sqlChar[100];
    char responseChar[200];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    strcpy(sqlChar, "DELETE FROM USERS WHERE USERNAME='");
    strcat(sqlChar, this->name);
    strcat(sqlChar, "';");

    char *sqlStatement = new char[100];
    strcpy(sqlStatement, sqlChar);    

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        return "SQL Error!\n";
        sqlite3_free(ErrMsg);
    }
    else{
        strcpy(responseChar, "User '");
        strcat(responseChar, this->name);
        strcat(responseChar, "' deleted from database!\n");
        this->log_status = false;
        //printf("User '%s' deleted from database!\n", username);
    }
    sqlite3_close(myDatabase);

    char *response = new char[strlen(responseChar)];
    strcpy(response, responseChar);
    return response;
}

string User::loginUser(char *username, char *password){
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    char *ErrMsg = 0;
    char sqlChar[100];
    char responseChar[200];
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    strcpy(sqlChar, "SELECT USERNAME FROM USERS WHERE USERNAME='");
    strcat(sqlChar, username);
    strcat(sqlChar, "' AND PASSWORD='");
    strcat(sqlChar, password);
    strcat(sqlChar, "';");
    char *sqlStatement = new char[100];
    strcpy(sqlStatement, sqlChar);    

    run = sqlite3_prepare_v2(myDatabase, sqlStatement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }

    run = sqlite3_step(s);
    if(run == SQLITE_ROW){
        //printf("Successfully connected as '%s'!\n", username);
        strcpy(responseChar, "Successfully connected as '");
        strcat(responseChar, username);
        strcat(responseChar, "'!\n");
        log_status = true;
        strcpy(this->name, username);
    }
    else{
        //printf("User '%s' not found. Please try loggin in again!\n", username);
        strcpy(responseChar, "Wrong credentials. Please try again!\n");  
    }
    sqlite3_finalize(s);
    sqlite3_close(myDatabase);

    char *response = new char[strlen(responseChar)];
    strcpy(response, responseChar);
    return response;
}   

string User::logoutUser(){
    this->log_status = false;
    this->view_flag = false;
    this->last_search.clear();
    this->recommendations.clear();
    return "Logged out!\n";
}

string createSqlStatement(Book b){
    string isbn = b.getISBN();
    string author = b.getAuthor();
    string title = b.getTitle();
    string genres = b.getGenres();
    string year = b.getYear();
    string rating = b.getRating();

    int ANDflag = 0;
    string sql = "SELECT isbn ||'#'|| title FROM books WHERE ";
    if(!isbn.empty()){
        ANDflag++;
        sql = sql + "isbn = '" + isbn + "' ";
    }
    if(ANDflag > 0) sql += "AND ";
    
    if(!author.empty()){
        ANDflag++;
        sql = sql + "author = '" + author + "' ";
    } 
    else ANDflag = 0;
    if(ANDflag > 0) sql += "AND ";

    if(!title.empty()){
        ANDflag++;
        sql = sql + "title LIKE '%" + title + "%' ";
    }
    else ANDflag = 0;
    if(ANDflag > 0) sql += "AND ";

    if(!genres.empty()){
        ANDflag++;
        sql = sql + "genres = '" + genres + "' ";
    }
    else ANDflag = 0;
    if(ANDflag > 0) sql += "AND ";

    if(!year.empty()){
        ANDflag++;
        sql = sql + "year = '" + year + "' ";
    }
    else ANDflag = 0;
    if(ANDflag > 0) sql += "AND ";

    if(!rating.empty()){
        ANDflag++;
        sql = sql + "rating = '" + rating + "' ";
    }
    else ANDflag = 0;
    if(ANDflag > 0) sql += "AND ";
    sql += "1=1;";

    return sql;
}

string User::searchBook(Book b){
    vector < string > query_result;
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        query_result.push_back("Couldn't open database!\n");
    }

    string str = createSqlStatement(b);
    cout << str << endl;
    const char *sql_statement = const_cast<char*>(str.c_str());

    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }

    while((run = sqlite3_step(s)) == SQLITE_ROW){
        char *line = (char*)sqlite3_column_text(s, 0);
        query_result.push_back(line);
    }

    if(run != SQLITE_DONE)
        printf("Error!\n");

    sqlite3_finalize(s);
    sqlite3_close(myDatabase);
    
    string result = "";
    if(query_result.size() == 0){
        return "Sorry! No books matched your search criteria!\n";
    }
    else{
        this->last_search = query_result;
        this->view_flag = true;
        result += "We found these books matching your search criteria:\n\n";
        for(int i = 0; i < query_result.size(); i++){
            string query_isbn = query_result[i].substr(0,13);
            result = result + "         ";
            result += to_string(i+1);
            result += ". ";
            result += query_result[i].substr(14);
            result += "\n";
        }
        result += "\n[server]--> To view a book type view 'index'!\n";
    }
    return result;
}

string User::viewBook(int search_index){
    if(search_index > last_search.size() || search_index < 1)
        return "Invalid index!\n";
    
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    char *ErrMsg = 0;
    int run;
    string book_isbn = last_search[search_index-1].substr(0, 13);

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    string sql = "SELECT isbn||'#'||title||'#'||author||'#'||genre||'#'||";
    sql += "subgenre||'#'||year||'#'||rating ";
    sql += "FROM books WHERE isbn = '" + book_isbn + "';";

    const char *sql_statement = const_cast<char*>(sql.c_str());

    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    
    string book_info = "Here's some details about your selected book:\n\n         • ISBN:       ";
    while((run = sqlite3_step(s)) == SQLITE_ROW){
        char *line = (char*)sqlite3_column_text(s, 0);
        book_info += line;
    }

    if(run != SQLITE_DONE)
        printf("Error!\n");

    sqlite3_finalize(s);
    sqlite3_close(myDatabase);

    vector < string > titles = {"Title :     ", "Author :    ", "Year :      ", "Genres :    ", "Subgenres : ", "Rating :    "};
    size_t it = 0;
    int i = 0;
    while((it = book_info.find("#", it)) != string::npos){
        string replace_delim = "\n         • " + titles[i++];
        book_info.replace(it, 1, replace_delim);
        it += 8;
    }
    
    printf("*%s*\n", book_info.c_str());

    Book b(book_isbn);
    this->last_view = b;

    this->download_flag = true;
    return book_info;
}

string User::getLastView(){
    return this->last_view.getISBN();
}

string User::downloadBook(){
    if(last_view.isEmpty())
        return "You must view a book first!\n";

    downloads.push_back(last_view);
    return "Downloaded your last viewed book! You can use 'downloads' to view your downloaded books!\n";
}

string User::getDownloads(){
    string result = "You've downloaded these books so far: \n";
    for(int i = 0; i < downloads.size(); i++){
        result += "\n         • ";
        result += downloads[i].getISBN();
    }
    return result;
}


string User::recommend(){
    if(recommendations.size() == 0)
        return "You have no activity. We can't recommend you anything yet!\n";
    
    //sort(recommendations.begin(), recommendations.end(), greater<int>());

    char charResult[500];
    strcpy(charResult, "Here are some books you might like:\n\n");
    for(int i = 0; i < recommendations.size(); i++){
        char index[15];
        sprintf(index, "         %d. ", i+1);
        strcat(charResult, index);
        strcat(charResult, recommendations[i].second.c_str());
        strcat(charResult, "\n");
    }
    strcat(charResult, "\n[server]--> To view a book type view 'index'!\n");

    char *result = new char[500];
    strcpy(result, charResult);
    return result;
}

void User::updateRec(Book b){
    for(int i = 0; i < recommendations.size(); i++){
        if( recommendations[i].second == b.getISBN() ){
            recommendations[i].first++;
            return;
        }
    }
    recommendations.push_back(make_pair(1, b.getISBN()));
}
