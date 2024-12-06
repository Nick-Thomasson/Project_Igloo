#ifndef USERINFO_H
#define USERINFO_H

#include <string>
using namespace std;

class UserInfo
{
private:
    string username;
    string password;
    string securityQuestion;
    string securityAnswer;
    string lastSignIn;
    int entryCount;

public:
    // Getters
    string GetUsername() const;
    string GetPassword() const;
    string GetSecurityQuestion() const;
    string GetSecurityAnswer() const;
    string GetLastSignIn() const;
    int GetEntryCount() const;

    // Setters
    void SetUsername(const string& newUsername);
    void SetPassword(const string& newPassword);
    void SetSecurityQuestion(const string& newSecurityQuestion);
    void SetSecurityAnswer(const string& newSecurityAnswer);
    void SetLastSignIn(const string& newLastSignIn);
    void SetEntryCount(int newEntryCount);
};

#endif
