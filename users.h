#include <vector>
#include <string>

using namespace std;

class User{
    private:
        bool log_status;
        char name[20];
        vector < pair <int,string> > recommendations;
    public:
        User();
        bool isLogged();
        char *registerUser(char username[20], char password[20]);
        char *deleteUser();
        char *loginUser(char username[20], char password[20]);
        char *logoutUser();
        char *getName();
        char *searchAuthor(char author[100]);

        void boost();
        char *recommend();
};