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
#include <conio.h>
using namespace std;

const string ADMIN_EMAIL = "admin@bike2u.com";
const string ADMIN_PWD = "Admin@123";

const int MIN_PWD = 8;
const int REPORTS_PER_PAGE = 20;
const int CUSTOMERS_PER_PAGE = 10;
const int NUM_OPTIONS = 11;

const vector<string> damageOptions = {
      "Flat Tire", "Brake Problem", "Chain Problem", "Gear Problem", "Wheel Damage", "Seat Damage",
      "Handlebar Problem", "Light Problem", "Lock Problem", "Frame Damage", "Other"
};

const string ADMIN_FILE = "admin.csv";
const string USER_FILE = "user.csv";
const string REPAIR_FILE = "repair_reports.csv";

struct Account {
    string accountID;
    string name;
    string email;
    string phone;
    string password;
    string registrationDate;
};

struct RepairReport {
    string repairID;
    string userID;
    string userName;
    string bicycleID;
    string date;
    string time;
    string damageType;
    string status = "Pending";
};

void clearScreen();
void waitForEnter(const string& indent = "");
void Menu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs);

void loginMenu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs);
void adminMenu(vector<RepairReport>& repairs, vector<Account>& users);
void registerMenu(vector<Account>& users, vector<RepairReport>& repairs);
void adminRepairServiceMenu(vector<RepairReport>& repairs);
void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs);
void accountManagementMenu(vector<RepairReport>& repairs, vector<Account>& users);

int getValidOption(int minVal, int maxVal, const string& indent = "");
bool confirmAction(const string& message);
string generateIDFromList(const vector<string>& existingIDs, const string& prefix);
string generateAccountID(const vector<Account>& users);
string generateRepairID(const vector<RepairReport>& repairs);

bool validateEmail(const string& email);
bool validatePhone(const string& phone);
bool validatePassword(const string& password);
string getPasswordInput(const string& prompt);
bool emailExist(const vector<Account>& users, const string& email);
int registerUser(vector<Account>& users);
bool matchAdmin(const Account& admin, const string& email, const string& password);
int matchUser(const vector<Account>& users, const string& email, const string& password);

void repairService(const Account& user, vector<RepairReport>& repairs);
void viewAllRepairReports(const vector<RepairReport>& repairs);
void searchRepairDetails(const vector<RepairReport>& repairs);
void updateRepairReportStatus(vector<RepairReport>& repairs);
bool previous_nextPage(int& page, int totalPages);
int findRepairIndex(const vector<RepairReport>& repairs, const string& repairID);
void printRepairDetails(const RepairReport& repair);
void getCurrentDateTime(string& date, string& timeText);
string displayValue(const string& value);
void userProfile(vector<Account>& users, int currentIdx);
void viewAllAcc(const vector<Account>& users);
void deleteAcc(vector<RepairReport>& repairs, vector<Account>& users);

static string trim(const string& s);
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

void Menu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs) {
    const string indent = string(28, ' ');
    while (true) {
        clearScreen();

        cout << "\n" << indent << "'########::'####:'##:::'##:'########:::::'#######::'##::::'##:\n";
        cout << indent << " ##.... ##:. ##:: ##::'##:: ##.....:::::'##.... ##: ##:::: ##:\n";
        cout << indent << " ##:::: ##:: ##:: ##:'##::: ##::::::::::..::::: ##: ##:::: ##:\n";
        cout << indent << " ########::: ##:: #####:::: ######:::::::'#######:: ##:::: ##:\n";
        cout << indent << " ##.... ##:: ##:: ##. ##::: ##...:::::::'##:::::::: ##:::: ##:\n";
        cout << indent << " ##:::: ##:: ##:: ##:. ##:: ##:::::::::: ##:::::::: ##:::: ##:\n";
        cout << indent << " ########::'####: ##::. ##: ########:::: #########:. #######::\n";
        cout << indent << "........:::....::..::::..::........:::::.........:::.......:::\n";
        cout << string(5, '\n') << endl;


        cout << indent << "[1] Login\n";
        cout << indent << "[2] Register\n";
        cout << indent << "[0] Exit\n";
        int option = getValidOption(0, 2, indent);
        if (option == -1)
            continue;

        switch (option) {
        case 1:
            loginMenu(admin, users, repairs);
            break;
        case 2:
            registerMenu(users, repairs);
            break;
        case 0:
            if (confirmAction("Are you sure want to exit the program?"))
                exit(0);
            break;
        }
    }
}

