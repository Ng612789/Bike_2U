#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <limits>
#include <cctype>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using namespace std;

const string ADMIN_EMAIL = "admin@bike2u.com";
const string ADMIN_PWD = "Admin@123";

const int MIN_PWD = 8;
const int REPORTS_PER_PAGE = 20;
const int NUM_OPTIONS = 11;

const vector<string> damageOptions = {
      "Flat Tire", "Brake Problem", "Chain Problem", "Gear Problem", "Wheel Damage", "Seat Damage",
      "Handlebar Problem", "Light Problem", "Lock Problem", "Frame Damage", "Other"
};

const string ADMIN_FILE = "admin.csv";
const string USER_FILE = "user.csv";
const string REPAIR_FILE = "repair_reports.csv";

struct Account {
    int accountID;
    string name;
    string email;
    string phone;
    string password;
};

struct RepairReport {
    int repairID;
    int userID;
    string userName;
    string bicycleID;
    string date;
    string time;
    string damageType;
    string status = "Pending";
};

// Extra help(辅助) function
void clearScreen();
void waitForEnter(const string& indent = "");

// All the menu function
void Menu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs);
void adminMenu(vector<RepairReport>& repairs);
void adminRepairServiceMenu(vector<RepairReport>& repairs);
void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs);

// Input help function
int getValidOption(int minVal, int maxVal, const string& indent = "");
bool getYesNo(const string& prompt);
int getRepairID();

// Verify emial & password function and Login & Register function
bool validateEmail(const string& email);
bool validatePassword(const string& password);
bool emailExist(const vector<Account>& users, const string& email);
int registerUser(vector<Account>& users);
bool matchAdmin(const Account& admin, const string& email, const string& password);
int matchUser(const vector<Account>& users, const string& email, const string& password);

// User and Admin damage report dan help(辅助) functionn
void repairService(const Account& user, vector<RepairReport>& repairs);
void viewAllDamageReports(const vector<RepairReport>& repairs);
void viewDamageReportDetails(const vector<RepairReport>& repairs);
void updateDamageReportStatus(vector<RepairReport>& repairs);
int findRepairIndex(const vector<RepairReport>& repairs, int repairID);
void printRepairDetails(const RepairReport& repair);
void getCurrentDateTime(string& date, string& timeText);
string displayValue(const string& value);
static string trim(const string& s);

// All save & load function
void Admin_SaveFile(const Account& admin);
bool Admin_LoadFile(Account& admin);
void User_SaveFile(const vector<Account>& users);
void User_LoadFile(vector<Account>& users);
void Repair_SaveFile(const vector<RepairReport>& repairs);
void Repair_LoadFile(vector<RepairReport>& repairs);


void clearScreen() {
    system("cls");
}

void waitForEnter(const string& indent) {
    cout << "\n" << indent << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Menu
void Menu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs) {
    const string indent1 = string(28, ' '), indent2 = string(43, ' '), indent3 = string(25, ' ');
    while (true) {
        clearScreen();

        cout << "\n" << indent1 << "'########::'####:'##:::'##:'########:::::'#######::'##::::'##:\n";
        cout << indent1 << " ##.... ##:. ##:: ##::'##:: ##.....:::::'##.... ##: ##:::: ##:\n";
        cout << indent1 << " ##:::: ##:: ##:: ##:'##::: ##::::::::::..::::: ##: ##:::: ##:\n";
        cout << indent1 << " ########::: ##:: #####:::: ######:::::::'#######:: ##:::: ##:\n";
        cout << indent1 << " ##.... ##:: ##:: ##. ##::: ##...:::::::'##:::::::: ##:::: ##:\n";
        cout << indent1 << " ##:::: ##:: ##:: ##:. ##:: ##:::::::::: ##:::::::: ##:::: ##:\n";
        cout << indent1 << " ########::'####: ##::. ##: ########:::: #########:. #######::\n";
        cout << indent1 << "........:::....::..::::..::........:::::.........:::.......:::\n";
        cout << string(5, '\n') << endl;


        cout << indent1 << "[1] Login\n";
        cout << indent1 << "[2] Register\n";
        int option = getValidOption(1, 2, indent1);
        if (option == -1)
            continue;

        if (option == 1) {
            clearScreen();
            cout << string(6, '\n') << endl;
            cout << indent2 << "============ LOGIN ============\n";
            cout << string(2, '\n') << endl;

            string email, password;
            cout << indent2 << "Email   : ";
            getline(cin, email);
            cout << indent2 << "Password: ";
            getline(cin, password);

            if (matchAdmin(admin, email, password)) {
                adminMenu(repairs);
            }
            else {
                int idx = matchUser(users, email, password);
                if (idx != -1) {
                    userMenu(users, idx, repairs);
                }
                else {
                    cout << "\n" << indent2 << "[X] Invalid email or password.\n";
                    waitForEnter(indent2);
                }
            }
        }
        else {
            clearScreen();
            cout << string(6, '\n') << endl;
            cout << indent3 << string(30, '=') << " REGISTER " << string(30, '=') << "\n";
            cout << string(2, '\n') << endl;

            int idx = registerUser(users);
            if (idx != -1) {
                User_SaveFile(users);
                cout << "\n" << indent3 << "[^_^] Registration successful! Logging you in...\n";
                waitForEnter(indent3);
                userMenu(users, idx, repairs);
            }
            else {
                cout << "\n" << indent3 << "[X] Registration failed.\n";
                waitForEnter(indent3);
            }
        }
    }
}

