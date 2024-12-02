#include <iostream>
#include <pqxx/pqxx> // Include the libpqxx library for PostgreSQL

using namespace std;
using namespace pqxx; // For convenience

int main() {
    try {
        // Connection details from the information you provided
        connection C("dbname=dfqugt45bplafp user=ueid349f444hpc password=p115197e8dec98b7fd9d5e32e93fbe0a6911b95fd5fd837d7614dd97d031cfe75 host=caij57unh724n3.cluster-czrs8kj4isg7.us-east-1.rds.amazonaws.com port=5432 sslmode=require");

        if (C.is_open()) {
            cout << "Connected to the database successfully." << endl;
        }
        else {
            cout << "Can't open the database." << endl;
            return 1;
        }

        // Get the user's name
        string name;
        cout << "What is your name? ";
        cin >> name;

        // Create a transactional object
        work W(C);

        // Insert the name into the "users" table using the "username" column
        string sql = "INSERT INTO users (username) VALUES (" + W.quote(name) + ");";
        W.exec(sql);
        W.commit();

        cout << "Name inserted into the database successfully." << endl;

        // No explicit disconnect needed
    }
    catch (const std::exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