void loginMenu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs) {
    const string indent = string(43, ' ');
    clearScreen();
    cout << string(6, '\n') << endl;
    cout << indent << "============ LOGIN ============\n";
    cout << string(2, '\n') << endl;

    string email, password;
    cout << indent << "[Enter 0 to turn back]\n" << endl;
    cout << indent << "Email   : ";
    getline(cin, email);
    if (trim(email) == "0") {
        return;
    }

    password = getPasswordInput(indent + "Password: ");

    if (matchAdmin(admin, email, password)) {
        adminMenu(repairs, users);
    }
    else {
        int idx = matchUser(users, email, password);
        if (idx != -1) {
            userMenu(users, idx, repairs);
        }
        else {
            cout << "\n" << indent << "[X] Invalid email or password.\n";
            waitForEnter(indent);
        }
    }
}

void registerMenu(vector<Account>& users, vector<RepairReport>& repairs) {
    const string indent = string(25, ' ');
    clearScreen();
    cout << string(6, '\n') << endl;
    cout << indent << string(30, '=') << " REGISTER " << string(30, '=') << "\n";
    cout << string(2, '\n') << endl;

    int idx = registerUser(users);
    if (idx == -2) {
        return;
    }
    if (idx != -1) {
        User_SaveFile(users);
        cout << "\n" << indent << "[^_^] Registration successful! Processing to main menu...\n";
        waitForEnter(indent);
        userMenu(users, idx, repairs);
    }
    else {
        cout << "\n" << indent << "[X] Registration failed.\n";
        waitForEnter(indent);
    }
}

void adminMenu(vector<RepairReport>& repairs, vector<Account>& users) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== ADMIN MENU =====\n";
        cout << indent << "[1] Customer Management\n";
        cout << indent << "[2] Repair Menu\n";
        cout << indent << "[0] Log out\n";

        int option = getValidOption(0, 2, indent);
        if (option == -1) continue;

        switch (option) {
        case 1:
            accountManagementMenu(repairs, users);
            break;
        case 2:
            adminRepairServiceMenu(repairs);
            break;
        case 0:
            if (confirmAction("Are you sure want to log out?"))
                return;
            break;
        }
    }
}

void adminRepairServiceMenu(vector<RepairReport>& repairs) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== ADMIN - REPAIR MENU =====\n";
        cout << indent << "[1] View All Repair Reports\n";
        cout << indent << "[2] Search Repair Report\n";
        cout << indent << "[3] Update Repair Report Status\n";
        cout << indent << "[0] Back\n";

        int option = getValidOption(0, 3, indent);
        if (option == -1)
            continue;

        switch (option) {
        case 1:
            viewAllRepairReports(repairs);
            break;
        case 2:
            searchRepairDetails(repairs);
            break;
        case 3:
            updateRepairReportStatus(repairs);
            break;
        case 0:
            if (confirmAction("Are you sure want to go back?"))
                return;
            break;
        }
    }
}

void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== USER MENU =====\n";
        cout << indent << "[1] [User function 1]\n";
        cout << indent << "[2] [User function 2]\n";
        cout << indent << "[3] Repair Service\n";
        cout << indent << "[4] Profile\n";
        cout << indent << "[0] Logout\n";

        int option = getValidOption(0, 4, indent);
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
            break;
        case 3:
            repairService(users[currentIdx], repairs);
            break;
        case 4:
            userProfile(users, currentIdx);
            break;
        case 0:
            if (confirmAction("Are you sure want to log out?"))
                return;
            break;
        }
    }
}

void accountManagementMenu(vector<RepairReport>& repairs, vector<Account>& users) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== ACCOUNT MANAGEMENT =====\n";
        cout << indent << "[1] View All Customers\n";
        cout << indent << "[2] Delete Account\n";
        cout << indent << "[0] Back\n";

        int option = getValidOption(0, 2, indent);
        if (option == -1) continue;

        switch (option) {
        case 1:
            viewAllAcc(users);
            break;
        case 2:
            deleteAcc(repairs, users);
            break;
        case 0:
            if (confirmAction("Are you sure want to go back?"))
                return;
            break;
        }
    }
}

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

