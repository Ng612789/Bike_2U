#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <limits>
#include <cctype>
using namespace std;

const string ADMIN_FILE = "admin.csv";
const string USER_FILE = "user.csv";
const int MIN_PWD = 8;

const string ADMIN_EMAIL = "admin@bike2u.com";
const string ADMIN_PWD = "Admin@123";

struct Account {
    int accountID;
    string name;
    string email;
    string phone;
    string password;
};

// Prototypes
void clearScreen();
void waitForEnter();
void Menu(Account& admin, vector<Account>& users);
void adminMenu(Account& admin);
void userMenu(vector<Account>& users, int currentIdx);

int validateOption(int minVal, int maxVal);
bool validateEmail(const string& email);
bool validatePassword(const string& password);
int  registerUser(vector<Account>& users);
bool emailExist(const vector<Account>& users, const string& email);
bool matchAdmin(const Account& admin, const string& email, const string& password);
int  matchUser(const vector<Account>& users, const string& email, const string& password);

void Admin_SaveFile(const Account& admin);
bool Admin_LoadFile(Account& admin);
void User_SaveFile(const vector<Account>& users);
void User_LoadFile(vector<Account>& users);


void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void Menu(Account& admin, vector<Account>& users) {
    int option;
    while (true) {
        clearScreen();
        cout << "'########::'####:'##:::'##:'########:::::'#######::'##::::'##:\n";
        cout << " ##.... ##:. ##:: ##::'##:: ##.....:::::'##.... ##: ##:::: ##:\n";
        cout << " ##:::: ##:: ##:: ##:'##::: ##::::::::::..::::: ##: ##:::: ##:\n";
        cout << " ########::: ##:: #####:::: ######:::::::'#######:: ##:::: ##:\n";
        cout << " ##.... ##:: ##:: ##. ##::: ##...:::::::'##:::::::: ##:::: ##:\n";
        cout << " ##:::: ##:: ##:: ##:. ##:: ##:::::::::: ##:::::::: ##:::: ##:\n";
        cout << " ########::'####: ##::. ##: ########:::: #########:. #######::\n";
        cout << "........:::....::..::::..::........:::::.........:::.......:::\n";

        cout << string(5, '\n') << endl;

        cout << "[1] Login\n";
        cout << "[2] Register\n";
        option = validateOption(1, 2);

        if (option == 1) {
            clearScreen();
            cout << "\n===== LOGIN =====\n";

            string email, password;
            cout << "Email   : ";
            getline(cin, email);
            cout << "Password: ";
            getline(cin, password);

            if (matchAdmin(admin, email, password)) {
                adminMenu(admin);
                continue;
            }

            int idx = matchUser(users, email, password);
            if (idx != -1) {
                userMenu(users, idx);
                continue;
            }

            cout << "\n[X] Invalid email or password.\n";
            waitForEnter();

        }
        else if (option == 2) {
            clearScreen();
            cout << "\n===== REGISTER =====\n";

            int idx = registerUser(users);
            if (idx != -1) {
                User_SaveFile(users);
                cout << "\n[^_^] Registration successful! Logging you in...\n";
                waitForEnter();
                clearScreen();
                userMenu(users, idx);
            }
            else {
                cout << "\n[X] Registration failed.\n";
                waitForEnter();
            }
        }
    }
}

void adminMenu(Account& admin) {
    int option;
    do {
        clearScreen();
        cout << "\n===== ADMIN DASHBOARD =====\n";
        cout << "[1] [Admin function 1]\n";
        cout << "[2] [Admin function 2]\n";
        cout << "[3] [Admin function 3]\n";
        cout << "[4] Logout\n";
        option = validateOption(1, 4);

        switch (option) {
        case 1:
            cout << "\n[Not implemented yet]\n";
            break;
        case 2:
            cout << "\n[Not implemented yet]\n";
            break;
        case 3:
            cout << "\n[Not implemented yet]\n";
            break;
        case 4:
            break;
        default:
            break;
        }
    } while (option != 4);
}

void userMenu(vector<Account>& users, int currentIdx) {
    int option;
    do {
        clearScreen();
        cout << "\n===== USER DASHBOARD =====\n";
        cout << "[1] [User function 1]\n";
        cout << "[2] [User function 2]\n";
        cout << "[3] [User function 3]\n";
        cout << "[4] Logout\n";
        option = validateOption(1, 4);

        switch (option) {
        case 1:
            cout << "\n[Not implemented yet]\n";
            break;
        case 2:
            cout << "\n[Not implemented yet]\n";
            break;
        case 3:
            cout << "\n[Not implemented yet]\n";
            break;
        case 4:
            break;
        default:
            break;
        }
    } while (option != 4);
}

int validateOption(int minVal, int maxVal) {
    string input;
    cout << "Enter option (" << minVal << "-" << maxVal << "): ";
    getline(cin, input);

    if (input.empty()) {
        cout << "[X] Input can't be empty. Please enter a number.\n";
        waitForEnter();
        return -1;
    }

    if (input.length() > 1) {
        cout << "[X] Invalid input! Please enter a single digit number.\n";
        waitForEnter();
        return -1;
    }

    for (char c : input) {
        if (!isdigit(c)) {
            cout << "[X] Invalid input! Numbers only (no letters, spaces or symbols).\n";
            waitForEnter();
            return -1;
        }
    }

    int option = input[0] - '0';

    if (option < minVal || option > maxVal) {
        cout << "[X] Invalid option! Please enter number between " << minVal << " and " << maxVal << ".\n";
        waitForEnter();
        return -1;
    }

    return option;
}

