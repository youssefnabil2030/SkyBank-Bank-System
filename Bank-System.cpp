#include <iostream>
#include <mysql/mysql.h>
#include <thread>
#include <mutex>
#include <string>
using namespace std;

// MySQL Connection Mutex
mutex dbMutex;

// Function to connect to the MySQL database
MYSQL* connectDatabase() {
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        cerr << "MySQL initialization failed!" << endl;
        exit(1);
    }

    // Connect to the database
    if (!mysql_real_connect(conn, "localhost", "root", "password", "bank_system", 3306, NULL, 0)) {
        cerr << "Connection Failed: " << mysql_error(conn) << endl;
        exit(1);
    }

    return conn;
}

// Create Account
void createAccount(MYSQL* conn, const string& name, double balance) {
    lock_guard<mutex> lock(dbMutex); // Thread-safe access
    string query = "INSERT INTO accounts (name, balance) VALUES ('" + name + "', " + to_string(balance) + ")";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error creating account: " << mysql_error(conn) << endl;
    } else {
        cout << "Account created successfully!" << endl;
    }
}

// Deposit Amount
void deposit(MYSQL* conn, int accountID, double amount) {
    lock_guard<mutex> lock(dbMutex);
    string query = "UPDATE accounts SET balance = balance + " + to_string(amount) + " WHERE id = " + to_string(accountID);
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error depositing money: " << mysql_error(conn) << endl;
    } else {
        cout << "Deposited " << amount << " successfully!" << endl;
    }
}

// Display Account
void displayAccount(MYSQL* conn, int accountID) {
    lock_guard<mutex> lock(dbMutex);
    string query = "SELECT name, balance FROM accounts WHERE id = " + to_string(accountID);
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching account details: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res == NULL) {
        cerr << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(res))) {
        cout << "Account Holder: " << row[0] << "\nBalance: " << row[1] << endl;
    } else {
        cout << "Account not found!" << endl;
    }
    mysql_free_result(res);
}

// Main Menu
void handleMenu(MYSQL* conn) {
    int choice;
    while (true) {
        cout << "\n=== Banking System ===\n";
        cout << "1. Create Account\n2. Deposit\n3. Display Account\n4. Exit\nEnter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string name;
            double balance;
            cout << "Enter Account Holder Name: ";
            cin >> name;
            cout << "Enter Initial Balance: ";
            cin >> balance;
            createAccount(conn, name, balance);
        } else if (choice == 2) {
            int accountID;
            double amount;
            cout << "Enter Account ID: ";
            cin >> accountID;
            cout << "Enter Deposit Amount: ";
            cin >> amount;
            deposit(conn, accountID, amount);
        } else if (choice == 3) {
            int accountID;
            cout << "Enter Account ID: ";
            cin >> accountID;
            displayAccount(conn, accountID);
        } else if (choice == 4) {
            cout << "Exiting..." << endl;
            break;
        } else {
            cout << "Invalid choice!" << endl;
        }
    }
}

// Main Function
int main() {
    MYSQL* conn = connectDatabase();

    // Handle multiple clients using threads (simulate by invoking menu in a thread)
    thread client1(handleMenu, conn);
    thread client2(handleMenu, conn);

    client1.join();
    client2.join();

    mysql_close(conn);
    return 0;
}
