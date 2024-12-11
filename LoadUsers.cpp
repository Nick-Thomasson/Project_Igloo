#include "LoadUsers.h"
#include <pqxx/pqxx>
#include <iostream>
#include <vector>
#include "UserInfo.h"
#include <string>
// Assuming UserInfo is defined in a separate file
using namespace pqxx;
using namespace std;



extern vector<UserInfo> users; // Declare the external vector if needed

void LoadUsers() {
	try {
		connection C("dbname=dfqugt45bplafp user=ueid349f444hpc password=p115197e8dec98b7fd9d5e32e93fbe0a6911b95fd5fd837d7614dd97d031cfe75 host=caij57unh724n3.cluster-czrs8kj4isg7.us-east-1.rds.amazonaws.com port=5432 sslmode=require");

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
