#include "SaveUsers.h"
#include <pqxx/pqxx>
#include <iostream>
#include <string>
#include <vector>
#include "UserInfo.h" // Assuming UserInfo is defined in a separate file
using namespace pqxx;
using namespace std;

extern std::vector<UserInfo> users; // Declare the external vector if needed

 void SaveUsers() {
    try {
        connection C("dbname=dfqugt45bplafp user=ueid349f444hpc password=p115197e8dec98b7fd9d5e32e93fbe0a6911b95fd5fd837d7614dd97d031cfe75 host=caij57unh724n3.cluster-czrs8kj4isg7.us-east-1.rds.amazonaws.com port=5432 sslmode=require");

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