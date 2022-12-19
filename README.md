# ReadsProfiler

compile using: g++ server.cpp users.cpp books.cpp -l sqlite3 -lpthread -o server
               g++ client.cpp -o client
run using    : ./server
             : ./client
