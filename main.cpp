#include <wx/wx.h>
#include <vector>
#include <string>
#include <regex>
#include <ctime>
#include <fstream>
#include <sstream>

// Global data to keep track of users, passwords, and security questions
// TEST BRANCH

struct UserInfo {
    std::string username;
    std::string password;
    std::string securityQuestion;
    std::string securityAnswer;
    int entryCount;
    std::string lastSignIn;
};

std::vector<UserInfo> users;

const std::string USERS_FILE = "users.txt";

void LoadUsers() {
    std::ifstream file(USERS_FILE);
    if (!file.is_open()) {
        wxMessageBox("Could not open user data file. A new file will be created on save.", "Info", wxOK | wxICON_INFORMATION);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string username, password, securityQuestion, securityAnswer, lastSignIn;
        int entryCount;

        std::getline(ss, username, ',');
        std::getline(ss, password, ',');
        std::getline(ss, securityQuestion, ',');
        std::getline(ss, securityAnswer, ',');
        ss >> entryCount;
        ss.ignore(1, ','); // Skip the comma after the integer
        std::getline(ss, lastSignIn);

        if (!username.empty()) {
            users.push_back({ username, password, securityQuestion, securityAnswer, entryCount, lastSignIn });
        }
    }
    file.close();
}

void SaveUsers() {
    std::ofstream file(USERS_FILE, std::ios::trunc);
    if (!file.is_open()) {
        wxMessageBox("Could not open user data file for saving.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    for (const auto& user : users) {
        file << user.username << ","
            << user.password << ","
            << user.securityQuestion << ","
            << user.securityAnswer << ","
            << user.entryCount << ","
            << user.lastSignIn << "\n";
    }
    file.close();
}

std::string GetCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d", ltm);
    return std::string(buffer);
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
    frame->SetBackgroundColour(wxColour(221, 160, 221)); // Light purple color similar to Visual Studio
    frame->Show(true);
    return true;
}

UserFrame::UserFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 600)),
    userListBox(nullptr), btnLogin(nullptr), btnAddUser(nullptr), btnDeleteUser(nullptr), btnForgotPassword(nullptr), userInfoText(nullptr), btnCloseApp(nullptr) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // Instruction text
    wxStaticText* instructions = new wxStaticText(panel, wxID_ANY, "Welcome to your Personal Journal!\n\nPlease select an action below:\n- Log In: To access your journal entries.\n- Add User: To create a new journal profile.\n- Delete User: To remove an existing user.\n- Forgot Password: To recover your password.");
    instructions->Wrap(480);
    vbox->Add(instructions, 0, wxALL, 10);

    // Bug disclaimer
    wxStaticText* bugDisclaimer = new wxStaticText(panel, wxID_ANY, "If you encounter any bugs, please contact the developer at nthomasson22@yahoo.com.");
    bugDisclaimer->Wrap(480);
    vbox->Add(bugDisclaimer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    // User list box
    userListBox = new wxListBox(panel, ID_USER_LISTBOX);
    for (const auto& user : users) {
        userListBox->Append(user.username);
    }
    vbox->Add(userListBox, 1, wxEXPAND | wxALL, 10);

    // User info text
    userInfoText = new wxStaticText(panel, wxID_ANY, "");
    vbox->Add(userInfoText, 0, wxALL, 10);

    // Buttons for actions
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

        // Ask the security question
        wxString securityQuestion = wxString::FromUTF8(users[selection].securityQuestion.c_str());
        wxTextEntryDialog securityAnswerDialog(this, "Security question for " + username + ": " + securityQuestion, "Security Check");
        if (securityAnswerDialog.ShowModal() != wxID_OK || securityAnswerDialog.GetValue().ToStdString() != users[selection].securityAnswer) {
            wxMessageBox("Incorrect answer to the security question. Please try again.", "Login Failed", wxOK | wxICON_ERROR);
            return;
        }

        // Ask for password
        wxTextEntryDialog passwordDialog(this, "Enter password for " + username + ":", "Log In", "", wxTextEntryDialogStyle | wxTE_PASSWORD);
        if (passwordDialog.ShowModal() == wxID_OK) {
            wxString enteredPassword = passwordDialog.GetValue();

            // Verify username and password
            if (users[selection].password == enteredPassword.ToStdString()) {
                users[selection].lastSignIn = GetCurrentDate();
                wxMessageBox("Welcome, " + username + "!", "Login Successful", wxOK | wxICON_INFORMATION);
                SetTitle(username + "'s Personal Journal");
                UpdateUserInfo();
            }
            else {
                wxMessageBox("Incorrect password. Please try again.", "Login Failed", wxOK | wxICON_ERROR);
            }
        }
    }
    else {
        wxMessageBox("Please select a user to log in.", "Error", wxOK | wxICON_ERROR);
    }
}

