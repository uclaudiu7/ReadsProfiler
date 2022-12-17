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

User::User(){
    this->log_status = false; 
    this->view_flag = false;
    initializeDownloads();
    initializeRecommendations();
}

void User::initializeDownloads(){
    this->downloads.clear();
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
    }

    string sql = "SELECT ISBN FROM DOWNLOADS WHERE USERNAME = '";
    sql = sql + this->getName() + "';";

    const char *sql_statement = const_cast<char*>(sql.c_str());

    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }

    while((run = sqlite3_step(s)) == SQLITE_ROW){
        char *line = (char*)sqlite3_column_text(s, 0);
        this->downloads.push_back(Book(line));
    }

    if(run != SQLITE_DONE)
        printf("Error!\n");

    sqlite3_finalize(s);
    sqlite3_close(myDatabase);
}

void User::initializeRecommendations(){
    this->recommendations.clear();
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
    }

    string sql = "SELECT RANK||'#'||ISBN FROM RECOMMENDATIONS WHERE USERNAME = '";
    sql = sql + this->getName() + "';";
    const char *sql_statement = const_cast<char*>(sql.c_str());

    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }

    while((run = sqlite3_step(s)) == SQLITE_ROW){
        char *temp = (char*)sqlite3_column_text(s, 0);
        string line = temp, recom_isbn;
        int rank, delim_pos = line.find("#");
        rank = stoi(line.substr(0, delim_pos));
        recom_isbn = line.substr(delim_pos + 1);

        this->recommendations.push_back(make_pair(rank, recom_isbn));
    }

    if(run != SQLITE_DONE)
        printf("Error!\n");

    sqlite3_finalize(s);
    sqlite3_close(myDatabase);
}

bool insertIntoTable(string sql){
    sqlite3* myDatabase;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    const char *sql_statement = const_cast<char*>(sql.c_str());

    run = sqlite3_exec(myDatabase, sql_statement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL Error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
        return false;
    }
    printf("da\n");
    sqlite3_close(myDatabase);
    return true;
}
 
string User::getName(){ return this->name; }

bool User::isLogged(){ return this->log_status; }

bool User::canView(){ return this->view_flag; }

string User::registerUser(char username[20], char password[20]){
    sqlite3* myDatabase;
    string sql, response;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }
    
    sql = "INSERT INTO USERS VALUES ('";
    sql = sql + username + "', '" + password + "');";
    const char *sql_statement = const_cast<char*>(sql.c_str());

    run = sqlite3_exec(myDatabase, sql_statement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        response = "User '";
        response = response + username + "' already exists. Try a different username!\n";
        
        fprintf(stderr, "SQL Error: %s\n", ErrMsg);
        printf("User '%s' already exists. Try a different username!\n", username);
        sqlite3_free(ErrMsg);
    }
    else{
        response = "User '";
        response = response + username + "' registered successfully!\n";
        
        printf("User '%s' registered successfully!\n", username);
        this->log_status = true;
        this->name = username;
    }
    sqlite3_close(myDatabase);

    return response;
}

string User::deleteUser(){
    if(log_status == false)
        return "You must be logged in first!\n";
    
    sqlite3* myDatabase;
    string sql, response;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    sql = "DELETE FROM USERS WHERE USERNAME = '";
    sql = sql + this->getName() + "';";

    const char *sql_statement = const_cast<char*>(sql.c_str());   

    run = sqlite3_exec(myDatabase, sql_statement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        return "SQL Error!\n";
        sqlite3_free(ErrMsg);
    }
    else{
        response = "User '";
        response = response + this->getName() + "' deleted from database!\n";
        this->log_status = false;
    }
    sqlite3_close(myDatabase);

    return response;
}

string User::loginUser(char *username, char *password){
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    string sql, response;
    char *ErrMsg = 0;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return "Couldn't open database!\n";
    }

    sql = "SELECT USERNAME FROM USERS WHERE USERNAME = '";
    sql = sql + username + "' AND PASSWORD = '" + password + "';";

    const char *sql_statement = const_cast<char*>(sql.c_str());    

    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }

    run = sqlite3_step(s);
    if(run == SQLITE_ROW){
        response = "Successfully connected as '";
        response = response + username + "'!\n";
        log_status = true;
        this->name = username;
        this->initializeDownloads();
        this->initializeRecommendations();
    }
    else{
        response = "Wrong credentials. Please try again!\n";
    }
    sqlite3_finalize(s);
    sqlite3_close(myDatabase);


    return response;
}   