bool confirmAction(const string& message) {
    while (true) {
        cout << message << " (Y/N): ";
        string input;
        getline(cin, input);

        if (input.length() == 1) {
            char answer = toupper(static_cast<unsigned char>(input[0]));
            if (answer == 'Y')
                return true;
            if (answer == 'N')
                return false;
        }
        cout << "[X] Invalid input. Please enter Y or N.\n";
    }
}

string generateIDFromList(const vector<string>& existingIDs, const string& prefix) {
    int maxNum = 0;
    for (const string& id : existingIDs) {
        if (id.rfind(prefix, 0) == 0) {
            string numPart = id.substr(prefix.length());
            if (!numPart.empty() && all_of(numPart.begin(), numPart.end(), ::isdigit)) {
                int num = stoi(numPart);
                if (num > maxNum) maxNum = num;
            }
        }
    }
    ostringstream oss;
    oss << prefix << setw(4) << setfill('0') << (maxNum + 1);
    return oss.str();
}

string generateAccountID(const vector<Account>& users) {
    vector<string> ids;
    ids.reserve(users.size());
    for (const auto& u : users)
        ids.push_back(u.accountID);
    return generateIDFromList(ids, "CUS");
}

string generateRepairID(const vector<RepairReport>& repairs) {
    vector<string> ids;
    ids.reserve(repairs.size());
    for (const auto& r : repairs)
        ids.push_back(r.repairID);
    return generateIDFromList(ids, "BRR");
}

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

