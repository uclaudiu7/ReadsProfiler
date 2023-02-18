# Online Library Client-Server Application with Recommendation System

  This project implements a client-server application for an online library with basic search and download functionalities. Users can search for books by genre, author, title, year of publication, ISBN, and rating. The server keeps track of user searches and downloads, and uses this data to provide personalized book recommendations based on the user's interests and activity. The recommendation system takes into account factors such as user tastes, preferences of similar users, and ratings of recommended books. The application is designed to improve its accuracy over time as more user data is collected. The code is implemented in C/C++, using the SQLite library to manage the database.

compile using: g++ server.cpp users.cpp books.cpp -l sqlite3 -lpthread -o server
               g++ client.cpp -o client
run using    : ./server
             : ./client