string User::logoutUser(){
    this->log_status = false;
    this->view_flag = false;
    this->last_search.clear();
    this->recommendations.clear();
    this->downloads.clear();
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
            result += "📚 ";
            result += to_string(i+1);
            result += ". ";
            result += query_result[i].substr(14);
            result += "\n";

            Book b(query_isbn);
            updateRec(b, 1);
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

    Book b(book_isbn);
    string book_info = "\n#" + book_isbn + "#" + b.getTitle() + "#" +  b.getAuthor() + "#" + b.getYear() + "#" + b.getGenres() + "#" + b.getRating();
    vector < string > titles = {"ISBN :      ", "Title :     ", "Author :    ", "Year :      ", "Genres :    ", "Rating :    "};
    size_t it = 0;
    int i = 0;
    while((it = book_info.find("#", it)) != string::npos){
        string replace_delim = "\n         • " + titles[i++];
        book_info.replace(it, 1, replace_delim);
        it += 8;
    }

    recSimilarBooks(b, 3);
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

    string sql = "INSERT INTO DOWNLOADS(username, isbn) VALUES ('";
    sql = sql + this->getName() + "', '" + last_view.getISBN() + "');";

    if(insertIntoTable(sql) == true){
        recSimilarBooks(last_view, 6);
        return "Downloaded your last viewed book! You can use 'downloads' to view your downloaded books!\n";
    }
    else
        return "We've encountered an error. Please try again!\n";
}

string User::getDownloads(){
    if(downloads.size() == 0)
        return "You haven't downloaded a book yet!\n";
    
    string result = "You've downloaded these books so far: \n";
    for(int i = 0; i < downloads.size(); i++){
        result += "\n         • ";
        result += downloads[i].getISBN();
    }
    return result;
}

void User::recSimilarBooks(Book b, int recom_strength){
    string isbn = b.getISBN();
    string author = b.getAuthor();
    string genres = b.getGenres();
    vector < Book > similar_books;

    sqlite3* myDatabase;
    sqlite3_stmt *s;
    int run = sqlite3_open("database.db", &myDatabase);
    if(run != SQLITE_OK)
        return;

    string sql = "SELECT isbn FROM BOOKS WHERE author = '" + author;
    sql = sql + "' OR genre = '" + genres + "';";
    const char *sql_statement = const_cast<char*>(sql.c_str());

    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK)
        return;

    while((run = sqlite3_step(s)) == SQLITE_ROW){
        char *line = (char*)sqlite3_column_text(s, 0);
        string similar_book_isbn = line;
        Book b(similar_book_isbn);
        similar_books.push_back(b);
    }
    sqlite3_finalize(s);
    sqlite3_close(myDatabase);

    for(int i = 0; i < similar_books.size(); i++)
        updateRec(similar_books[i], recom_strength);
}

string User::recommend(){

    if(recommendations.size() == 0)
        return "You have no activity. We can't recommend you anything yet!\n";
    
    for(int i = 0; i < recommendations.size()-1; i++)
        for(int j = 1; j < recommendations.size(); j++)
            if(recommendations[i].first < recommendations[j].first)
                swap(recommendations[i], recommendations[j]);

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

void User::updateRec(Book b, int recom_strength){
    for(int i = 0; i < recommendations.size(); i++){
        if( recommendations[i].second == b.getISBN() ){
            recommendations[i].first += recom_strength;
            string sql = "UPDATE RECOMMENDATIONS SET rank=rank+" + to_string(recom_strength) + " WHERE username='";
            sql = sql + this->getName() + "' " + "AND isbn='" + b.getISBN() + "';";
            insertIntoTable(sql);
            return;
        }
    }
    string sql = "INSERT INTO RECOMMENDATIONS(username, rank, isbn) VALUES ('";
    sql = sql + this->getName() + "', '" + to_string(recom_strength) + "', '" + b.getISBN() + "');";
    insertIntoTable(sql);

    recommendations.push_back(make_pair(recom_strength, b.getISBN()));
}