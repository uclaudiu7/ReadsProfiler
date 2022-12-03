#include <vector>
#include <string>

class User{
    private:
        bool log_status;
        char name[20];
    public:
        User();
        bool isLogged();
        char *registerUser(char username[20], char password[20]);
        char *deleteUser();
        char *loginUser(char username[20], char password[20]);
        char *logoutUser();
        char *getName();
        char *searchAuthor(char author[100]);
};