void adminMenu(vector<RepairReport>& repairs) {
    const string indent = string(4, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== ADMIN DASHBOARD =====\n";
        cout << indent << "[1] [Admin function 1]\n";
        cout << indent << "[2] [Admin function 2]\n";
        cout << indent << "[3] Repair Service\n";
        cout << indent << "[4] Logout\n";

        int option = getValidOption(1, 4, indent);
        if (option == -1)
            continue;

        switch (option) {
        case 1:
        case 2:
            cout << indent << "\n[Not implemented yet]\n";
            waitForEnter(indent);
            break;
        case 3:
            adminRepairServiceMenu(repairs);
            break;
        case 4:
            return;
        }
    }
}

void adminRepairServiceMenu(vector<RepairReport>& repairs) {
    const string indent = string(6, ' ');   // 更深一层缩进6个空格
    while (true) {
        clearScreen();
        cout << indent << "===== ADMIN - REPAIR SERVICE =====\n";
        cout << indent << "[1] View All Damage Reports\n";
        cout << indent << "[2] View Report Details\n";
        cout << indent << "[3] Update Report Status\n";
        cout << indent << "[0] Back\n";

        int option = getValidOption(0, 3, indent);
        if (option == -1)
            continue;

        switch (option) {
        case 1:
            viewAllDamageReports(repairs);
            break;
        case 2:
            viewDamageReportDetails(repairs);
            break;
        case 3:
            updateDamageReportStatus(repairs);
            break;
        case 0:
            return;
        }
    }
}

void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs) {
    const string indent = string(4, ' ');   // 用户菜单缩进4个空格
    while (true) {
        clearScreen();
        cout << indent << "===== USER DASHBOARD =====\n";
        cout << indent << "[1] [User function 1]\n";
        cout << indent << "[2] [User function 2]\n";
        cout << indent << "[3] Repair Service\n";
        cout << indent << "[4] Logout\n";

        int option = getValidOption(1, 4, indent);
        if (option == -1)
            continue;

        switch (option) {
        case 1:
            cout << indent << "\n[Not implemented yet]\n";
            waitForEnter(indent);
            break;
        case 2:
            cout << indent << "\n[Not implemented yet]\n";
            waitForEnter(indent);
        case 3:
            repairService(users[currentIdx], repairs);
            break;
        case 4:
            return;
        }
    }
}

// Input help function
int getValidOption(int minVal, int maxVal, const string& indent) {
    while (true) {
        string input;
        cout << indent << "Enter option (" << minVal << "-" << maxVal << "): ";
        getline(cin, input);

        if (input.length() != 1 || !isdigit(static_cast<unsigned char>(input[0]))) {
            cout << indent << "[X] Please enter one valid number.\n";
            waitForEnter(indent);
            return -1;
        }

        int option = input[0] - '0';
        if (option < minVal || option > maxVal) {
            cout << indent << "[X] Please enter a number between " << minVal << " and " << maxVal << ".\n";
            waitForEnter(indent);
            return -1;
        }
        return option;
    }
}

