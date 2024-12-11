#include "SaveUsers.h"
#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <vector>
#include "UserInfo.h" 

using namespace pqxx;
using namespace std;


extern string DataBaseAddress;
extern vector<UserInfo> users; 

void SaveUsers() {
	try {
		connection C(DataBaseAddress);

		if (C.is_open()) {
			work W(C);

			for (const auto& user : users) {
				string sql = "INSERT INTO users (username, password, security_question, security_answer, last_sign_in, entry_count) VALUES (" +
					W.quote(user.GetUsername()) + ", " +
					W.quote(user.GetPassword()) + ", " +
					W.quote(user.GetSecurityQuestion()) + ", " +
					W.quote(user.GetSecurityAnswer()) + ", " +
					W.quote(user.GetLastSignIn()) + ", " +
					to_string(user.GetEntryCount()) +
					") ON CONFLICT (username) DO UPDATE SET " +
					"password = EXCLUDED.password, " +
					"security_question = EXCLUDED.security_question, " +
					"security_answer = EXCLUDED.security_answer, " +
					"last_sign_in = EXCLUDED.last_sign_in, " +
					"entry_count = EXCLUDED.entry_count;";
				W.exec(sql);
			}

			W.commit();
			cout << "Users saved to the database successfully." << endl;
		}
		else {
			cout << "Can't open the database." << endl;
		}
	}
	catch (const exception& e) {
		cerr << e.what() << endl;
	}
}