bool emailExist(const vector<Account>& users, const string& email) {
    for (const Account& u : users) {
        if (u.email == email)
            return true;
    }
    return false;
}

bool validateEmail(const string& email) {
    regex pattern(R"(^[A-Za-z0-9][A-Za-z0-9._%+-]*@[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)+$)");
    return regex_match(email, pattern);
}

bool validatePassword(const string& password) {
    if (password.length() < static_cast<size_t>(MIN_PWD))
        return false;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;

    for (char c : password) {
        if (isupper(c))
            hasUpper = true;
        else if (islower(c))
            hasLower = true;
        else if (isdigit(c))
            hasDigit = true;
        else
            hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

int registerUser(vector<Account>& users) {
    Account newAcc;

    cout << "Name" << string(12, ' ') << ": ";
    getline(cin, newAcc.name);
    if (newAcc.name.empty()) {
        cout << "[X] Name can't be empty!\n";
        return -1;
    }

    cout << "Email" << string(11, ' ') << ": ";
    getline(cin, newAcc.email);
    if (!validateEmail(newAcc.email)) {
        cout << "[X] Invalid the email format! (e.g. name@example.com)\n";
        return -1;
    }
    if (emailExist(users, newAcc.email)) {
        cout << "Email already registered!\n";
        return -1;
    }

    cout << "Phone" << string(11, ' ') << ": ";
    getline(cin, newAcc.phone);
    if (newAcc.phone.empty()) {
        cout << "[X] Phone can't be empty!\n";
        return -1;
    }

    string pwd1, pwd2;
    cout << "Password" << string(8, ' ') << ": ";
    getline(cin, pwd1);

    if (!validatePassword(pwd1)) {
        cout << "Password must have at least " << MIN_PWD << " characters, including uppercase letter, lowercase letter, " << "number and special symbol!\n";
        return -1;
    }

    cout << "Confirm Password: ";
    getline(cin, pwd2);
    if (pwd1 != pwd2) {
        cout << "[X] Passwords don't match!\n";
        return -1;
    }
    newAcc.password = pwd1;
    newAcc.accountID = static_cast<int>(users.size()) + 1;

    users.push_back(newAcc);
    return static_cast<int>(users.size()) - 1;
}

bool matchAdmin(const Account& admin, const string& email, const string& password) {
    return admin.email == email && admin.password == password;
}

int matchUser(const vector<Account>& users, const string& email, const string& password) {
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].email == email && users[i].password == password)
            return static_cast<int>(i);
    }
    return -1;
}

void Admin_SaveFile(const Account& admin) {
    ofstream outFile(ADMIN_FILE);
    if (!outFile) {
        cout << "[Warning] Cann't save admin file\n";
        return;
    }
    outFile << admin.accountID << "," << admin.name << "," << admin.email << "," << admin.phone << "," << admin.password << "\n";
    outFile.close();
}

bool Admin_LoadFile(Account& admin) {
    ifstream inFile(ADMIN_FILE);
    if (!inFile)
        return false;

    string line;
    if (!getline(inFile, line) || line.empty())
        return false;

    stringstream ss(line);
    string idStr, name, email, phone, password;
    getline(ss, idStr, ',');
    getline(ss, name, ',');
    getline(ss, email, ',');
    getline(ss, phone, ',');
    getline(ss, password, ',');

    admin.accountID = stoi(idStr);
    admin.name = name;
    admin.email = email;
    admin.phone = phone;
    admin.password = password;

    inFile.close();
    return true;
}

void User_SaveFile(const vector<Account>& users) {
    ofstream outFile(USER_FILE);
    if (!outFile) {
        cout << "[Warning] Cann't save user file\n";
        return;
    }
    for (const Account& u : users) {
        outFile << u.accountID << "," << u.name << "," << u.email << "," << u.phone << "," << u.password << "\n";
    }
    outFile.close();
}

void User_LoadFile(vector<Account>& users) {
    ifstream inFile(USER_FILE);
    if (!inFile)
        return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty())
            continue;

        stringstream ss(line);
        string idStr, name, email, phone, password;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, email, ',');
        getline(ss, phone, ',');
        getline(ss, password, ',');

        Account acc;
        acc.accountID = stoi(idStr);
        acc.name = name;
        acc.email = email;
        acc.phone = phone;
        acc.password = password;

        users.push_back(acc);
    }
    inFile.close();
}

int main() {

    Account admin;
    if (!Admin_LoadFile(admin)) {
        admin.accountID = 1;
        admin.name = "System Administrator";
        admin.email = ADMIN_EMAIL;
        admin.phone = "-";
        admin.password = ADMIN_PWD;
        Admin_SaveFile(admin);
    }

    vector<Account> users;
    User_LoadFile(users);

    Menu(admin, users);
    return 0;
}