bool getYesNo(const string& prompt) {
    while (true) {
        string input;
        cout << prompt;
        getline(cin, input);

        if (input.length() == 1) {
            char answer = toupper(static_cast<unsigned char>(input[0]));
            if (answer == 'Y')
                return true;
            if (answer == 'N')
                return false;
        }
        cout << "[X] Please enter Y or N only.\n";
    }
}

int getRepairID() {
    string input;
    cout << "Enter Report ID: ";
    getline(cin, input);

    if (input.empty()) {
        cout << "[X] Report ID cannot be empty.\n";
        return -1;
    }
    for (char c : input) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            cout << "[X] Report ID must contain numbers only.\n";
            return -1;
        }
    }
    try {
        return stoi(input);
    }
    catch (...) {
        cout << "[X] Invalid Report ID.\n";
        return -1;
    }
}

// Verify emial & password function and Login & Register function
bool emailExist(const vector<Account>& users, const string& email) {
    for (const Account& user : users) {
        if (user.email == email)
            return true;
    }
    return false;
}

bool validateEmail(const string& email) {
    regex pattern(R"(^[A-Za-z0-9][A-Za-z0-9._%+-]*@[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)+$)");
    return regex_match(email, pattern);
}

bool validatePassword(const string& password) {
    if (password.length() < static_cast<size_t>(MIN_PWD)) return false;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password) {
        if (isupper(static_cast<unsigned char>(c)))
            hasUpper = true;
        else if (islower(static_cast<unsigned char>(c)))
            hasLower = true;
        else if (isdigit(static_cast<unsigned char>(c)))
            hasDigit = true;
        else hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

int registerUser(vector<Account>& users) {
    const string et = string(25, ' ');
    Account newAcc;
    cout << et << "Name            : ";
    getline(cin, newAcc.name);
    if (newAcc.name.empty()) {
        cout << et << "[X] Name cannot be empty.\n";
        return -1;
    }

    cout << et << "Email           : ";
    getline(cin, newAcc.email);
    if (!validateEmail(newAcc.email)) {
        cout << et << "[X] Invalid email format.\n";
        return -1;
    }
    if (emailExist(users, newAcc.email)) {
        cout << et << "[X] Email already registered.\n";
        return -1;
    }

    cout << et << "Phone           : ";
    getline(cin, newAcc.phone);
    if (newAcc.phone.empty()) {
        cout << et << "[X] Phone cannot be empty.\n";
        return -1;
    }

    string confirmPassword;
    cout << et << "Password        : ";
    getline(cin, newAcc.password);
    if (!validatePassword(newAcc.password)) {
        cout << et << "[X] Password needs " << MIN_PWD << "+ characters, uppercase, lowercase, number and special symbol.\n";
        return -1;
    }
    cout << et << "Confirm Password: ";
    getline(cin, confirmPassword);
    if (newAcc.password != confirmPassword) {
        cout << et << "[X] Passwords do not match.\n";
        return -1;
    }

    newAcc.accountID = static_cast<int>(users.size()) + 1;
    users.push_back(newAcc);
    return static_cast<int>(users.size()) - 1;
}

bool matchAdmin(const Account& admin, const string& email, const string& password) {
    return admin.email == email && admin.password == password;
}

int matchUser(const vector<Account>& users, const string& email, const string& password) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].email == email && users[i].password == password) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// User and Admin damage report dan help(辅助) functionn

void getCurrentDateTime(string& date, string& timeText) {
    time_t now = time(nullptr);
    tm localTime{};
    localtime_s(&localTime, &now);
    ostringstream dateStream, timeStream;
    dateStream << put_time(&localTime, "%Y-%m-%d");
    timeStream << put_time(&localTime, "%H:%M");
    date = dateStream.str();
    timeText = timeStream.str();
}

