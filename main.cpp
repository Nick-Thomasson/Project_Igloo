#include <wx/wx.h>
#include <vector>
#include <string>
#include <regex>
#include <ctime>
#include <fstream>
#include <sstream>
#include "LoadUsers.h"
#include "SaveUsers.h"
#include "UserInfo.h"
#include <iostream>
#include <pqxx/pqxx>

using namespace pqxx;
using namespace std;

// Global data
vector<UserInfo> users;
void LoadUsers();
void SaveUsers(); 




static string GetCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d", ltm);
    return string(buffer);
}

class UserApp : public wxApp {
public:
    virtual bool OnInit();
};

class UserFrame : public wxFrame {
public:
    UserFrame(const wxString& title);
    ~UserFrame();

private:
    wxButton* btnCloseApp;
    wxListBox* userListBox;
    wxButton* btnLogin;
    wxButton* btnAddUser;
    wxButton* btnDeleteUser;
    wxButton* btnForgotPassword;
    wxStaticText* userInfoText;

    void OnLogin(wxCommandEvent& event);
    void OnAddUser(wxCommandEvent& event);
    void OnDeleteUser(wxCommandEvent& event);
    void OnForgotPassword(wxCommandEvent& event);
    void OnUserSelection(wxCommandEvent& event);
    void OnCloseApp(wxCommandEvent& event);
    void UpdateUserInfo();

    wxDECLARE_EVENT_TABLE();
};

enum {
    ID_USER_LISTBOX = 5,
    ID_BTN_LOGIN,
    ID_BTN_ADD_USER,
    ID_BTN_DELETE_USER,
    ID_BTN_FORGOT_PASSWORD,
    ID_BTN_CLOSE_APP
};

wxBEGIN_EVENT_TABLE(UserFrame, wxFrame)
EVT_BUTTON(ID_BTN_LOGIN, UserFrame::OnLogin)
EVT_BUTTON(ID_BTN_ADD_USER, UserFrame::OnAddUser)
EVT_BUTTON(ID_BTN_DELETE_USER, UserFrame::OnDeleteUser)
EVT_BUTTON(ID_BTN_FORGOT_PASSWORD, UserFrame::OnForgotPassword)
EVT_BUTTON(ID_BTN_CLOSE_APP, UserFrame::OnCloseApp)
EVT_LISTBOX(ID_USER_LISTBOX, UserFrame::OnUserSelection)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(UserApp);

bool UserApp::OnInit() {
    LoadUsers();
    UserFrame* frame = new UserFrame("Personal Journal");
    frame->SetBackgroundColour(wxColour(221, 160, 221));
    frame->Show(true);
    return true;
}

UserFrame::UserFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 600)),
    userListBox(nullptr), btnLogin(nullptr), btnAddUser(nullptr), btnDeleteUser(nullptr), btnForgotPassword(nullptr), userInfoText(nullptr), btnCloseApp(nullptr) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText* instructions = new wxStaticText(panel, wxID_ANY, "Welcome to your Personal Journal!");
    instructions->Wrap(480);
    vbox->Add(instructions, 0, wxALL, 10);

    userListBox = new wxListBox(panel, ID_USER_LISTBOX);
    for (const auto& user : users) {
        userListBox->Append(user.GetUsername());
    }
    vbox->Add(userListBox, 1, wxEXPAND | wxALL, 10);

    userInfoText = new wxStaticText(panel, wxID_ANY, "");
    vbox->Add(userInfoText, 0, wxALL, 10);

    btnLogin = new wxButton(panel, ID_BTN_LOGIN, "Log In");
    btnAddUser = new wxButton(panel, ID_BTN_ADD_USER, "Add User");
    btnDeleteUser = new wxButton(panel, ID_BTN_DELETE_USER, "Delete User");
    btnForgotPassword = new wxButton(panel, ID_BTN_FORGOT_PASSWORD, "Forgot Password");
    btnCloseApp = new wxButton(panel, ID_BTN_CLOSE_APP, "Close Program");

    vbox->Add(btnLogin, 0, wxEXPAND | wxALL, 5);
    vbox->Add(btnAddUser, 0, wxEXPAND | wxALL, 5);
    vbox->Add(btnDeleteUser, 0, wxEXPAND | wxALL, 5);
    vbox->Add(btnForgotPassword, 0, wxEXPAND | wxALL, 5);
    vbox->Add(btnCloseApp, 0, wxEXPAND | wxALL, 5);

    panel->SetSizer(vbox);
}

UserFrame::~UserFrame() {
    SaveUsers();
}

