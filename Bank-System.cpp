#include <wx/wx.h>
#include <sqlite3.h>
#include <iostream>
#include <string>

using namespace std;

// Function to encrypt the database
void encryptDatabase(sqlite3* db) {
    string encryptionKey = "MyStrongKey123";
    string query = "PRAGMA key = '" + encryptionKey + "';";
    sqlite3_exec(db, query.c_str(), 0, 0, 0);
}

// Database Initialization
void initDatabase() {
    sqlite3* db;
    if (sqlite3_open("bank_encrypted.db", &db) == SQLITE_OK) {
        encryptDatabase(db);
        string createTable = "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, name TEXT, balance REAL);";
        sqlite3_exec(db, createTable.c_str(), 0, 0, 0);
    } else {
        cerr << "Error opening database!" << endl;
    }
    sqlite3_close(db);
}

// Function to Add Account
void addAccount(string name, double balance) {
    sqlite3* db;
    sqlite3_open("bank_encrypted.db", &db);
    encryptDatabase(db);

    string query = "INSERT INTO accounts (name, balance) VALUES ('" + name + "', " + to_string(balance) + ");";
    sqlite3_exec(db, query.c_str(), 0, 0, 0);

    sqlite3_close(db);
}

// GUI Class
class BankApp : public wxFrame {
public:
    wxTextCtrl *nameInput, *amountInput;
    wxButton *createBtn, *exitBtn;

    BankApp() : wxFrame(NULL, wxID_ANY, "SkyBank System", wxDefaultPosition, wxSize(400, 300)) {
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        wxStaticText* nameLabel = new wxStaticText(panel, wxID_ANY, "Account Holder Name:");
        vbox->Add(nameLabel, 0, wxLEFT | wxTOP, 10);
        nameInput = new wxTextCtrl(panel, wxID_ANY);
        vbox->Add(nameInput, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

        wxStaticText* amountLabel = new wxStaticText(panel, wxID_ANY, "Initial Deposit:");
        vbox->Add(amountLabel, 0, wxLEFT | wxTOP, 10);
        amountInput = new wxTextCtrl(panel, wxID_ANY);
        vbox->Add(amountInput, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

        createBtn = new wxButton(panel, wxID_ANY, "Create Account");
        vbox->Add(createBtn, 0, wxALIGN_CENTER | wxTOP, 15);
        createBtn->Bind(wxEVT_BUTTON, &BankApp::OnCreate, this);

        exitBtn = new wxButton(panel, wxID_EXIT, "Exit");
        vbox->Add(exitBtn, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
        exitBtn->Bind(wxEVT_BUTTON, &BankApp::OnExit, this);

        panel->SetSizer(vbox);
        Centre();
    }

    void OnCreate(wxCommandEvent& event) {
        string name = string(nameInput->GetValue().mb_str());
        double amount = stod(string(amountInput->GetValue().mb_str()));
        addAccount(name, amount);
        wxMessageBox("Account Created Successfully!", "Success", wxOK | wxICON_INFORMATION);
    }

    void OnExit(wxCommandEvent& event) {
        Close(true);
    }
};

// Main Application
class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        initDatabase();
        BankApp* frame = new BankApp();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