void repairService(const Account& user, vector<RepairReport>& repairs) {
    string damageType;
    bool validInput = false;

    while (!validInput) {
        clearScreen();
        cout << "===== REPAIR SERVICE =====\n";
        cout << "\n----- User Information -----\n";
        cout << "ID        : " << user.accountID << "\n";
        cout << "Name      : " << user.name << "\n";
        string bicycleID = "";
        cout << "Bicycle ID: " << bicycleID << "\n";

        cout << "\n========== DAMAGE TYPE ==========\n";
        for (int i = 0; i < NUM_OPTIONS; ++i) {
            cout << i + 1 << ". " << damageOptions[i] << "\n";
        }
        cout << "\nEnter one or more numbers (comma-separated, e.g., 1,3,5): ";

        string inputLine;
        getline(cin, inputLine);

        inputLine.erase(remove_if(inputLine.begin(), inputLine.end(), ::isspace), inputLine.end());

        if (inputLine.empty()) {
            cout << "[X] Input cannot be empty.\n";
            waitForEnter();
            continue;
        }

        vector<int> selected;
        bool parseError = false;
        stringstream ss(inputLine);
        string token;
        while (getline(ss, token, ',')) {
            if (token.empty()) {
                parseError = true;
                break;
            }
            bool isNumber = true;
            for (char c : token) {
                if (!isdigit(static_cast<unsigned char>(c))) {
                    isNumber = false;
                    break;
                }
            }
            if (!isNumber) {
                parseError = true;
                break;
            }
            int num = stoi(token);
            if (num < 1 || num > NUM_OPTIONS) {
                parseError = true;
                break;
            }
            if (find(selected.begin(), selected.end(), num) != selected.end()) {
                parseError = true;
                break;
            }
            selected.push_back(num);
        }

        if (parseError) {
            cout << "[X] Invalid input. Please enter numbers between 1 and " << NUM_OPTIONS << " separated by commas, without duplicates.\n";
            waitForEnter();
            continue;
        }

        string combined;
        bool hasOther = false;
        for (int idx : selected) {
            if (idx == NUM_OPTIONS) {
                hasOther = true;
                continue;
            }
            if (!combined.empty()) combined += ", ";
            combined += damageOptions[idx - 1];
        }

        if (hasOther) {
            cout << "\nDescribe the damage in detail: ";
            string description;
            getline(cin, description);
            if (description.empty() || description.find(',') != string::npos) {
                cout << "[X] Description cannot be empty or contain a comma.\n";
                waitForEnter();
                continue;
            }
            if (!combined.empty()) combined += ", ";
            combined += "Other - " + description;
        }

        if (combined.empty() && !hasOther) {
            cout << "[X] No valid damage type selected.\n";
            waitForEnter();
            continue;
        }

        damageType = combined;
        validInput = true;
    }


    clearScreen();
    cout << "===== REPAIR REPORT SUMMARY =====\n";
    cout << "ID          : " << user.accountID << "\n";
    cout << "Name        : " << user.name << "\n";
    cout << "Bicycle ID  : " << displayValue("") << "\n";
    cout << "Damage Type : " << damageType << "\n";

    bool submit = getYesNo("\nSubmit this report? (Y/N): ");
    if (submit) {
        if (!getYesNo("You chose Submit. Confirm submission? (Y/N): ")) {
            cout << "\n[X] Submission cancelled.\n";
            waitForEnter();
            return;
        }
    }
    else {
        if (getYesNo("You chose Not Submit. Confirm cancellation? (Y/N): ")) {
            cout << "\n[X] Submission cancelled.\n";
            waitForEnter();
            return;
        }
        cout << "\n[X] Cancellation was not confirmed. Report was not submitted.\n";
        waitForEnter();
        return;
    }


    RepairReport repair;
    repair.repairID = repairs.empty() ? 1 : repairs.back().repairID + 1;
    repair.userID = user.accountID;
    repair.userName = user.name;
    repair.bicycleID = "";
    getCurrentDateTime(repair.date, repair.time);
    repair.damageType = damageType;
    repair.status = "Pending";

    repairs.push_back(repair);
    Repair_SaveFile(repairs);
    cout << "\n[^_^] Repair report submitted successfully.\n";
    waitForEnter();
}

string displayValue(const string& value) {
    return value.empty() ? "-" : value;
}

int findRepairIndex(const vector<RepairReport>& repairs, int repairID) {
    for (size_t i = 0; i < repairs.size(); ++i) {
        if (repairs[i].repairID == repairID)
            return static_cast<int>(i);
    }
    return -1;
}