void UserFrame::OnLogin(wxCommandEvent& event) {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        wxString username = userListBox->GetString(selection);
        const UserInfo& user = users[selection];

        wxTextEntryDialog securityDialog(this, "Answer the security question:\n" + user.GetSecurityQuestion(), "Login");
        if (securityDialog.ShowModal() == wxID_OK) {
            if (securityDialog.GetValue().ToStdString() == user.GetSecurityAnswer()) {
                wxTextEntryDialog passwordDialog(this, "Enter your password:", "Login", "", wxTextEntryDialogStyle | wxTE_PASSWORD);
                if (passwordDialog.ShowModal() == wxID_OK && passwordDialog.GetValue().ToStdString() == user.GetPassword()) {
                    wxMessageBox("Welcome, " + username + "!", "Success", wxOK | wxICON_INFORMATION);
                    userListBox->SetSelection(wxNOT_FOUND);
                    return;
                }
            }
        }
        wxMessageBox("Login failed.", "Error", wxOK | wxICON_ERROR);
    }
}
void UserFrame::OnAddUser(wxCommandEvent& event) {
    wxTextEntryDialog usernameDialog(this, "Enter a username:", "Add User");
    if (usernameDialog.ShowModal() != wxID_OK) {
        return;
    }
    string username = usernameDialog.GetValue().ToStdString();

    wxTextEntryDialog passwordDialog(this, "Enter a password:", "Add User", "", wxTextEntryDialogStyle | wxTE_PASSWORD);
    if (passwordDialog.ShowModal() != wxID_OK) {
        return;
    }
    string password = passwordDialog.GetValue().ToStdString();

    wxTextEntryDialog questionDialog(this, "Enter a security question:", "Add User");
    if (questionDialog.ShowModal() != wxID_OK) {
        return;
    }
    string securityQuestion = questionDialog.GetValue().ToStdString();

    wxTextEntryDialog answerDialog(this, "Enter the answer to your security question:", "Add User");
    if (answerDialog.ShowModal() != wxID_OK) {
        return;
    }
    string securityAnswer = answerDialog.GetValue().ToStdString();

    UserInfo newUser;
    newUser.SetUsername(username);
    newUser.SetPassword(password);
    newUser.SetSecurityQuestion(securityQuestion);
    newUser.SetSecurityAnswer(securityAnswer);
    newUser.SetEntryCount(0);
    newUser.SetLastSignIn(""); // Use an empty string or a default value

    try {
        connection C("dbname=dfqugt45bplafp user=ueid349f444hpc password=p115197e8dec98b7fd9d5e32e93fbe0a6911b95fd5fd837d7614dd97d031cfe75 host=caij57unh724n3.cluster-czrs8kj4isg7.us-east-1.rds.amazonaws.com port=5432 sslmode=require");

        if (C.is_open()) {
            work W(C);

            string sql = "INSERT INTO users (username, password, security_question, security_answer, last_sign_in, entry_count) VALUES (" +
                W.quote(newUser.GetUsername()) + ", " +
                W.quote(newUser.GetPassword()) + ", " +
                W.quote(newUser.GetSecurityQuestion()) + ", " +
                W.quote(newUser.GetSecurityAnswer()) + ", " +
                "NULL, " +  // Use NULL for last_sign_in
                to_string(newUser.GetEntryCount()) + ");";
            W.exec(sql);
            W.commit();

            users.push_back(newUser);
            userListBox->Append(newUser.GetUsername());
            wxMessageBox("User added successfully.", "Success", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("Failed to connect to the database.", "Error", wxOK | wxICON_ERROR);
        }
    }
    catch (const exception& e) {
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
    }
}


void UserFrame::OnDeleteUser(wxCommandEvent& event) {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        string username = users[selection].GetUsername();

        int response = wxMessageBox("Are you sure you want to delete the user \"" + username + "\"?", "Confirm Deletion", wxYES_NO | wxICON_WARNING);
        if (response == wxYES) {
            try {
                connection C("dbname=dfqugt45bplafp user=ueid349f444hpc password=p115197e8dec98b7fd9d5e32e93fbe0a6911b95fd5fd837d7614dd97d031cfe75 host=caij57unh724n3.cluster-czrs8kj4isg7.us-east-1.rds.amazonaws.com port=5432 sslmode=require");

                if (C.is_open()) {
                    work W(C);

                    string sql = "DELETE FROM users WHERE username = " + W.quote(username) + ";";
                    W.exec(sql);
                    W.commit();

                    users.erase(users.begin() + selection);
                    userListBox->Delete(selection);
                    userInfoText->SetLabel("");

                    wxMessageBox("User deleted successfully.", "Success", wxOK | wxICON_INFORMATION);
                }
                else {
                    wxMessageBox("Failed to connect to the database.", "Error", wxOK | wxICON_ERROR);
                }
            }
            catch (const exception& e) {
                wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
            }
        }
    }
    else {
        wxMessageBox("Please select a user to delete.", "Error", wxOK | wxICON_ERROR);
    }
}

void UserFrame::OnForgotPassword(wxCommandEvent& event) {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        const UserInfo& user = users[selection];

        wxTextEntryDialog securityDialog(this, "Answer the security question:\n" + user.GetSecurityQuestion(), "Forgot Password");
        if (securityDialog.ShowModal() == wxID_OK) {
            if (securityDialog.GetValue().ToStdString() == user.GetSecurityAnswer()) {
                wxMessageBox("Your password is: " + user.GetPassword(), "Password Recovery", wxOK | wxICON_INFORMATION);
            }
            else {
                wxMessageBox("Incorrect answer to the security question.", "Error", wxOK | wxICON_ERROR);
            }
        }
    }
    else {
        wxMessageBox("Please select a user to recover the password for.", "Error", wxOK | wxICON_ERROR);
    }
}

void UserFrame::OnUserSelection(wxCommandEvent& event) {
    UpdateUserInfo();
}

void UserFrame::OnCloseApp(wxCommandEvent& event) {
    SaveUsers();
    Close(true);
}

void UserFrame::UpdateUserInfo() {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        const UserInfo& user = users[selection];
        userInfoText->SetLabel("Entries: " + to_string(user.GetEntryCount()) + "\nLast Sign-In: " + user.GetLastSignIn());
    }
    else {
        userInfoText->SetLabel("");
    }
}
