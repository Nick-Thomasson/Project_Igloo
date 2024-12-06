#include "UserInfo.h"

// Getters
string UserInfo::GetUsername() const {
    return username;
}
string UserInfo::GetPassword() const {
    return password;
}
string UserInfo::GetSecurityQuestion() const {
    return securityQuestion;
    //
}
string UserInfo::GetSecurityAnswer() const {
    return securityAnswer;
}
string UserInfo::GetLastSignIn() const {
    return lastSignIn;
}
int UserInfo::GetEntryCount() const {
    return entryCount;
}

// Setters
void UserInfo::SetUsername(const string& newUsername) {
    username = newUsername;
}
void UserInfo::SetPassword(const string& newPassword) {
    password = newPassword;
}
void UserInfo::SetSecurityQuestion(const string& newSecurityQuestion) {
    securityQuestion = newSecurityQuestion;
}
void UserInfo::SetSecurityAnswer(const string& newSecurityAnswer) {
    securityAnswer = newSecurityAnswer;
}
void UserInfo::SetLastSignIn(const string& newLastSignIn) {
    lastSignIn = newLastSignIn;
}
void UserInfo::SetEntryCount(int newEntryCount) {
    entryCount = newEntryCount;
}