void viewAllDamageReports(const vector<RepairReport>& repairs) {
    if (repairs.empty()) {
        clearScreen();
        cout << "No damage reports found.\n";
        waitForEnter();
        return;
    }

    size_t page = 0;
    const size_t totalPages = (repairs.size() + REPORTS_PER_PAGE - 1) / REPORTS_PER_PAGE;

    while (true) {
        clearScreen();
        cout << string(94, '-') << "\n";
        cout << "                              All Damage Reports\n";
        cout << string(94, '-') << "\n";
        cout << left << setw(6) << "ID" << setw(13) << "Bicycle ID" << setw(12) << "Date" << setw(8) << "Time" << setw(30) << "Issues" << "Status\n";

        size_t first = page * REPORTS_PER_PAGE;
        size_t last = min(first + REPORTS_PER_PAGE, repairs.size());
        for (size_t i = first; i < last; ++i) {
            const RepairReport& repair = repairs[i];
            cout << left << setw(6) << repair.repairID << setw(13) << displayValue(repair.bicycleID) << setw(12) << repair.date
                << setw(8) << repair.time << setw(30) << repair.damageType.substr(0, 29) << repair.status << "\n";
        }

        int pending = 0, inProgress = 0, completed = 0, cancelled = 0;
        for (const RepairReport& repair : repairs) {
            if (repair.status == "Pending")
                ++pending;
            else if (repair.status == "In Progress")
                ++inProgress;
            else if (repair.status == "Completed")
                ++completed;
            else if (repair.status == "Cancelled")
                ++cancelled;
        }

        cout << string(94, '-') << "\n";
        cout << "Page " << page + 1 << "/" << totalPages << "\n\n";
        cout << "Pending: " << pending << "\tIn Progress: " << inProgress
            << "\tCompleted: " << completed << "\tCancelled: " << cancelled << "\n\n";
        cout << "[1] Next Page\n";
        cout << "[0] Back\n";

        int option = getValidOption(0, 1, "");
        if (option == 0)
            return;
        if (option == 1 && page + 1 < totalPages)
            ++page;
        else if (option == 1) {
            cout << "[X] This is the last page.\n";
            waitForEnter();
        }
    }
}

void printRepairDetails(const RepairReport& repair) {
    cout << string(54, '-') << "\n";
    cout << "Report ID       : " << setw(3) << setfill('0') << repair.repairID << setfill(' ') << "\n";
    cout << "Name            : " << repair.userName << "\n";
    cout << "Bicycle ID      : " << displayValue(repair.bicycleID) << "\n";
    cout << "Date & Time     : " << repair.date << " " << repair.time << "\n";
    cout << "Damage Type     : " << repair.damageType << "\n";
    cout << "Status          : " << repair.status << "\n";
    cout << string(54, '-') << "\n";
}

void viewDamageReportDetails(const vector<RepairReport>& repairs) {
    clearScreen();
    cout << string(54, '-') << "\n";
    cout << string(15, ' ') << "View Report Details\n";
    cout << string(54, '-') << "\n";
    int repairID = getRepairID();
    if (repairID == -1) {
        waitForEnter();
        return;
    }

    int index = findRepairIndex(repairs, repairID);
    if (index == -1) {
        cout << "[X] Report not found.\n";
        waitForEnter();
        return;
    }

    clearScreen();
    printRepairDetails(repairs[index]);
    cout << "[0] Back\n";
    while (getValidOption(0, 0, "") != 0) {}
}

void updateDamageReportStatus(vector<RepairReport>& repairs) {
    while (true) {
        clearScreen();
        cout << string(54, '-') << "\n";
        cout << "            Update Report Status\n";
        cout << string(54, '-') << "\n";

        int repairID = getRepairID();
        if (repairID == -1) {
            waitForEnter();
            return;
        }

        int index = findRepairIndex(repairs, repairID);
        if (index == -1) {
            cout << "[X] Report not found.\n";
            waitForEnter();
            return;
        }

        RepairReport& repair = repairs[index];
        clearScreen();
        cout << string(54, '-') << "\n";
        cout << "Report ID       : " << setw(3) << setfill('0') << repair.repairID << setfill(' ') << "\n";
        cout << "Current Status  : " << repair.status << "\n";
        cout << string(54, '-') << "\n";
        cout << "Select New Status:\n";
        cout << "[1] Pending\n";
        cout << "[2] In Progress\n";
        cout << "[3] Completed\n";
        cout << "[4] Cancelled\n";
        cout << "[0] Back\n";

        int option = getValidOption(0, 4, "");
        if (option == -1)
            continue;
        if (option == 0)
            return;

        const string statuses[] = { "Pending", "In Progress", "Completed", "Cancelled" };
        repair.status = statuses[option - 1];
        Repair_SaveFile(repairs);

        cout << string(54, '-') << "\n";
        cout << "Status updated successfully!\n";
        cout << "New Status: " << repair.status << "\n";
        cout << string(54, '-') << "\n";
        waitForEnter();
        return;
    }
}

