#include "LoadUsers.h"
#include <pqxx/pqxx>
#include <iostream>
#include <vector>
#include "UserInfo.h"
#include <string>

using namespace pqxx;
using namespace std;


extern string DataBaseAddress;
extern vector<UserInfo> users; // Declare the external vector if needed

void LoadUsers() {
	try {
		connection C(DataBaseAddress);

		if (C.is_open()) {
			cout << "Connected to the database successfully." << endl;

			work W(C);
			result R = W.exec("SELECT username, password, security_question, security_answer, last_sign_in, entry_count FROM users;");

			for (auto row : R) {
				UserInfo user;
				user.SetUsername(row["username"].c_str());
				user.SetPassword(row["password"].c_str());
				user.SetSecurityQuestion(row["security_question"].c_str());
				user.SetSecurityAnswer(row["security_answer"].c_str());
				user.SetLastSignIn(row["last_sign_in"].c_str());
				user.SetEntryCount(row["entry_count"].as<int>());

				users.push_back(user);
			}
		}
		else {
			cout << "Can't open the database." << endl;
		}
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
	}
}
