#include <iostream>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "users.h"
#include "books.h"

using namespace std;

#define PORT 3002
#define MAX_THREADS 100

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl;       //descriptorul intors de accept
}thData;

sqlite3* myDatabase;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for (i = 0; i < argc; i++){ 
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void addDefaultBooks(){
    /*### this function adds some default books to the databse ###*/

    char *ErrMsg = 0;
    const char *sqlStatement;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9781035003426', " \
                    "'Burial of Ghosts', " \
                    "'Ann Cleeves', " \
                    "'2022', 'Crime', 'Fiction', '8');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO BOOKS VALUES('9781509889624', " \
                    "'The Rising Tide', " \
                    "'Ann Cleeves', " \
                    "'2021', 'Mistery', 'Fiction', '8');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO BOOKS VALUES('9781529049602', " \
                    "'Date with Betrayal', " \
                    "'Julia Chapman', " \
                    "'2022', 'Crime', 'Mistery', '10');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO BOOKS VALUES('9781529097993', " \
                    "'The Cat Who Caught a Killer', " \
                    "'L. T. Shearer', " \
                    "'2020', 'Mistery', 'Crime', '9');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9780399588174', " \
                    "'Born a Crime', " \
                    "'Trevor Noah', " \
                    "'2016', 'Biography', 'Comedy', '6');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO BOOKS VALUES('9780747532743', " \
                    "'Harry Potter and the Philosopher`s Stone', " \
                    "'J. K. Rowling', " \
                    "'1997', 'Magic', 'Fiction', '10');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO BOOKS VALUES('9780747538493', " \
                    "'Harry Potter and the Chamber of Secrets', " \
                    "'J. K. Rowling', " \
                    "'1998', 'Magic', 'Fiction', '9');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9781408703991', " \
                    "'The Cuckoo`s Calling', " \
                    "'J. K. Rowling', " \
                    "'2013', 'Crime', 'Fiction', '7');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9780679745587', " \
                    "'In Cold Blood', " \
                    "'Truman Capote', " \
                    "'1995', 'Crime', 'Nonfiction', '8');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}
    
    sqlStatement = "INSERT INTO BOOKS VALUES('9781542005227', " \
                    "'If You Tell', " \
                    "'Gregg Olsen', " \
                    "'2019', 'Nonfiction', 'Biography', '6');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding book: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Book added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('J. K. Rowling', 'fiction, magic, crime');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('Truman Capote', 'crime, nonfiction');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('Gregg Olsen', 'nonfiction, biography');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('Ann Cleeves', 'crime, fiction, mistery');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('Julia Chapman', 'crime, mistery');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('L. T. Shearer', 'mistery, crime');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}

    sqlStatement = "INSERT INTO AUTHORS VALUES('Trevor Noah', 'comedy, biography');";
    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMsg);
    if(run != SQLITE_OK){
        fprintf(stderr, "Error while adding author: %s!\n", ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else{ printf("Author added!\n");}
    

    sqlite3_close(myDatabase);
}

void createTable(const char *sqlStatement, const char *tableName){
    /*### creating necessary tables in our databse ###*/
    int run;
    char *ErrMSg = 0;

    run = sqlite3_exec(myDatabase, sqlStatement, callback, 0, &ErrMSg);
    if(run != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", ErrMSg);
        sqlite3_free(ErrMSg);
    }
    else{
        printf("Table %s created successfully!\n", tableName);
    }
}

void createDatabase(){
    /*### if no database is found, we create one, else we open it ###*/
    FILE *exists = fopen("database.db", "r");
    if(exists){
        printf("Database already exists!\n");
        fclose(exists);
        return;
    }
    printf("Creating database...\n");
    const char *sqlStatement;
    int run;

    run = sqlite3_open("database.db", &myDatabase);
    if(run){
        fprintf(stderr, "Couldn't open database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }
    else{
        printf("Database opened successfully!\n");
    }    
    
    /*############## creating database tables ##############*/
    sqlStatement = "CREATE TABLE USERS(" \
                 "USERNAME TEXT PRIMARY KEY NOT NULL," \
                 "PASSWORD TEXT NOT NULL);";
    createTable(sqlStatement, "USERS");

    sqlStatement = "CREATE TABLE BOOKS(" \
                 "ISBN TEXT PRIMARY KEY NOT NULL," \
                 "TITLE TEXT NOT NULL, " \
                 "AUTHOR TEXT NOT NULL, " \
                 "GENRE TEXT NOT NULL, " \
                 "SUBGENRE TEXT NOT NULL, " \
                 "YEAR TEXT NOT NULL, " \
                 "RATING NUMERIC NOT NULL);";
    createTable(sqlStatement, "BOOKS");

    sqlStatement = "CREATE TABLE DOWNLOADS(" \
                 "DOWNLOAD_ID INTEGER PRIMARY KEY NOT NULL, " \
                 "USERNAME TEXT NOT NULL, " \
                 "ISBN TEXT NOT NULL);";
    createTable(sqlStatement, "DOWNLOADS");

    sqlStatement = "CREATE TABLE RECOMMENDATIONS(" \
                 "RECOM_ID INTEGER PRIMARY KEY NOT NULL, " \
                 "USERNAME TEXT NOT NULL, " \
                 "RANK INTEGER NOT NULL, " \
                 "ISBN TEXT NOT NULL);";
    createTable(sqlStatement, "RECOMMENDATIONS");

    sqlStatement = "CREATE TABLE AUTHORS(" \
                 "NAME TEXT PRIMARY KEY NOT NULL, " \
                 "GENRES TEXT NOT NULL);";
    createTable(sqlStatement, "AUTHORS");

    addDefaultBooks();
}

string handleRegister(char command[100], User &u){
    /*###### check if command is register <user> <pass> <pass> ######*/
    /*######        check if pasword matches in database       ######*/

    char username[20] = "";
    char password[20] = "";
    char password2[20] = "";

    int fields = sscanf(command, "register %s %s %s", username, password, password2);
    if(fields == 3 && strcmp(password, password2) == 0)
        return u.registerUser(username, password);
    else
        return "Please register using following command: register 'username' 'password' 'password'!\n";
}

string handleDelete(User &u){
    if(u.isLogged() == false)
        return "You must login first!\n";
    return u.deleteUser();
}

string handleLogin(char command[100], User &u){
    if(u.isLogged())
        return "You are already logged in!\n";
    char username[100] = "";
    char password[100] = "";
    
    int fields = sscanf(command, "login %s %s", username, password);
    if(fields = 2)
        return u.loginUser(username, password);
    else
        return "Please login using: login <username> <password> !\n";
}

string handleLogout(User &u){
    if(u.isLogged() == false)
        return "You must login first!\n";
    return u.logoutUser();
}

string handleStatus(User &u){
    if(u.isLogged() == true)
        return "You are logged in as '" + u.getName() + "'!\n";
    else
        return "You are not logged in!\n";
}

string handleHelp(){
    return "Available commands: help, register, delete account, login, logout, status, search, download, view, stop.\n";
}

string handleSearch(char command[400], User &u){
    /*
    if(u.isLogged() == false)
        return "You must login first!\n";
    */
    char *token = strtok(command, "#");
    string isbn = token + 5;

    token = strtok(nullptr, "#");
    string author = token + 7;
    
    token = strtok(nullptr, "#");
    string title = token + 6;
    
    token = strtok(nullptr, "#");
    string genres = token + 7;
    
    token = strtok(nullptr, "#");
    string year = token + 5;
    
    token = strtok(nullptr, "#");
    string rating = token + 7;

    Book input_book(isbn, author, title, genres, year, rating);
    return u.searchBook(input_book);
}

string handleView(char command[400], User &u){
    if(u.getViewType() == "false")
        return "You have nothing to view!\n";
    int view_index;
    int fields = sscanf(command, "view %d", &view_index);
    if(fields != 1)
        return "Please select a book using following command: view 'index'!\n";
    if(u.getViewType() == "search")
        return u.viewBook(view_index);
    if(u.getViewType() == "recommend")
        return u.viewRecommend(view_index);
    return "";
}

string handleDownload(User &u){
    if(u.isLogged() == false)
        return "You must login first!\n";
    return u.downloadBook();
}

string handleDownloads(User &u){
    if(u.isLogged() == false)
        return "You must login first!\n";
    return u.getDownloads();
}

string handleRecommend(User &u){
    if(u.isLogged() == false)
        return "You must login first!\n";
    return u.recommend();
}

string handleAuthor(char command[400]){
    char author[100];
    int fields = sscanf(command, "author %[^\n]s", author);
    if(fields != 1)
        return "Please ask for information about an author using: author <name>!\n";
    
    sqlite3* myDatabase;
    sqlite3_stmt *s;
    int run;
    string name = author;
    string genres;
    string sql = "SELECT genres FROM AUTHORS WHERE name = '" + name + "';";
    const char *sql_statement = const_cast<char*>(sql.c_str());
    
    run = sqlite3_open("database.db", &myDatabase);
    if(run)
        return "SQL Error!\n";
    run = sqlite3_prepare_v2(myDatabase, sql_statement, -1, &s, NULL);
    if(run != SQLITE_OK)
        return "SQL Error!\n";
    while((run = sqlite3_step(s)) == SQLITE_ROW){
        char *line = (char*)sqlite3_column_text(s, 0);
        genres = line;
    }

    string response = name + " adresses these genres: " + genres + ".\n";
    return response;
}

string handleCommand(char command[400], User &u){
    if(strncmp(command, "register", 8) == 0)
        return handleRegister(command, u);
    if(strncmp(command, "delete account", 14) == 0)
        return handleDelete(u);
    if(strncmp(command, "login", 5) == 0)
        return handleLogin(command, u);
    if(strncmp(command, "logout", 6) == 0)
        return handleLogout(u);
    if(strncmp(command, "status", 6) == 0)
        return handleStatus(u);
    if(strncmp(command, "stop", 4) == 0)
        return "stop";
    if(strncmp(command, "help", 4) == 0)
        return handleHelp();
    if(strncmp(command, "search", 6) == 0)
        return handleSearch(command+7, u);
    if(strncmp(command, "view", 4) == 0)
        return handleView(command, u);
    if(strncmp(command, "downloads", 9) == 0)
        return handleDownloads(u);
    if(strncmp(command, "download", 8) == 0)
        return handleDownload(u);
    if(strncmp(command, "author", 6) == 0)
        return handleAuthor(command);

    if(strncmp(command, "recommend", 9) == 0)
        return handleRecommend(u);

    return "Invalid command. Type 'help' to display available commands!\n";
}

void raspunde(void *arg){
    int nr, i=0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);
	char command[400];
    char response[400];
    User u;

    while(1){
        bzero(command, 400);

        if(read(tdL.cl, command, 400) <= 0){
            perror("[server]Eroare la read() de la client.\n");
        }

        printf("[client]--> %s\n", command);

        if(strncmp("stop", command, 4) == 0){
            write(tdL.cl, "Server stopped!\n", 16);
            break;
        }

        string handle_string = handleCommand(command, u);
        const char *temp = handle_string.c_str();
        strcpy(response, temp);

        write(tdL.cl, response, strlen(response));
        bzero(response, 400);
    }
}

static void *treat(void * arg){		
    struct thData tdL; 
    tdL= *((struct thData*)arg);	
    printf ("[new thread created]\n");
    fflush (stdout);		 
    pthread_detach(pthread_self());	
    raspunde((struct thData*)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close ((intptr_t)arg);
    return(NULL);  
};

int main(){
    struct sockaddr_in server;
    struct sockaddr_in from;
    int nr;
    int sd;
    int pid;
    pthread_t th[100];
    int i = 0;

    char command[400];
    char response[400];

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&server, sizeof(server));   // preparing data structures
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if(bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1){
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    if(listen(sd, MAX_THREADS) == -1){
        perror("[server]Eroare la listen().\n");
        return errno;
    }
    createDatabase();

    while(1){
        bzero(command, 400);
        int client;
        thData * td;
        socklen_t length = sizeof(from);

        if((client = accept(sd, (struct sockaddr *) &from, &length)) < 0){
            perror("Error accepting connection!\n");
            continue;
        }

        td = (struct thData*)malloc(sizeof(struct thData));
        td->idThread++;
        td->cl = client;

        pthread_create(&th[i], NULL, &treat, td);
    }

    return 0;
}