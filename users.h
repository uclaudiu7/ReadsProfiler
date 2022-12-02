class User{
    private:
        bool log_status;
        char name[20];
    public:
        User();
        char *registerUser(char username[20], char password[20]);
        char *deleteUser();
        char *loginUser(char username[20], char password[20]);
        char *logoutUser();
        bool isLogged();
        char *getName();
};