void UserFrame::OnAddUser(wxCommandEvent& event) {
    wxTextEntryDialog usernameDialog(this, "Enter new username:", "Add User");
    if (usernameDialog.ShowModal() == wxID_OK) {
        wxString newUser = usernameDialog.GetValue();

        // Check if username contains disallowed characters or spaces
        std::regex usernameRegex("^[a-zA-Z0-9_]+$");
        if (!std::regex_match(newUser.ToStdString(), usernameRegex)) {
            wxMessageBox("Username cannot contain spaces or special characters.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        // Check if user already exists
        for (const auto& user : users) {
            if (user.username == newUser.ToStdString()) {
                wxMessageBox("Username already exists.", "Error", wxOK | wxICON_ERROR);
                return;
            }
        }

        // Ask for password
        wxTextEntryDialog passwordDialog(this, "Enter password:", "Add User", "", wxTextEntryDialogStyle | wxTE_PASSWORD);
        if (passwordDialog.ShowModal() == wxID_OK) {
            wxString newPassword = passwordDialog.GetValue();
            if (!newPassword.IsEmpty()) {
                // Provide security question options
                wxArrayString securityQuestions;
                securityQuestions.Add("What is your mother's maiden name?");
                securityQuestions.Add("What was the name of your first pet?");
                securityQuestions.Add("What is the name of the town where you were born?");
                securityQuestions.Add("What was your high school mascot?");

                wxSingleChoiceDialog questionDialog(this, "Select a security question for account recovery:", "Add User", securityQuestions);
                if (questionDialog.ShowModal() == wxID_OK) {
                    wxString selectedQuestion = questionDialog.GetStringSelection();
                    wxTextEntryDialog securityAnswerDialog(this, "Enter the answer to your selected security question:", "Add User");
                    if (securityAnswerDialog.ShowModal() == wxID_OK) {
                        wxString securityAnswer = securityAnswerDialog.GetValue();
                        users.push_back({ newUser.ToStdString(), newPassword.ToStdString(), selectedQuestion.ToStdString(), securityAnswer.ToStdString(), 0, "Never" });
                        userListBox->Append(newUser);
                        UpdateUserInfo();
                    }
                    else {
                        wxMessageBox("Security answer cannot be empty.", "Error", wxOK | wxICON_ERROR);
                    }
                }
            }
            else {
                wxMessageBox("Password cannot be empty.", "Error", wxOK | wxICON_ERROR);
            }
        }
    }
}

void UserFrame::OnDeleteUser(wxCommandEvent& event) {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        users.erase(users.begin() + selection);
        userListBox->Delete(selection);
        UpdateUserInfo();
    }
    else {
        wxMessageBox("Please select a user to delete.", "Error", wxOK | wxICON_ERROR);
    }
}

void UserFrame::OnForgotPassword(wxCommandEvent& event) {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        wxString username = userListBox->GetString(selection);

        // Ask the security question
        wxString securityQuestion = wxString::FromUTF8(users[selection].securityQuestion.c_str());
        wxTextEntryDialog securityAnswerDialog(this, "Security question for " + username + ": " + securityQuestion, "Forgot Password");
        if (securityAnswerDialog.ShowModal() == wxID_OK) {
            wxString enteredAnswer = securityAnswerDialog.GetValue();
            if (enteredAnswer.ToStdString() == users[selection].securityAnswer) {
                wxMessageBox("Your password is: " + wxString::FromUTF8(users[selection].password.c_str()), "Password Recovery", wxOK | wxICON_INFORMATION);
            }
            else {
                wxMessageBox("Incorrect answer to the security question. Please try again.", "Password Recovery Failed", wxOK | wxICON_ERROR);
            }
        }
    }
    else {
        wxMessageBox("Please select a user to recover the password.", "Error", wxOK | wxICON_ERROR);
    }
}

void UserFrame::OnUserSelection(wxCommandEvent& event) {
    UpdateUserInfo();
}

void UserFrame::OnCloseApp(wxCommandEvent& event) {
    wxMessageBox("Goodbye for now!", "Closing Application", wxOK | wxICON_INFORMATION);
    SaveUsers();
    Close(true);
}

void UserFrame::UpdateUserInfo() {
    int selection = userListBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        const UserInfo& user = users[selection];
        wxString info = wxString::Format("Entries: %d\nLast Sign-In: %s", user.entryCount, user.lastSignIn);
        userInfoText->SetLabel(info);
    }
    else {
        userInfoText->SetLabel("");
    }
}