bool validatePhone(const string& phone) {
    string cleaned;
    if (phone.empty())
        return false;
    for (char c : phone) {
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    }
    return (phone.length() >= 10 && phone.length() <= 11);
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

string getPasswordInput(const string& prompt) {
    cout << prompt;
    string password;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if (ch >= 32 && ch <= 126) {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

int registerUser(vector<Account>& users) {
    const string et = string(25, ' ');
    Account newAcc;

    cout << et << "[Enter 0 to turn back]" << endl;
    cout << et << "Name" << string(28, ' ') << ": ";
    getline(cin, newAcc.name);
    if (trim(newAcc.name) == "0")
        return -2;
    if (newAcc.name.empty()) {
        cout << et << "[X] Name cannot be empty.\n";
        return -1;
    }

    cout << et << "Email (example@gmail.com)" << string(7, ' ') << ": ";
    getline(cin, newAcc.email);
    if (trim(newAcc.email) == "0")
        return -2;
    if (!validateEmail(newAcc.email)) {
        cout << et << "[X] Invalid email format. (e.g., user@example.com)\n";
        return -1;
    }
    if (emailExist(users, newAcc.email)) {
        cout << et << "[X] Email already registered.\n";
        return -1;
    }

    cout << et << "Phone (10-11 digits)" << string(12, ' ') << ": ";
    getline(cin, newAcc.phone);
    if (trim(newAcc.phone) == "0")
        return -2;
    if (!validatePhone(newAcc.phone)) {
        cout << et << "[X] Invalid phone number. Please enter digits only (10-11 digits)\n";
        return -1;
    }

    string confirmPassword;
    cout << et << "Password (at least 8 characters): ";
    getline(cin, newAcc.password);
    if (trim(newAcc.password) == "0")
        return -2;
    if (!validatePassword(newAcc.password)) {
        cout << et << "[X] Password needs " << MIN_PWD << "+ characters (include characters, uppercase, lowercase, number and special symbol).\n";
        return -1;
    }

    cout << et << "Confirm Password" << string(16, ' ') << ": ";
    getline(cin, confirmPassword);
    if (trim(confirmPassword) == "0")
        return -2;
    if (newAcc.password != confirmPassword) {
        cout << et << "[X] Passwords do not match.\n";
        return -1;
    }


    newAcc.accountID = generateAccountID(users);
    string date, time;
    getCurrentDateTime(date, time);
    newAcc.registrationDate = date;
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
        cout << "\nEnter one or more numbers (e.g., 1,2,3): ";

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

    int attempts = 0;
    int choice = -1;
    while (attempts < 3) {
        clearScreen();
        cout << "===== REPAIR REPORT SUMMARY =====\n";
        cout << "ID          : " << user.accountID << "\n";
        cout << "Name        : " << user.name << "\n";
        cout << "Bicycle ID  : " << displayValue("") << "\n";
        cout << "Damage Type : " << damageType << "\n";

        cout << "\nSubmit this report? (Y/N): ";
        string input;
        getline(cin, input);
        if (input.length() == 1) {
            char c = toupper(static_cast<unsigned char>(input[0]));
            if (c == 'Y') {
                choice = 1;
                break;
            }
            if (c == 'N') {
                choice = 0;
                break;
            }
        }

        attempts++;
        if (attempts < 3) {
            cout << "[X] Invalid input. Please enter Y or N only. (" << 3 - attempts << " attempts remaining)\n";
            waitForEnter();
        }
        else {
            cout << "[X] Too many invalid attempts. Operation cancelled.\n";
            waitForEnter();
            return;
        }
    }

    if (choice == -1)
        return;

    if (choice == 1) {
        int confirmAttempts = 0;
        int confirmChoice = -1;
        while (confirmAttempts < 3) {
            clearScreen();
            cout << "===== REPAIR REPORT SUMMARY =====\n";
            cout << "ID          : " << user.accountID << "\n";
            cout << "Name        : " << user.name << "\n";
            cout << "Bicycle ID  : " << displayValue("") << "\n";
            cout << "Damage Type : " << damageType << "\n";

            cout << "\nConfirm submission? (Y/N): ";
            string input;
            getline(cin, input);
            if (input.length() == 1) {
                char c = toupper(static_cast<unsigned char>(input[0]));
                if (c == 'Y') {
                    confirmChoice = 1;
                    break;
                }
                if (c == 'N') {
                    confirmChoice = 0;
                    break;
                }
            }
            confirmAttempts++;
            if (confirmAttempts < 3) {
                cout << "[X] Invalid input. Please enter Y or N only. (" << 3 - confirmAttempts << " attempts remaining)\n";
                waitForEnter();
            }
            else {
                cout << "[X] Too many invalid attempts. Operation cancelled.\n";
                waitForEnter();
                return;
            }
        }
        if (confirmChoice == 1) {
            RepairReport repair;
            repair.repairID = generateRepairID(repairs);
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
        else {
            cout << "\n[X] Submission cancelled.\n";
            waitForEnter();
            return;
        }
    }
    else {
        int confirmAttempts = 0;
        int confirmChoice = -1;
        while (confirmAttempts < 3) {
            clearScreen();
            cout << "===== REPAIR REPORT SUMMARY =====\n";
            cout << "ID          : " << user.accountID << "\n";
            cout << "Name        : " << user.name << "\n";
            cout << "Bicycle ID  : " << displayValue("") << "\n";
            cout << "Damage Type : " << damageType << "\n";

            cout << "\nConfirm to cancel? (Y/N): ";
            string input;
            getline(cin, input);
            if (input.length() == 1) {
                char c = toupper(static_cast<unsigned char>(input[0]));
                if (c == 'Y') {
                    confirmChoice = 1;
                    break;
                }
                if (c == 'N') {
                    confirmChoice = 0;
                    break;
                }
            }
            confirmAttempts++;
            if (confirmAttempts < 3) {
                cout << "[X] Invalid input. Please enter Y or N only. (" << 3 - confirmAttempts << " attempts remaining)\n";
                waitForEnter();
            }
            else {
                cout << "[X] Too many invalid attempts. Operation cancelled.\n";
                waitForEnter();
                return;
            }
        }
        if (confirmChoice == 1) {
            cout << "\n[X] Submission cancelled.\n";
            waitForEnter();
            return;
        }
        else {
            cout << "\n[X] Submission cancelled.\n";
            waitForEnter();
            return;
        }
    }
}

string displayValue(const string& value) {
    return value.empty() ? "-" : value;
}

int findRepairIndex(const vector<RepairReport>& repairs, const string& repairID) {
    for (size_t i = 0; i < repairs.size(); ++i) {
        if (repairs[i].repairID == repairID)
            return static_cast<int>(i);
    }
    return -1;
}

void viewAllRepairReports(const vector<RepairReport>& repairs) {
    if (repairs.empty()) {
        clearScreen();
        cout << "No damage reports found.\n";
        waitForEnter();
        return;
    }

    int page = 0;
    int totalPages = static_cast<int>((repairs.size() + REPORTS_PER_PAGE - 1) / REPORTS_PER_PAGE);

    while (true) {
        clearScreen();
        cout << string(94, '-') << "\n";
        cout << string(37, ' ') << "All Repair Reports\n";
        cout << string(94, '-') << "\n";
        cout << left << setw(10) << "ID" << setw(13) << "Bicycle ID" << setw(12) << "Date" << setw(8) << "Time" << setw(30) << "Issues" << "Status\n";

        size_t first = page * REPORTS_PER_PAGE;
        size_t last = min(first + REPORTS_PER_PAGE, repairs.size());
        for (size_t i = first; i < last; ++i) {
            const RepairReport& repair = repairs[i];
            cout << left << setw(10) << repair.repairID << setw(13) << displayValue(repair.bicycleID) << setw(12) << repair.date << setw(8) << repair.time << setw(30) << repair.damageType.substr(0, 29) << repair.status << "\n";
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
        cout << "Pending: " << pending << "\tIn Progress: " << inProgress << "\tCompleted: " << completed << "\tCancelled: " << cancelled << "\n\n";

        if (!previous_nextPage(page, totalPages)) {
            return;
        }
    }
}

bool previous_nextPage(int& page, int totalPages) {
    cout << "[1] Previous Page\n";
    cout << "[2] Next Page\n";
    cout << "[0] Back\n";

    int option = getValidOption(0, 2, "");
    if (option == 0) {
        if (confirmAction("Are you sure want to go back?"))
            return false;
        else
            return true;
    }

    if (option == 1 && page > 0) {
        --page;
        return true;
    }
    else if (option == 1) {
        cout << "[X] This is the first page.\n";
        waitForEnter();
        return true;
    }

    if (option == 2 && page + 1 < totalPages) {
        ++page;
        return true;
    }
    else if (option == 2) {
        cout << "[X] This is the last page.\n";
        waitForEnter();
        return true;
    }
    return true;
}

void printRepairDetails(const RepairReport& repair) {
    cout << string(54, '-') << "\n";
    cout << "Report ID       : " << repair.repairID << "\n";
    cout << "Name            : " << repair.userName << "\n";
    cout << "Bicycle ID      : " << displayValue(repair.bicycleID) << "\n";
    cout << "Date & Time     : " << repair.date << " " << repair.time << "\n";
    cout << "Damage Type     : " << repair.damageType << "\n";
    cout << "Status          : " << repair.status << "\n";
    cout << string(54, '-') << "\n";
}

void searchRepairDetails(const vector<RepairReport>& repairs) {
    clearScreen();
    cout << string(54, '-') << "\n";
    cout << string(15, ' ') << "Search Repair Report\n";
    cout << string(54, '-') << "\n";
    string repairID;
    cout << "Enter Report ID: ";
    getline(cin, repairID);
    repairID = trim(repairID);
    if (repairID.empty()) {
        cout << "[X] Report ID cannot be empty.\n";
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

void updateRepairReportStatus(vector<RepairReport>& repairs) {
    while (true) {
        clearScreen();
        cout << string(54, '-') << "\n";
        cout << string(16, ' ') << "Update Report Status\n";
        cout << string(54, '-') << "\n";

        string repairID;
        cout << "Enter Report ID: ";
        getline(cin, repairID);
        repairID = trim(repairID);
        if (repairID.empty()) {
            cout << "[X] Report ID cannot be empty.\n";
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
        cout << "Report ID       : " << repair.repairID << "\n";
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

void userProfile(vector<Account>& users, int currentIdx) {
    const string indent = string(0, ' ');
    Account& user = users[currentIdx];
    bool changed = false;
    string input;

    while (true) {
        clearScreen();
        cout << indent << "===== USER PROFILE =====\n";
        cout << indent << "ID          : " << user.accountID << "\n";
        cout << indent << "Name        : " << user.name << "\n";
        cout << indent << "Email       : " << user.email << "\n";
        cout << indent << "Phone       : " << user.phone << "\n";
        cout << indent << "Password    : ********\n\n";
        cout << indent << "[1] Edit Name\n";
        cout << indent << "[2] Edit Email\n";
        cout << indent << "[3] Edit Phone\n";
        cout << indent << "[4] Change Password\n";
        cout << indent << "[0] Exit without saving\n";

        int option = getValidOption(0, 4, indent);
        if (option == -1)
            continue;

        if (option == 0) {
            if (changed) {
                while (true) {
                    cout << indent << "Changes will not be saved. Confirm exit? (Y/N): ";
                    string confirm;
                    getline(cin, confirm);
                    if (confirm == "Y" || confirm == "y") {
                        cout << indent << "Exit without saving.\n";
                        waitForEnter(indent);
                        return;
                    }
                    else if (confirm == "N" || confirm == "n") {
                        cout << indent << "Exit without unsave.\n";
                        waitForEnter(indent);
                        break;
                    }
                    else {
                        cout << indent << "[X] Invalid input. Please enter Y or N.\n";
                        waitForEnter(indent);
                    }
                }
            }
            else {
                cout << indent << "No changes made.\n";
                waitForEnter(indent);
                return;
            }
        }
        else if (option == 1) {
            cout << indent << "New Name (leave empty to keep current): ";
            getline(cin, input);
            string trimmed = trim(input);
            if (trimmed.empty()) {
                cout << indent << "Name unchanged (empty input).\n";
                waitForEnter(indent);
            }
            else if (trimmed != user.name) {
                user.name = trimmed;
                changed = true;
                cout << indent << "Name updated.\n";
                waitForEnter(indent);
            }
            else {
                cout << indent << "Name unchanged (same as current).\n";
                waitForEnter(indent);
            }
        }
        else if (option == 2) {
            cout << indent << "New Email (leave empty to keep current): ";
            getline(cin, input);
            if (!input.empty()) {
                string trimmed = trim(input);
                if (!validateEmail(trimmed)) {
                    cout << indent << "[X] Invalid email format.\n";
                    waitForEnter(indent);
                    continue;
                }
                bool duplicate = false;
                for (size_t i = 0; i < users.size(); ++i) {
                    if (i != currentIdx && users[i].email == trimmed) {
                        duplicate = true;
                        break;
                    }
                }
                if (duplicate) {
                    cout << indent << "[X] Email already registered by another user.\n";
                    waitForEnter(indent);
                    continue;
                }
                user.email = trimmed;
                changed = true;
                cout << indent << "Email updated.\n";
                waitForEnter(indent);
            }
            else {
                cout << indent << "Email unchanged.\n";
                waitForEnter(indent);
            }
        }
        else if (option == 3) {
            cout << indent << "New Phone (leave empty to keep current): ";
            getline(cin, input);
            if (!input.empty()) {
                string trimmed = trim(input);
                if (!validatePhone(trimmed)) {
                    cout << indent << "[X] Invalid phone number. Please enter 10-11 digits.\n";
                    waitForEnter(indent);
                    continue;
                }
                user.phone = trimmed;
                changed = true;
                cout << indent << "Phone updated.\n";
                waitForEnter(indent);
            }
            else {
                cout << indent << "Phone unchanged.\n";
                waitForEnter(indent);
            }
        }
        else if (option == 4) {
            cout << indent << "Current Password: ";
            string oldPwd;
            getline(cin, oldPwd);
            if (oldPwd != user.password) {
                cout << indent << "[X] Incorrect current password.\n";
                waitForEnter(indent);
                continue;
            }

            cout << indent << "New Password: ";
            string newPwd;
            getline(cin, newPwd);
            if (!validatePassword(newPwd)) {
                cout << indent << "[X] Password must be " << MIN_PWD << "+ characters(include uppercase, lowercase, digit, and special.)\n";
                waitForEnter(indent);
                continue;
            }

            cout << indent << "Confirm New Password: ";
            string confirm;
            getline(cin, confirm);
            if (newPwd != confirm) {
                cout << indent << "[X] Passwords do not match.\n";
                waitForEnter(indent);
                continue;
            }
            user.password = newPwd;
            changed = true;
            cout << indent << "Password updated.\n";
            waitForEnter(indent);
        }
    }
}

void viewAllAcc(const vector<Account>& users) {
    if (users.empty()) {
        clearScreen();
        cout << "No customers registered yet.\n";
        waitForEnter();
        return;
    }

    int page = 0;
    int totalPages = static_cast<int>((users.size() + CUSTOMERS_PER_PAGE - 1) / CUSTOMERS_PER_PAGE);

    while (true) {
        clearScreen();
        cout << string(90, '-') << "\n";
        cout << string(37, ' ') << " ALL CUSTOMERS \n";
        cout << string(90, '-') << "\n";
        cout << left << setw(5) << "No" << setw(12) << "Account ID" << setw(20) << "Name" << setw(25) << "Email" << setw(15) << "Phone" << setw(15) << "Reg. Date" << "\n";
        cout << string(90, '-') << "\n";

        size_t first = page * CUSTOMERS_PER_PAGE;
        size_t last = min(first + CUSTOMERS_PER_PAGE, users.size());
        for (size_t i = first; i < last; ++i) {
            const Account& user = users[i];
            cout << left << setw(5) << (i + 1) << setw(12) << user.accountID << setw(20) << user.name.substr(0, 19) << setw(25) << user.email.substr(0, 24) << setw(15) << user.phone << setw(15) << user.registrationDate << "\n";
        }

        cout << string(90, '-') << "\n";
        cout << "Page " << page + 1 << "/" << totalPages << "\n";
        cout << "Total Customers: " << users.size() << "\n\n";

        if (!previous_nextPage(page, totalPages)) {
            return;
        }
    }
}

void deleteAcc(vector<RepairReport>& repairs, vector<Account>& users) {
    const string indent = string(0, ' ');
    clearScreen();
    cout << indent << "===== DELETE ACCOUNT =====\n";
    cout << indent << "Enter Email or Account ID of user to delete: ";
    string input;
    getline(cin, input);
    if (input.empty()) {
        cout << indent << "[X] Input cannot be empty.\n";
        waitForEnter(indent);
        return;
    }

    int targetIndex = -1;
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].accountID == input || users[i].email == input) {
            targetIndex = static_cast<int>(i);
            break;
        }
    }

    if (targetIndex == -1) {
        cout << indent << "[X] User not found.\n";
        waitForEnter(indent);
        return;
    }

    const Account& user = users[targetIndex];
    cout << indent << "User found:\n";
    cout << indent << "ID   : " << user.accountID << "\n";
    cout << indent << "Name : " << user.name << "\n";
    cout << indent << "Email: " << user.email << "\n";
    cout << indent << "Phone: " << user.phone << "\n";
    cout << indent << "Reg. : " << user.registrationDate << "\n\n";
    cout << indent << "Are you sure want to delete this account? (Y/N): ";
    string confirm;
    getline(cin, confirm);
    if (confirm != "Y" && confirm != "y") {
        cout << indent << "Deletion cancelled.\n";
        waitForEnter(indent);
        return;
    }

    users.erase(users.begin() + targetIndex);
    User_SaveFile(users);

    cout << indent << "[^_^] Account deleted successfully.\n";
    waitForEnter(indent);
}

static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

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

    admin.accountID = id;
    return true;
}

void User_SaveFile(const vector<Account>& users) {
    ofstream outFile(USER_FILE);
    if (!outFile)
        return;
    for (const Account& user : users) {
        outFile << user.accountID << ',' << user.name << ',' << user.email << ',' << user.phone << ',' << user.password << "," << user.registrationDate << '\n';
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
        if (!getline(ss, user.registrationDate, ',')) {
            user.registrationDate = "";
        }

        user.name = trim(user.name);
        user.email = trim(user.email);
        user.phone = trim(user.phone);
        user.password = trim(user.password);
        user.registrationDate = trim(user.registrationDate);
        id = trim(id);

        user.accountID = id;
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
        outFile << repair.repairID << ';' << repair.userID << ';' << repair.userName << ';' << repair.bicycleID << ';' << repair.date << ';' << repair.time << ';' << repair.damageType << ';' << repair.status << '\n';
    }
}

void Repair_LoadFile(vector<RepairReport>& repairs) {
    ifstream inFile(REPAIR_FILE);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        RepairReport repair;
        string repairID, userID;
        getline(ss, repairID, ';');
        getline(ss, userID, ';');
        getline(ss, repair.userName, ';');
        getline(ss, repair.bicycleID, ';');
        getline(ss, repair.date, ';');
        getline(ss, repair.time, ';');
        getline(ss, repair.damageType, ';');
        getline(ss, repair.status, ';');

        repair.userName = trim(repair.userName);
        repair.bicycleID = trim(repair.bicycleID);
        repair.date = trim(repair.date);
        repair.time = trim(repair.time);
        repair.damageType = trim(repair.damageType);
        repair.status = trim(repair.status);
        repairID = trim(repairID);
        userID = trim(userID);

        repair.repairID = repairID;
        repair.userID = userID;
        repairs.push_back(repair);
    }
}

int main() {
    Account admin;
    if (!Admin_LoadFile(admin)) {
        admin.accountID = "1";
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