static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}


// All the save &load file
void Admin_SaveFile(const Account& admin) {
    ofstream outFile(ADMIN_FILE);
    if (outFile) {
        outFile << admin.accountID << ',' << admin.name << ',' << admin.email << ',' << admin.phone << ',' << admin.password << '\n';
    }
}

bool Admin_LoadFile(Account& admin) {
    ifstream inFile(ADMIN_FILE);
    string line;
    if (!getline(inFile, line) || line.empty())
        return false;

    stringstream ss(line);
    string id;
    getline(ss, id, ',');
    getline(ss, admin.name, ',');
    getline(ss, admin.email, ',');
    getline(ss, admin.phone, ',');
    getline(ss, admin.password, ',');

    admin.name = trim(admin.name);
    admin.email = trim(admin.email);
    admin.phone = trim(admin.phone);
    admin.password = trim(admin.password);
    id = trim(id);

    try {
        admin.accountID = stoi(id);
    }
    catch (...) {
        return false;
    }
    return true;
}

void User_SaveFile(const vector<Account>& users) {
    ofstream outFile(USER_FILE);
    if (!outFile)
        return;
    for (const Account& user : users) {
        outFile << user.accountID << ',' << user.name << ',' << user.email << ',' << user.phone << ',' << user.password << '\n';
    }
}

void User_LoadFile(vector<Account>& users) {
    ifstream inFile(USER_FILE);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Account user;
        string id;
        getline(ss, id, ',');
        getline(ss, user.name, ',');
        getline(ss, user.email, ',');
        getline(ss, user.phone, ',');
        getline(ss, user.password, ',');

        // trim
        user.name = trim(user.name);
        user.email = trim(user.email);
        user.phone = trim(user.phone);
        user.password = trim(user.password);
        id = trim(id);

        try {
            user.accountID = stoi(id);
        }
        catch (...) {
            continue;
        }
        users.push_back(user);
    }
}

void Repair_SaveFile(const vector<RepairReport>& repairs) {
    ofstream outFile(REPAIR_FILE);
    if (!outFile) {
        cout << "[Warning] Cannot save repair report file.\n";
        return;
    }
    for (const RepairReport& repair : repairs) {
        outFile << repair.repairID << ',' << repair.userID << ',' << repair.userName << ',' << repair.bicycleID << ','
            << repair.date << ',' << repair.time << ',' << repair.damageType << ',' << repair.status << '\n';
    }
}

void Repair_LoadFile(vector<RepairReport>& repairs) {
    ifstream inFile(REPAIR_FILE);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        RepairReport repair;
        string repairID, userID;
        getline(ss, repairID, ',');
        getline(ss, userID, ',');
        getline(ss, repair.userName, ',');
        getline(ss, repair.bicycleID, ',');
        getline(ss, repair.date, ',');
        getline(ss, repair.time, ',');
        getline(ss, repair.damageType, ',');
        getline(ss, repair.status, ',');

        repair.userName = trim(repair.userName);
        repair.bicycleID = trim(repair.bicycleID);
        repair.date = trim(repair.date);
        repair.time = trim(repair.time);
        repair.damageType = trim(repair.damageType);
        repair.status = trim(repair.status);
        repairID = trim(repairID);
        userID = trim(userID);

        try {
            repair.repairID = stoi(repairID);
            repair.userID = stoi(userID);
        }
        catch (...) {
            continue;
        }
        repairs.push_back(repair);
    }
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
    vector<RepairReport> repairs;
    User_LoadFile(users);
    Repair_LoadFile(repairs);

    Menu(admin, users, repairs);
    return 0;
}