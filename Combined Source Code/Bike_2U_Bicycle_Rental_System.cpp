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
#include <chrono>
#include <map>
#include <filesystem>

#include "GenerateQRCode.hpp"

using namespace std;
using qrcodegen::QrCode;

// ------------------------------------------------------------------
// Constants & File Names
// ------------------------------------------------------------------
const string ADMIN_EMAIL = "admin@bike2u.com";
const string ADMIN_PWD = "Admin@123";

const int MIN_PWD = 8;
const int REPORTS_PER_PAGE = 20;
const int CUSTOMERS_PER_PAGE = 10;
const int NUM_OPTIONS = 6;

const vector<string> damageOptions = {
      "Flat Tire", "Brake Problem", "Chain Problem", "Gear Problem", "Seat Damage", "Other"
};

const string ADMIN_FILE = "admin.csv";
const string USER_FILE = "user.csv";
const string REPAIR_FILE = "repair_reports.csv";
const string BOOKING_FILE = "booking.csv";
const string INVENTORY_FILE = "inventory.csv";
const string PAYMENT_FILE = "payment.csv";

const double RATE_CITY = 6.00;
const double RATE_MOUNTAIN = 7.00;
const double RATE_ROAD = 5.00;
const double RATE_ELECTRIC = 9.00;
const int MAX_RENTAL_HOURS = 24;
const double MEMBER_DISCOUNT = 0.05;

const vector<string> PENANG_STATIONS = {
    "George Town",
    "Bayan Lepas",
    "Balik Pulau",
    "Batu Ferringhi",
    "Gelugor",
    "Tanjung Tokong"
};

// ------------------------------------------------------------------
// Structures
// ------------------------------------------------------------------
struct Account {
    string accountID;
    string name;
    string email;
    string phone;
    string password;
    string registrationDate;
    string membership;
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
    string station;
};

struct Bicycle {
    string bikeID;
    string type;
    string brand;
    double rentalRatePerHour;
    string station;
    string status = "Available";
};

struct Booking {
    string bookingID;
    string customerID;
    string bikeID;
    string bookingDate;
    string bookingTime;
    string pickupStation;
    string returnTime;
    string paymentMethod;
    string membership;
    int duration;
    string status;
};

// ------------------------------------------------------------------
// Payment-related structures
// ------------------------------------------------------------------
const double OVERTIME_RATE = 5.50;
const double TAX_RATE = 0.06;

struct Payment {
    string paymentID;
    string bookingID;
    string bikeID;
    string customerName;
    double paymentAmount;
    string paymentMethod;
    string paymentDate;
};

vector<Payment> transactions;
string currentCustomer = "";

// ------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------
void clearScreen();
void waitForEnter(const string& indent = "");
void Menu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings);


void loginMenu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings);
void adminMenu(vector<RepairReport>& repairs, vector<Account>& users, vector<Bicycle>& inventory, vector<Booking>& bookings);
void registerMenu(vector<Account>& users, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings);
void adminRepairServiceMenu(vector<RepairReport>& repairs);
void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings);
void accountManagementMenu(vector<RepairReport>& repairs, vector<Account>& users);

int getValidOption(int minVal, int maxVal, const string& indent = "");
bool confirmAction(const string& message);
string generateIDFromList(const vector<string>& existingIDs, const string& prefix);
string generateAccountID(const vector<Account>& users);
string generateRepairID(const vector<RepairReport>& repairs);
string generateBicycleID(const vector<Bicycle>& inventory);
string generateBookingID(const vector<Booking>& bookings);
string generatePaymentID(const vector<Payment>& payments);

bool validateEmail(const string& email);
bool validatePhone(const string& phone);
bool validatePassword(const string& password);
string getPasswordInput(const string& prompt);
bool emailExist(const vector<Account>& users, const string& email);
int registerUser(vector<Account>& users);
bool matchAdmin(const Account& admin, const string& email, const string& password);
int matchUser(const vector<Account>& users, const string& email, const string& password);

void repairService(const Account& user, vector<RepairReport>& repairs, const vector<Booking>& bookings);
void viewAllDamageReports(const vector<RepairReport>& repairs);
void viewDamageReportDetails(const vector<RepairReport>& repairs);
void updateDamageReportStatus(vector<RepairReport>& repairs);
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

// Bicycle inventory functions
void DisplayInventoryMenu(vector<Bicycle>& inventory, vector<Booking>& bookings);
bool isValidBikeType(const string& type);
bool isInteger(const string& s);
bool isDouble(const string& s);
void AddBicycle(vector<Bicycle>& inventory);
void UpdateBicycle(vector<Bicycle>& inventory, const vector<Booking>& bookings);
void RemoveBicycle(vector<Bicycle>& inventory);
int SearchBicycle(const vector<Bicycle>& inventory, const string& bikeID);
void ViewBicycle(const vector<Bicycle>& inventory);
void SaveInventoryToFile(const vector<Bicycle>& inventory);
void LoadInventoryFromFile(vector<Bicycle>& inventory);
string getStationFromUser(bool allowCancel = false);

// Rental booking functions
void rentalBookingMenu(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID, const string& customerName,
    const string& membership);
void CreateBooking(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID, const string& customerName,
    const string& membership);
void ViewMyBookings(const vector<Booking>& bookings, const string& customerID);
void ReturnBicycle(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID);
void SaveBookingsToFile(const vector<Booking>& bookings, const string& filename);
void LoadBookingsFromFile(vector<Booking>& bookings, const string& filename);
void SyncInventoryWithBookings(const vector<Booking>& bookings, vector<Bicycle>& inventory);
bool checkAvailability(const string& bikeID, const vector<Bicycle>& inventory);
bool hasActiveBookingForBike(const string& bikeID, const vector<Booking>& bookings);

// Payment functions
void LoadPaymentFromFile();
void SavePaymentToFile();
string ProcessPaymentForBooking(const Booking& booking, double rentalHours, double baseRate, const string& customerName);
bool isValidExpiry(const string& expiry);
void DisplayUserPaymentHistory(const string& customerName, const vector<Booking>& bookings);
void RequestRefund(vector<Account>& users, int currentIdx, vector<Booking>& bookings, vector<RepairReport>& repairs, vector<Payment>& transactions);
void DisplayAllPayments(const vector<Booking>& bookings);
void GenerateReceipt(const string& paymentID);
bool isValidCardNumber(const string& cardNum);
bool isValidCVV(const string& cvv);
void generateInvoiceForUser(
    vector<Account>& users,
    int currentIdx,
    vector<Booking>& bookings,
    vector<Bicycle>& inventory
);

void memberTypeAnalytics(const vector<Account>& users);
void bicycleAvailabilityAnalytics(const vector<Bicycle>& inventory);
string getMonth(const string& date);
string getYear(const string& date);
string monthName(const string& month);
string money(double value);
void revenueAnalytics(const vector<Payment>& transactions);
void bicycleAnalysis(const vector<Bicycle>& inventory);
void reportingMenu(vector<Account>& users, vector<Bicycle>& inventory, vector<Payment>& transactions);


// ------------------------------------------------------------------
// Function Definitions
// ------------------------------------------------------------------

void clearScreen() {
    system("cls");
}

void waitForEnter(const string& indent) {
    cout << "\n" << indent << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ------------------------------------------------------------------
// ID Generation
// ------------------------------------------------------------------
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

string generateBicycleID(const vector<Bicycle>& inventory) {
    vector<string> ids;
    ids.reserve(inventory.size());
    for (const auto& b : inventory)
        ids.push_back(b.bikeID);
    return generateIDFromList(ids, "BIC");
}

string generateBookingID(const vector<Booking>& bookings) {
    vector<string> ids;
    ids.reserve(bookings.size());
    for (const auto& b : bookings)
        ids.push_back(b.bookingID);
    return generateIDFromList(ids, "BKK");
}

string generatePaymentID(const vector<Payment>& payments) {
    vector<string> ids;
    ids.reserve(payments.size());
    for (const auto& p : payments)
        ids.push_back(p.paymentID);
    return generateIDFromList(ids, "INV");
}

// ------------------------------------------------------------------
// Main Menu
// ------------------------------------------------------------------
void Menu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings) {
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
            loginMenu(admin, users, repairs, inventory, bookings);
            break;
        case 2:
            registerMenu(users, repairs, inventory, bookings);
            break;
        case 0:
            cout << indent;
            if (confirmAction("Are you sure want to exit the program?"))
                return;
            break;
        }
    }
}

// ------------------------------------------------------------------
// Login / Register
// ------------------------------------------------------------------
void loginMenu(Account& admin, vector<Account>& users, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings) {
    const string indent = string(43, ' ');
    clearScreen();
    cout << string(6, '\n') << endl;
    cout << indent << "============ LOGIN ============\n";
    cout << string(2, '\n') << endl;

    string email, password;
    cout << indent << "[Enter 0 to return back]" << endl;
    cout << indent << "Email   : ";
    getline(cin, email);
    if (trim(email) == "0") {
        return;
    }

    password = getPasswordInput(indent + "Password: ");

    if (matchAdmin(admin, email, password)) {
        adminMenu(repairs, users, inventory, bookings);
    }
    else {
        int idx = matchUser(users, email, password);
        if (idx != -1) {
            currentCustomer = users[idx].name;
            userMenu(users, idx, repairs, inventory, bookings);
        }
        else {
            cout << "\n" << indent << "[X] Invalid email or password.\n";
            waitForEnter(indent);
        }
    }
}

void registerMenu(vector<Account>& users, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings) {
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
        cout << "\n" << indent << "Registration successful! Processing to user menu...\n";
        waitForEnter(indent);
        currentCustomer = users[idx].name;
        userMenu(users, idx, repairs, inventory, bookings);
    }
    else {
        cout << "\n" << indent << "[X] Registration failed.\n";
        waitForEnter(indent);
    }
}


// ------------------------------------------------------------------
// Admin Menu
// ------------------------------------------------------------------
void adminMenu(vector<RepairReport>& repairs, vector<Account>& users, vector<Bicycle>& inventory, vector<Booking>& bookings) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== ADMIN MENU =====\n";
        cout << indent << "[1] View Customer Details\n";
        cout << indent << "[2] View Reported Issues\n";
        cout << indent << "[3] View Bicycle Inventory\n";
        cout << indent << "[4] View All Transactions\n";
        cout << indent << "[5] View Analytics and Reports\n";
        cout << indent << "[0] Log out\n";

        int option = getValidOption(0, 5, indent);
        if (option == -1) continue;

        switch (option) {
        case 1:
            accountManagementMenu(repairs, users);
            break;
        case 2:
            adminRepairServiceMenu(repairs);
            break;
        case 3:
            DisplayInventoryMenu(inventory, bookings);
            break;
        case 4:
            DisplayAllPayments(bookings);
            break;
        case 5:
            reportingMenu(users, inventory, transactions);
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
        cout << indent << "[1] View all Reported issues\n";
        cout << indent << "[2] Filter Status\n";
        cout << indent << "[3] Update Status\n";
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
            if (confirmAction("Are you sure want to go back?"))
                return;
            break;
        }
    }
}

// ------------------------------------------------------------------
// User Menu
// ------------------------------------------------------------------
void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs, vector<Bicycle>& inventory, vector<Booking>& bookings) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "===== USER MENU =====\n";
        cout << indent << "[1] Bicycle Rental Booking\n";
        cout << indent << "[2] View Payment History\n";
        cout << indent << "[3] Request Refund\n";
        cout << indent << "[4] Report Issue\n";
        cout << indent << "[5] Profile\n";
        cout << indent << "[0] Log out\n";

        int option = getValidOption(0, 5, indent);
        if (option == -1)
            continue;

        switch (option) {
        case 1:
            rentalBookingMenu(bookings, inventory, users[currentIdx].accountID, users[currentIdx].name, users[currentIdx].membership);
            break;
        case 2:
            if (currentCustomer.empty()) {
                cout << "[X] No customer logged in.\n";
                waitForEnter();
            }
            else {
                DisplayUserPaymentHistory(currentCustomer, bookings);
            }
            break;
        case 3:
            RequestRefund(users, currentIdx, bookings, repairs, transactions);
            break;
        case 4:
            repairService(users[currentIdx], repairs, bookings);
            break;
        case 5:
            userProfile(users, currentIdx);
            break;
        case 0:
            if (confirmAction("Are you sure want to log out?")) {
                currentCustomer = "";
                return;
            }
            break;
        }
    }
}

// ------------------------------------------------------------------
// Account Management
// ------------------------------------------------------------------
void accountManagementMenu(vector<RepairReport>& repairs, vector<Account>& users) {
    const string indent = string(6, ' ');
    while (true) {
        clearScreen();
        cout << "===== ACCOUNT MANAGEMENT =====\n";
        cout << "[1] View All Customers\n";
        cout << "[2] Delete Account\n";
        cout << "[0] Back\n";

        int option = getValidOption(0, 2);
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

// ---------- Utility functions ----------
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
    return password.length() >= static_cast<size_t>(MIN_PWD);
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

    cout << et << "[Enter 0 to return back]" << endl;
    cout << et << "Name" << string(27, ' ') << ": ";
    getline(cin, newAcc.name);
    if (trim(newAcc.name) == "0")
        return -2;
    if (newAcc.name.empty()) {
        cout << et << "[X] Name cannot be empty.\n";
        return -1;
    }

    cout << et << "Email (e.g user@gamil.com)" << string(3, ' ') << ": ";
    getline(cin, newAcc.email);
    if (trim(newAcc.email) == "0")
        return -2;
    if (!validateEmail(newAcc.email)) {
        cout << et << "[X] Invalid email format. (e.g., user@gamil.com)\n";
        return -1;
    }
    if (emailExist(users, newAcc.email)) {
        cout << et << "[X] Email already registered.\n";
        return -1;
    }

    string domain = newAcc.email.substr(newAcc.email.find('@'));
    if (domain != "@gmail.com" && domain != "@yahoo.com") {
        cout << et << "[X] Only Gmail (@gmail.com) and Yahoo (@yahoo.com) email addresses are allowed.\n";
        return -1;
    }

    cout << et << "Phone (e.g 0102712051)" << string(9, ' ') << ": ";
    getline(cin, newAcc.phone);
    if (trim(newAcc.phone) == "0")
        return -2;
    if (!validatePhone(newAcc.phone)) {
        cout << et << "[X] Invalid phone number. Please enter digits only (10-11 digits)\n";
        return -1;
    }

    string confirmPassword;
    cout << et << "Password (at more 8 characters): ";
    getline(cin, newAcc.password);
    if (trim(newAcc.password) == "0")
        return -2;
    if (!validatePassword(newAcc.password)) {
        cout << et << "[X] Password needs more than" << MIN_PWD << "+ characters\n";
        return -1;
    }

    cout << et << "Confirm Password" << string(15, ' ') << ": ";
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


// ------------------------------------------------------------------
// Time function 
// ------------------------------------------------------------------
void getCurrentDateTime(string& date, string& timeText) {
    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm localTime;
    localtime_s(&localTime, &tt);
    ostringstream dateStream, timeStream;
    dateStream << put_time(&localTime, "%Y-%m-%d");
    timeStream << put_time(&localTime, "%H:%M");
    date = dateStream.str();
    timeText = timeStream.str();
}

void repairService(const Account& user, vector<RepairReport>& repairs, const vector<Booking>& bookings) {
    vector<string> bikeIDs;
    for (const auto& b : bookings) {
        if (b.customerID == user.accountID) {
            if (find(bikeIDs.begin(), bikeIDs.end(), b.bikeID) == bikeIDs.end()) {
                bikeIDs.push_back(b.bikeID);
            }
        }
    }

    if (bikeIDs.empty()) {
        clearScreen();
        cout << "===== REPAIR SERVICE =====\n";
        cout << "[X] You have no rental records. You can only report damage for a bicycle you have rented.\n";
        waitForEnter();
        return;
    }

    clearScreen();
    cout << "===== REPAIR SERVICE =====\n";
    cout << "\nSelect the bicycle you want to report damage for:\n";
    for (size_t i = 0; i < bikeIDs.size(); ++i) {
        cout << "  [" << i + 1 << "] Bike ID: " << bikeIDs[i] << "\n";
    }
    cout << "  [0] Cancel\n";
    cout << "Enter your choice (or 0 to cancel): ";

    int choice = -1;
    string input;
    getline(cin, input);
    if (input.empty()) {
        cout << "[X] Invalid input.\n";
        waitForEnter();
        return;
    }
    bool isNumber = true;
    for (char c : input) {
        if (!isdigit(static_cast<unsigned char>(c))) {
            isNumber = false;
            break;
        }
    }
    if (!isNumber) {
        cout << "[X] Please enter a number.\n";
        waitForEnter();
        return;
    }
    choice = stoi(input);
    if (choice == 0) {
        cout << "Operation cancelled.\n";
        waitForEnter();
        return;
    }
    if (choice < 1 || choice > static_cast<int>(bikeIDs.size())) {
        cout << "[X] Invalid choice.\n";
        waitForEnter();
        return;
    }
    string selectedBikeID = bikeIDs[choice - 1];

    string damageType;
    bool validInput = false;

    while (!validInput) {
        clearScreen();
        cout << "===== REPAIR SERVICE =====\n";
        cout << "\n----- User Information -----\n";
        cout << "ID        : " << user.accountID << "\n";
        cout << "Name      : " << user.name << "\n";
        cout << "Bicycle ID: " << selectedBikeID << "\n";

        cout << "\n========== DAMAGE TYPE ==========\n";
        for (int i = 0; i < NUM_OPTIONS; ++i) {
            cout << i + 1 << ". " << damageOptions[i] << "\n";
        }
        cout << "\n[Enter 0 to return back]";
        cout << "\nEnter one or more numbers (e.g., 1,2,3): ";

        string inputLine;
        getline(cin, inputLine);
        inputLine.erase(remove_if(inputLine.begin(), inputLine.end(), ::isspace), inputLine.end());

        if (inputLine == "0") {
            cout << "Operation cancelled.\n";
            waitForEnter();
            return;
        }

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

    string selectedStation;
    while (true) {
        clearScreen();
        cout << "===== SELECT BICYCLE LOCATION =====\n";
        cout << "\nWhere is the bicycle currently located? (for repair pickup)\n";
        for (size_t i = 0; i < PENANG_STATIONS.size(); ++i) {
            cout << "  [" << (i + 1) << "] " << PENANG_STATIONS[i] << "\n";
        }
        cout << "  [0] Cancel report\n";
        cout << "Enter choice: ";
        string stationInput;
        getline(cin, stationInput);
        if (stationInput.empty()) {
            cout << "[X] Invalid input.\n";
            waitForEnter();
            continue;
        }
        bool isNum = true;
        for (char c : stationInput) {
            if (!isdigit(static_cast<unsigned char>(c))) {
                isNum = false;
                break;
            }
        }
        if (!isNum) {
            cout << "[X] Please enter a number.\n";
            waitForEnter();
            continue;
        }
        int stChoice = stoi(stationInput);
        if (stChoice == 0) {
            cout << "Report cancelled.\n";
            waitForEnter();
            return;
        }
        if (stChoice < 1 || stChoice > static_cast<int>(PENANG_STATIONS.size())) {
            cout << "[X] Invalid station number.\n";
            waitForEnter();
            continue;
        }
        selectedStation = PENANG_STATIONS[stChoice - 1];
        break;
    }

    int attempts = 0;
    bool submitted = false;
    while (attempts < 3) {
        clearScreen();
        cout << "===== REPAIR REPORT SUMMARY =====\n";
        cout << "ID          : " << user.accountID << "\n";
        cout << "Name        : " << user.name << "\n";
        cout << "Bicycle ID  : " << selectedBikeID << "\n";
        cout << "Damage Type : " << damageType << "\n";
        cout << "Location    : " << selectedStation << "\n";

        cout << "\nSubmit this report? (Y/N): ";
        string confirmInput;
        getline(cin, confirmInput);
        if (confirmInput.length() == 1) {
            char c = toupper(static_cast<unsigned char>(confirmInput[0]));
            if (c == 'Y') {
                RepairReport repair;
                repair.repairID = generateRepairID(repairs);
                repair.userID = user.accountID;
                repair.userName = user.name;
                repair.bicycleID = selectedBikeID;
                getCurrentDateTime(repair.date, repair.time);
                repair.damageType = damageType;
                repair.status = "Pending";
                repair.station = selectedStation;

                repairs.push_back(repair);
                Repair_SaveFile(repairs);
                cout << "\nRepair report submitted successfully.\n";
                waitForEnter();
                submitted = true;
                break;
            }
            else if (c == 'N') {
                cout << "\n[X] Submission cancelled.\n";
                waitForEnter();
                submitted = false;
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

void viewAllDamageReports(const vector<RepairReport>& repairs) {
    if (repairs.empty()) {
        clearScreen();
        cout << "No damage reports found.\n";
        waitForEnter();
        return;
    }

    int page = 0;
    int totalPages = (repairs.size() + REPORTS_PER_PAGE - 1) / REPORTS_PER_PAGE;

    while (true) {
        clearScreen();
        cout << string(94, '=') << "\n";
        cout << string(30, ' ') << "All Repair Reports\n";
        cout << string(94, '=') << "\n";
        cout << left << setw(10) << "Repair ID" << setw(13) << "Bicycle ID" << setw(12) << "Date" << setw(8) << "Time" << setw(30) << "Issues" << "Status\n";

        size_t first = page * REPORTS_PER_PAGE;
        size_t last = min(first + REPORTS_PER_PAGE, repairs.size());
        for (size_t i = first; i < last; ++i) {
            const RepairReport& repair = repairs[i];
            cout << left << setw(10) << repair.repairID << setw(13) << displayValue(repair.bicycleID) << setw(12) << repair.date
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

void viewDamageReportDetails(const vector<RepairReport>& repairs) {
    while (true) {
        clearScreen();
        cout << string(54, '=') << "\n";
        cout << string(15, ' ') << "View Reports by Status\n";
        cout << string(54, '=') << "\n";
        cout << "[1] Pending\n";
        cout << "[2] In Progress\n";
        cout << "[3] Completed\n";
        cout << "[4] Cancelled\n";
        cout << "[0] Back\n";
        cout << "Enter option (0-4): ";
        int option = getValidOption(0, 4, "");
        if (option == -1)
            continue;
        if (option == 0) {
            if (confirmAction("Are you sure you want to go back?")) {
                return;
            }
            else {
                continue;
            }
        }

        string status;
        switch (option) {
        case 1:
            status = "Pending";
            break;
        case 2:
            status = "In Progress";
            break;
        case 3:
            status = "Completed";
            break;
        case 4:
            status = "Cancelled";
            break;
        }

        vector<RepairReport> filtered;
        for (const auto& r : repairs) {
            if (r.status == status) {
                filtered.push_back(r);
            }
        }

        if (filtered.empty()) {
            cout << "\nNo reports with status \"" << status << "\".\n";
            waitForEnter();
            continue;
        }

        clearScreen();
        cout << string(94, '=') << "\n";
        cout << string(30, ' ') << "Reports with Status: " << status << "\n";
        cout << string(94, '=') << "\n";
        cout << left << setw(10) << "Repair ID" << setw(13) << "Bicycle ID" << setw(12) << "Date" << setw(8) << "Time" << setw(30) << "Issues" << "Status\n";
        cout << string(94, '-') << "\n";
        for (const auto& r : filtered) {
            cout << left << setw(10) << r.repairID << setw(13) << r.bicycleID << setw(12) << r.date << setw(8) << r.time << setw(30) << r.damageType.substr(0, 29) << r.status << "\n";
        }
        cout << string(94, '-') << "\n";
        cout << "Total: " << filtered.size() << " reports.\n";
        waitForEnter();
    }
}

void updateDamageReportStatus(vector<RepairReport>& repairs) {
    while (true) {
        clearScreen();
        cout << string(54, '=') << "\n";
        cout << string(12, ' ') << "Update Report Status\n";
        cout << string(54, '=') << "\n";

        if (repairs.empty()) {
            cout << "No repair reports available.\n";
            waitForEnter();
            return;
        }

        cout << "\n--- All Repair Reports ---\n";
        cout << left << setw(14) << "Customer ID" << setw(13) << "Bicycle ID" << setw(12) << "Date" << setw(12) << "Status" << "\n";
        cout << string(47, '-') << "\n";
        for (const auto& repair : repairs) {
            cout << left << setw(14) << repair.repairID << setw(13) << repair.bicycleID << setw(12) << repair.date << setw(12) << repair.status << "\n";
        }
        cout << string(47, '-') << "\n\n";

        cout << "[Enter 0 to return back]" << endl;
        cout << "Enter Report ID to update status: ";
        string repairID;
        getline(cin, repairID);
        repairID = trim(repairID);

        if (repairID == "0") {
            if (confirmAction("Are you sure you want to go back?")) {
                return;
            }
            else {
                continue;
            }
        }

        if (repairID.empty()) {
            cout << "[X] Report ID cannot be empty.\n";
            waitForEnter();
            continue;
        }

        int index = findRepairIndex(repairs, repairID);
        if (index == -1) {
            cout << "[X] Report not found.\n";
            waitForEnter();
            continue;
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
        if (option == 0) {
            cout << "Returning to status update menu.\n";
            waitForEnter();
            continue;
        }

        const string statuses[] = { "Pending", "In Progress", "Completed", "Cancelled" };
        repair.status = statuses[option - 1];
        Repair_SaveFile(repairs);

        cout << string(54, '-') << "\n";
        cout << "Status updated successfully!\n";
        cout << "New Status: " << repair.status << "\n";
        cout << string(54, '-') << "\n";
        waitForEnter();
        continue;
    }
}

// ---------- User profile & account management ----------
void userProfile(vector<Account>& users, int currentIdx) {
    const string indent = string(4, ' ');
    Account& user = users[currentIdx];
    bool changed = false;
    string input;

    while (true) {
        clearScreen();
        cout << "===== USER PROFILE =====\n";
        cout << "ID          : " << user.accountID << "\n";
        cout << "Name        : " << user.name << "\n";
        cout << "Email       : " << user.email << "\n";
        cout << "Phone       : " << user.phone << "\n";
        cout << "Password    : " << user.password << "\n";
        cout << "[1] Edit Name\n";
        cout << "[2] Edit Email\n";
        cout << "[3] Edit Phone\n";
        cout << "[4] Change Password\n";
        cout << "[0] Exit and save changes\n";

        int option = getValidOption(0, 4);
        if (option == -1)
            continue;

        if (option == 0) {
            if (changed) {
                User_SaveFile(users);
                cout << indent << "Profile updated and saved successfully.\n";
            }
            else {
                cout << indent << "No changes made.\n";
            }
            waitForEnter(indent);
            return;
        }
        else if (option == 1) {
            cout << "New Name (leave empty to keep current): ";
            getline(cin, input);
            string trimmed = trim(input);
            if (trimmed.empty()) {
                cout << "Name unchanged (empty input).\n";
                waitForEnter(indent);
            }
            else if (trimmed != user.name) {
                user.name = trimmed;
                changed = true;
                cout << "Name updated.\n";
                waitForEnter(indent);
            }
            else {
                cout << "Name unchanged (same as current).\n";
                waitForEnter(indent);
            }
        }
        else if (option == 2) {
            cout << "New Email (leave empty to keep current): ";
            getline(cin, input);
            if (!input.empty()) {
                string trimmed = trim(input);
                if (!validateEmail(trimmed)) {
                    cout << "[X] Invalid email format.\n";
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
                    cout << "[X] Email already registered by another user.\n";
                    waitForEnter(indent);
                    continue;
                }
                string domain = trimmed.substr(trimmed.find('@'));
                if (domain != "@gmail.com" && domain != "@yahoo.com") {
                    cout << "[X] Only Gmail (@gmail.com) and Yahoo (@yahoo.com) email addresses are allowed.\n";
                    waitForEnter(indent);
                    continue;
                }
                user.email = trimmed;
                changed = true;
                cout << "Email updated.\n";
                waitForEnter(indent);
            }
            else {
                cout << "Email unchanged.\n";
                waitForEnter(indent);
            }
        }
        else if (option == 3) {
            cout << "New Phone (leave empty to keep current): ";
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
                cout << "Phone updated.\n";
                waitForEnter(indent);
            }
            else {
                cout << "Phone unchanged.\n";
                waitForEnter(indent);
            }
        }
        else if (option == 4) {
            cout << "Current Password: ";
            string oldPwd;
            getline(cin, oldPwd);
            if (oldPwd != user.password) {
                cout << "[X] Incorrect current password.\n";
                waitForEnter(indent);
                continue;
            }

            cout << "New Password: ";
            string newPwd;
            getline(cin, newPwd);
            if (!validatePassword(newPwd)) {
                cout << "[X] Password must be more than " << MIN_PWD << "+ characters\n";
                waitForEnter(indent);
                continue;
            }

            cout << "Confirm New Password: ";
            string confirm;
            getline(cin, confirm);
            if (newPwd != confirm) {
                cout << "[X] Passwords do not match.\n";
                waitForEnter(indent);
                continue;
            }
            user.password = newPwd;
            changed = true;
            cout << "Password updated.\n";
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
    int totalPages = (users.size() + CUSTOMERS_PER_PAGE - 1) / CUSTOMERS_PER_PAGE;

    while (true) {
        clearScreen();
        cout << string(90, '-') << "\n";
        cout << string(30, ' ') << "ALL CUSTOMERS\n";
        cout << string(90, '-') << "\n";
        cout << left << setw(5) << "No." << setw(12) << "Customer ID" << setw(20) << "Name" << setw(25) << "Email" << setw(15) << "Phone" << setw(15) << "Reg. Date" << "\n";
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
    clearScreen();
    cout << "===== DELETE ACCOUNT =====\n";

    if (users.empty()) {
        cout << "No users to delete.\n";
        waitForEnter();
        return;
    }

    cout << "\n--- All Registered Users ---\n";
    cout << left << setw(5) << "No." << setw(14) << "Customer ID" << setw(20) << "Name" << setw(25) << "Email" << setw(15) << "Phone" << "\n";
    cout << string(77, '-') << "\n";
    for (size_t i = 0; i < users.size(); ++i) {
        const Account& user = users[i];
        cout << left << setw(5) << (i + 1) << setw(14) << user.accountID << setw(20) << user.name.substr(0, 19) << setw(25) << user.email.substr(0, 24) << setw(15) << user.phone << "\n";
    }
    cout << string(77, '-') << "\n\n";

    string input;
    while (true) {
        cout << "[Enter 0 to return back]" << endl;
        cout << "Enter Email or Account ID of user to delete: ";
        getline(cin, input);
        string trimmedInput = trim(input);

        if (trimmedInput == "0") {
            if (confirmAction("Are you sure you want to go back?")) {
                return;
            }
            else {
                continue;
            }
        }

        if (trimmedInput.empty()) {
            cout << "[X] Input cannot be empty. Please enter Email or Account ID.\n";
            continue;
        }

        int targetIndex = -1;
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].accountID == trimmedInput || users[i].email == trimmedInput) {
                targetIndex = static_cast<int>(i);
                break;
            }
        }

        if (targetIndex == -1) {
            cout << "[X] User not found.\n";
            continue;
        }

        const Account& user = users[targetIndex];
        cout << "User found:\n";
        cout << "ID   : " << user.accountID << "\n";
        cout << "Name : " << user.name << "\n";
        cout << "Email: " << user.email << "\n";
        cout << "Phone: " << user.phone << "\n";
        cout << "Reg. : " << user.registrationDate << "\n\n";
        cout << "Are you sure want to delete this account? (Y/N): ";
        string confirm;
        getline(cin, confirm);
        if (confirm != "Y" && confirm != "y") {
            cout << "Deletion cancelled.\n";
            waitForEnter();
            return;
        }

        users.erase(users.begin() + targetIndex);
        User_SaveFile(users);
        cout << "Account deleted successfully.\n";
        waitForEnter();
        return;
    }
}

// ---------- Trim ----------
static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

// ---------- File I/O ----------
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
    admin.accountID = trim(id);
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
        user.accountID = trim(id);
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
        outFile << repair.repairID << ';' << repair.userID << ';' << repair.userName << ';' << repair.bicycleID << ';' << repair.date << ';' << repair.time << ';' << repair.damageType << ';' << repair.status << ';' << repair.station << '\n';
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
        getline(ss, repair.station, ';');

        repair.repairID = trim(repairID);
        repair.userID = trim(userID);
        repair.userName = trim(repair.userName);
        repair.bicycleID = trim(repair.bicycleID);
        repair.date = trim(repair.date);
        repair.time = trim(repair.time);
        repair.damageType = trim(repair.damageType);
        repair.status = trim(repair.status);
        repair.station = trim(repair.station);
        repairs.push_back(repair);
    }
}

// ==================================================================
// BICYCLE INVENTORY FUNCTIONS
// ==================================================================

void DisplayInventoryMenu(vector<Bicycle>& inventory, vector<Booking>& bookings) {
    while (true) {
        clearScreen();
        cout << "===================================\n";
        cout << "      Bicycle Inventory Menu       \n";
        cout << "===================================\n";
        cout << "[1] View all bicycles\n";
        cout << "[2] Add new bicycle\n";
        cout << "[3] Update bicycle\n";
        cout << "[4] Remove bicycle\n";
        cout << "[5] Search by bicycle type\n";
        cout << "[0] Return to main menu\n";

        int option = getValidOption(0, 5, "");
        if (option == -1) continue;

        switch (option) {
        case 1: ViewBicycle(inventory);
            waitForEnter();
            break;
        case 2:
            AddBicycle(inventory);
            break;
        case 3:
            UpdateBicycle(inventory, bookings);
            break;
        case 4:
            RemoveBicycle(inventory);
            break;
        case 5: {
            while (true) {
                clearScreen();
                cout << "===================================\n";
                cout << "       SEARCH BY BICYCLE TYPE      \n";
                cout << "===================================\n";
                cout << "Available types:\n";
                cout << "  [1] city\n";
                cout << "  [2] mountain\n";
                cout << "  [3] road\n";
                cout << "  [4] electric\n";
                cout << "Enter the number between 1-4 (or 0 to cancel): ";

                string input;
                getline(cin, input);
                input = trim(input);

                if (input == "0") {
                    if (confirmAction("Are you sure want to go back?")) {
                        cout << "Search cancelled.\n";
                        waitForEnter();
                        break;
                    }
                    else {
                        continue;
                    }
                }

                string selectedType;
                bool isNumber = true;
                for (char c : input) {
                    if (!isdigit(static_cast<unsigned char>(c))) {
                        isNumber = false;
                        break;
                    }
                }

                if (isNumber) {
                    int num = stoi(input);
                    switch (num) {
                    case 1:
                        selectedType = "city";
                        break;
                    case 2:
                        selectedType = "mountain";
                        break;
                    case 3:
                        selectedType = "road";
                        break;
                    case 4:
                        selectedType = "electric";
                        break;
                    default:
                        cout << "[X] Invalid number. Please choose 1-4.\n";
                        waitForEnter();
                        continue;
                    }
                }
                else {
                    string lowerInput = input;
                    transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
                    if (isValidBikeType(lowerInput)) {
                        selectedType = lowerInput;
                    }
                    else {
                        cout << "[X] Invalid type. Only city, mountain, road, electric are allowed.\n";
                        waitForEnter();
                        continue;
                    }
                }

                vector<Bicycle> matches;
                for (const auto& bike : inventory) {
                    string bikeTypeLower = bike.type;
                    transform(bikeTypeLower.begin(), bikeTypeLower.end(), bikeTypeLower.begin(), ::tolower);
                    if (bikeTypeLower == selectedType) {
                        matches.push_back(bike);
                    }
                }

                if (matches.empty()) {
                    cout << "\nNo bicycles found of type \"" << selectedType << "\".\n";
                    waitForEnter();
                    break;
                }

                cout << "\n==========================================\n";
                cout << "  Bicycles of type: " << selectedType << "\n";
                cout << "==========================================\n";
                cout << left << setw(12) << "ID" << setw(15) << "Type" << setw(15) << "Brand" << setw(15) << "Rate (RM/h)" << setw(12) << "Status" << setw(20) << "Station" << "\n";   // added station
                cout << string(89, '-') << "\n";

                for (const auto& bike : matches) {
                    cout << left << setw(12) << bike.bikeID << setw(15) << bike.type << setw(15) << bike.brand
                        << setw(15) << fixed << setprecision(2) << bike.rentalRatePerHour << setw(12) << bike.status << setw(20) << bike.station << "\n";
                }
                waitForEnter();
                break;
            }
            break;
        }
        case 0:
            if (confirmAction("Are you sure want to go back?")) {
                return;
            }
            break;
        default:
            cout << "[X] Invalid option.\n";
            waitForEnter();
        }
    }
}

bool isValidBikeType(const string& type) {
    static const vector<string> validTypes = { "city", "mountain", "road", "electric" };
    string lower = type;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return find(validTypes.begin(), validTypes.end(), lower) != validTypes.end();
}

bool isInteger(const string& s) {
    if (s.empty()) return false;
    return all_of(s.begin(), s.end(), ::isdigit);
}

bool isDouble(const string& s) {
    if (s.empty()) return false;
    bool hasDot = false;
    for (char c : s) {
        if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
        }
        else if (!isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

string getStationFromUser(bool allowCancel) {
    while (true) {
        cout << "\n===================================\n";
        cout << "    Available Stations (Penang)    \n";
        cout << "===================================\n";
        for (size_t i = 0; i < PENANG_STATIONS.size(); ++i) {
            cout << "  [" << (i + 1) << "] " << PENANG_STATIONS[i] << "\n";
        }
        cout << "Select station by number (1-" << PENANG_STATIONS.size() << ")"
            << (allowCancel ? " or 0 to cancel" : "") << ": ";
        string input;
        getline(cin, input);
        if (input.empty()) {
            cout << "[X] Please enter a number.\n";
            continue;
        }
        bool isNumber = true;
        for (char c : input) {
            if (!isdigit(static_cast<unsigned char>(c))) {
                isNumber = false;
                break;
            }
        }
        if (!isNumber) {
            cout << "[X] Invalid input. Enter a number.\n";
            continue;
        }
        int choice = stoi(input);
        if (allowCancel && choice == 0) {
            return "";
        }
        if (choice >= 1 && choice <= static_cast<int>(PENANG_STATIONS.size())) {
            return PENANG_STATIONS[choice - 1];
        }
        cout << "[X] Invalid choice. Please select between 1 and " << PENANG_STATIONS.size() << ".\n";
    }
}

void AddBicycle(vector<Bicycle>& inventory) {
    clearScreen();
    cout << "===================================\n";
    cout << "          ADD NEW BICYCLE          \n";
    cout << "===================================\n";

    Bicycle bike;
    bike.bikeID = generateBicycleID(inventory);

    string typeInput;
    while (true) {
        cout << "(Enter 0 to go back)\n";
        cout << "Enter Bicycle Type (city, mountain, road, electric): ";
        getline(cin, typeInput);

        if (typeInput == "0") {
            if (confirmAction("Are you sure want to go back? ")) {
                return;
            }
            else {
                continue;
            }
        }

        if (typeInput.empty()) {
            cout << "[X] Type cannot be empty.\n";
            continue;
        }

        string lowerType = typeInput;
        transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
        if (isValidBikeType(lowerType)) {
            bike.type = lowerType;
            break;
        }
        cout << "[X] Invalid type. Allowed types: city, mountain, road, electric.\n";
    }

    cout << "Enter Bicycle Brand (e.g., Giant, Trek): ";
    getline(cin, bike.brand);
    if (bike.brand.empty()) {
        cout << "[X] Brand cannot be empty.\n";
        waitForEnter();
        return;
    }

    if (bike.type == "city")
        bike.rentalRatePerHour = RATE_CITY;
    else if (bike.type == "mountain")
        bike.rentalRatePerHour = RATE_MOUNTAIN;
    else if (bike.type == "road")
        bike.rentalRatePerHour = RATE_ROAD;
    else if (bike.type == "electric")
        bike.rentalRatePerHour = RATE_ELECTRIC;
    else
        bike.rentalRatePerHour = 5.00;

    bike.status = "Available";

    bike.station = getStationFromUser();

    inventory.push_back(bike);
    cout << "\nBicycle added successfully with ID: " << bike.bikeID << "\n";
    cout << "Type: " << bike.type << ", Rate: RM " << fixed << setprecision(2) << bike.rentalRatePerHour << "/hour\n";
    cout << "Station: " << bike.station << "\n";
    SaveInventoryToFile(inventory);
    waitForEnter();
}

void UpdateBicycle(vector<Bicycle>& inventory, const vector<Booking>& bookings) {
    clearScreen();
    ViewBicycle(inventory);
    cout << "\n===================================\n";
    cout << "           UPDATE BICYCLE          \n";
    cout << "===================================\n";

    string id;
    int idx = -1;
    while (true) {
        cout << "Enter the Bicycle ID to update (or 0 to cancel): ";
        getline(cin, id);
        id = trim(id);

        if (id.empty()) {
            cout << "[X] Input cannot be empty.\n";
            waitForEnter();
            continue;
        }

        if (id == "0") {
            if (confirmAction("Are you sure want to go back?")) {
                cout << "Update cancelled.\n";
                waitForEnter();
                return;
            }
            else {
                continue;
            }
        }

        idx = SearchBicycle(inventory, id);
        if (idx == -1) {
            cout << "[X] Bicycle ID not found.\n";
            waitForEnter();
            continue;
        }
        break;
    }

    while (true) {
        clearScreen();
        cout << "=============================================\n";
        cout << " Update Menu for ID: " << inventory[idx].bikeID << "\n";
        cout << "=============================================\n";
        cout << "[1] Type      (current: " << inventory[idx].type << ")\n";
        cout << "[2] Brand     (current: " << inventory[idx].brand << ")\n";
        cout << "[3] Rate      (current: RM" << fixed << setprecision(2) << inventory[idx].rentalRatePerHour << ")\n";
        cout << "[4] Station   (current: " << inventory[idx].station << ")\n";
        cout << "[0] Return to Inventory Menu\n";

        int subOption = getValidOption(0, 4, "");
        if (subOption == -1) continue;

        switch (subOption) {
        case 1:
            cout << "Enter new type: ";
            getline(cin, inventory[idx].type);
            if (inventory[idx].type.empty()) {
                cout << "[X] Type cannot be empty. Keeping previous value.\n";
                waitForEnter();
            }
            else {
                if (isValidBikeType(inventory[idx].type)) {
                    cout << "Type updated.\n";
                }
                else {
                    cout << "[X] Invalid type. Keeping previous value.\n";
                }
                waitForEnter();
            }
            break;
        case 2:
            cout << "Enter new brand: ";
            getline(cin, inventory[idx].brand);
            if (inventory[idx].brand.empty()) {
                cout << "[X] Brand cannot be empty. Keeping previous value.\n";
                waitForEnter();
            }
            else {
                cout << "Brand updated.\n";
                waitForEnter();
            }
            break;
        case 3: {
            double newRate;
            while (true) {
                cout << "Enter new rate (positive, between 1.0 and 100.0): ";
                string rateInput;
                getline(cin, rateInput);

                if (rateInput.empty()) {
                    cout << "[X] Rate cannot be empty.\n";
                    continue;
                }

                bool valid = true;
                bool hasDecimal = false;
                for (char c : rateInput) {
                    if (c == '.') {
                        if (hasDecimal) { valid = false; break; }
                        hasDecimal = true;
                    }
                    else if (!isdigit(static_cast<unsigned char>(c))) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    cout << "[X] Please enter a valid positive number.\n";
                    continue;
                }

                newRate = stod(rateInput);
                if (newRate < 1.0 || newRate > 100.0) {
                    cout << "[X] Rate must be between 1.0 and 100.0.\n";
                    continue;
                }
                break;
            }
            inventory[idx].rentalRatePerHour = newRate;
            cout << "Rate updated.\n";
            waitForEnter();
            break;
        }
        case 4: {
            cout << "Current station: " << inventory[idx].station << "\n";
            string newStation = getStationFromUser();
            inventory[idx].station = newStation;
            cout << "Station updated.\n";
            waitForEnter();
            break;
        }
        case 0:
            if (confirmAction("Are you sure want to go back?")) {
                cout << "Returning to inventory menu.\n";
                waitForEnter();
                return;
            }
            waitForEnter();
            return;
        }
    }
}

void RemoveBicycle(vector<Bicycle>& inventory) {
    clearScreen();
    ViewBicycle(inventory);

    cout << "\n===================================\n";
    cout << "           REMOVE BICYCLE          \n";
    cout << "===================================\n";

    string id;
    int idx = -1;

    while (true) {
        cout << "Enter bicycle ID to remove (or 0 to cancel): ";
        getline(cin, id);
        id = trim(id);

        if (id.empty()) {
            cout << "[X] Input cannot be empty.\n";
            waitForEnter();
            continue;
        }

        if (id == "0") {
            if (confirmAction("Are you sure want to go back? ")) {
                cout << "Deletion cancelled.\n";
                waitForEnter();
                return;
            }
            else {
                continue;
            }
        }


        idx = SearchBicycle(inventory, id);
        if (idx == -1) {
            cout << "[X] Bicycle ID not found.\n";
            waitForEnter();
            continue;
        }

        if (inventory[idx].status == "Rented") {
            cout << "[X] Cannot remove a bicycle that is currently rented.\n";
            waitForEnter();
            continue;
        }
        break;
    }

    cout << "Are you sure want to remove this bicycle? (Y/N): ";
    string confirm;
    getline(cin, confirm);
    if (confirm != "Y" && confirm != "y") {
        cout << "Deletion cancelled.\n";
        waitForEnter();
        return;
    }

    inventory.erase(inventory.begin() + idx);
    cout << "\nBicycle removed successfully.\n";
    waitForEnter();
}

int SearchBicycle(const vector<Bicycle>& inventory, const string& bikeID) {
    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i].bikeID == bikeID)
            return static_cast<int>(i);
    }
    return -1;
}

void ViewBicycle(const vector<Bicycle>& inventory) {
    clearScreen();
    if (inventory.empty()) {
        cout << "No bicycles in inventory.\n";
        return;
    }

    cout << "\n===================================\n";
    cout << "             All Bicycles          \n";
    cout << "===================================\n";
    cout << left << setw(12) << "ID" << setw(15) << "Type" << setw(15) << "Brand" << setw(15) << "Rate (RM/h)" << setw(20) << "Station" << setw(12) << "Status" << "\n";
    cout << string(89, '-') << "\n";

    for (const auto& bike : inventory) {
        cout << left << setw(12) << bike.bikeID << setw(15) << bike.type << setw(15) << bike.brand
            << setw(15) << fixed << setprecision(2) << bike.rentalRatePerHour << setw(20) << bike.station << setw(12) << bike.status << "\n";
    }
}

void SaveInventoryToFile(const vector<Bicycle>& inventory) {
    ofstream outFile(INVENTORY_FILE);
    if (!outFile) {
        cerr << "Error: Cannot open " << INVENTORY_FILE << ".\n";
        return;
    }

    for (const auto& bike : inventory) {
        outFile << bike.bikeID << "," << bike.type << "," << bike.brand << "," << bike.rentalRatePerHour << "," << bike.station << "," << bike.status << "\n";
    }
    outFile.close();
}

void LoadInventoryFromFile(vector<Bicycle>& inventory) {
    ifstream inFile(INVENTORY_FILE);
    if (!inFile) return;

    inventory.clear();
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        Bicycle bike;

        getline(ss, token, ','); bike.bikeID = trim(token);
        getline(ss, token, ','); bike.type = trim(token);
        getline(ss, token, ','); bike.brand = trim(token);
        getline(ss, token, ',');
        if (!isDouble(token)) {
            cerr << "Invalid rate in inventory.csv: " << token << endl;
            continue; // skip this line or set default
        }
        bike.rentalRatePerHour = stod(token);
        getline(ss, token, ','); bike.station = trim(token);
        getline(ss, token, ','); bike.status = trim(token);

        // set defaults if needed
        if (bike.status.empty()) bike.status = "Available";
        if (bike.station.empty()) bike.station = "Unknown";

        inventory.push_back(bike);
    }
    inFile.close();
}

// ==================================================================
// RENTAL BOOKING FUNCTIONS
// ==================================================================
string addHoursToTime(const string& time, int hours) {
    int h = stoi(time.substr(0, 2));
    int m = stoi(time.substr(3, 2));
    h += hours;
    h %= 24;
    ostringstream oss;
    oss << setw(2) << setfill('0') << h << ":" << setw(2) << setfill('0') << m;
    return oss.str();
}

void rentalBookingMenu(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID, const string& customerName,
    const string& membership) {
    while (true) {
        clearScreen();
        cout << "========== RENTAL BOOKING MENU ==========\n";
        cout << "[1] Book a bicycle\n";
        cout << "[2] View booking details\n";
        cout << "[3] Return Bicycle\n";
        cout << "[0] Back to User Menu\n";

        string input;
        cout << "Enter your option (0-3): ";
        getline(cin, input);

        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid option .Please enter 0 - 3.\n";
            waitForEnter();
            continue;
        }
        int option = input[0] - '0';
        if (option < 0 || option > 3) {
            cout << "Invalid option .Please enter 0 - 3.\n";
            waitForEnter();
            continue;
        }

        switch (option) {
        case 1:
            CreateBooking(bookings, inventory, customerID, customerName, membership);
            break;
        case 2:
            ViewMyBookings(bookings, customerID);
            break;
        case 3:
            ReturnBicycle(bookings, inventory, customerID);
            break;
        case 0:
            // CONFIRM BEFORE GOING BACK
            if (confirmAction("Are you sure want to go back?")) {
                cout << "Returning to user menu.\n";
                waitForEnter();
                return;   // Y -> go back
            }
            // N -> stay in the menu (loop repeats)
            break;
        }
    }
}

void CreateBooking(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID, const string& customerName,
    const string& membership) {
    clearScreen();

    string pickupStation;
    while (true) {
        cout << "===== BOOK A BICYCLE =====\n";
        pickupStation = getStationFromUser(true);
        if (pickupStation.empty()) {
            if (confirmAction("Are you sure want to cancel the booking?")) {
                cout << "Booking cancelled.\n";
                waitForEnter();
                return;
            }
            else {
                clearScreen();
                continue;
            }
        }
        break;
    }

    clearScreen();
    cout << "Available bicycles at " << pickupStation << ":\n\n";
    int cityCount = 0, mountainCount = 0, roadCount = 0, electricCount = 0;
    for (const auto& bike : inventory) {
        if (bike.station == pickupStation && bike.status == "Available") {
            if (bike.type == "city") cityCount++;
            else if (bike.type == "mountain") mountainCount++;
            else if (bike.type == "road") roadCount++;
            else if (bike.type == "electric") electricCount++;
        }
    }
    cout << "[1] City     (" << cityCount << " available)\n";
    cout << "[2] Mountain (" << mountainCount << " available)\n";
    cout << "[3] Road     (" << roadCount << " available)\n";
    cout << "[4] Electric (" << electricCount << " available)\n";
    cout << "[0] Cancel\n";

    string typeInput;
    cout << "Select bike type (1-4 or 0 to cancel): ";
    getline(cin, typeInput);
    if (typeInput == "0") {
        if (confirmAction("Are you sure you want to go back? (Y/N): ")) {
            cout << "Booking cancelled.\n";
            waitForEnter();
            return;
        }
        else {
            // If N, we need to loop back; for simplicity we return
            return;
        }
    }
    int typeChoice;
    if (typeInput.length() != 1 || !isdigit(typeInput[0])) {
        cout << "[X] Invalid choice.\n";
        waitForEnter();
        return;
    }
    typeChoice = typeInput[0] - '0';
    if (typeChoice < 1 || typeChoice > 4) {
        cout << "[X] Invalid choice.\n";
        waitForEnter();
        return;
    }

    string selectedType;
    switch (typeChoice) {
    case 1: selectedType = "city"; break;
    case 2: selectedType = "mountain"; break;
    case 3: selectedType = "road"; break;
    case 4: selectedType = "electric"; break;
    }

    // Find first available bike of selected type at that station
    string assignedBikeID = "";
    for (const auto& bike : inventory) {
        if (bike.type == selectedType && bike.station == pickupStation &&
            bike.status == "Available") {
            assignedBikeID = bike.bikeID;
            break;
        }
    }
    if (assignedBikeID.empty()) {
        cout << "[X] No available " << selectedType << " bicycles at " << pickupStation << ".\n";
        waitForEnter();
        return;
    }
    cout << "Assigned bicycle: " << assignedBikeID << " (" << selectedType << ")\n";

    // 3. Duration
    int duration;
    while (true) {
        cout << "Enter rental duration in hours (1 - " << MAX_RENTAL_HOURS << "): ";
        string durInput;
        getline(cin, durInput);
        if (durInput.empty()) {
            cout << "[X] Duration cannot be empty.\n";
            continue;
        }
        bool valid = true;
        for (char c : durInput) {
            if (!isdigit(c)) { valid = false; break; }
        }
        if (!valid) {
            cout << "[X] Duration must be numeric.\n";
            continue;
        }
        duration = stoi(durInput);
        if (duration <= 0 || duration > MAX_RENTAL_HOURS) {
            cout << "Invalid duration. Please enter a value between 1 and " << MAX_RENTAL_HOURS << ".\n";
        }
        else {
            break;
        }
    }

    // 4. Current date and time
    string bookingDate, bookingTime;
    getCurrentDateTime(bookingDate, bookingTime);

    // 5. Return time = current time + duration hours
    string returnTime = addHoursToTime(bookingTime, duration);

    // 6. Base rate from assigned bike
    double baseRate = 0.0;
    for (const auto& bike : inventory) {
        if (bike.bikeID == assignedBikeID) {
            baseRate = bike.rentalRatePerHour;
            break;
        }
    }

    // 7. Create temporary booking
    Booking tempBooking;
    tempBooking.bookingID = generateBookingID(bookings);
    tempBooking.customerID = customerID;
    tempBooking.bikeID = assignedBikeID;
    tempBooking.bookingDate = bookingDate;
    tempBooking.bookingTime = bookingTime;
    tempBooking.pickupStation = pickupStation;
    tempBooking.returnTime = returnTime;
    tempBooking.paymentMethod = "";
    tempBooking.membership = membership;
    tempBooking.duration = duration;
    tempBooking.status = "Active";

    // ---- CONFIRM BOOKING DETAILS BEFORE PAYMENT ----
    clearScreen();
    cout << "===== BOOKING SUMMARY =====\n";
    cout << "Booking ID     : " << tempBooking.bookingID << "\n";
    cout << "Customer ID    : " << tempBooking.customerID << "\n";
    cout << "Bicycle ID     : " << tempBooking.bikeID << "\n";
    cout << "Date           : " << tempBooking.bookingDate << "\n";
    cout << "Time           : " << tempBooking.bookingTime << "\n";
    cout << "Pickup Station : " << tempBooking.pickupStation << "\n";
    cout << "Return Time    : " << tempBooking.returnTime << "\n";
    cout << "Duration       : " << tempBooking.duration << " hours\n";
    cout << "Membership     : " << tempBooking.membership << "\n";
    cout << "--------------------------\n";

    if (!confirmAction("Do you confirm all details are correct?")) {
        cout << "\nBooking cancelled.\n";
        waitForEnter();
        return;   // exit without payment
    }

    // 8. Process payment
    string paymentMethod = ProcessPaymentForBooking(tempBooking, duration, baseRate, customerName);
    if (paymentMethod.empty()) {
        cout << "Payment failed or cancelled. Booking not created.\n";
        waitForEnter();
        return;
    }
    tempBooking.paymentMethod = paymentMethod;

    // 9. Mark bike as Rented
    for (auto& bike : inventory) {
        if (bike.bikeID == assignedBikeID) {
            bike.status = "Rented";
            break;
        }
    }

    // 10. Save booking
    bookings.push_back(tempBooking);
    SaveBookingsToFile(bookings, BOOKING_FILE);

    cout << "\nBooking created successfully!\n";
    cout << "Booking ID: " << tempBooking.bookingID << "\n";
    cout << "Bicycle   : " << tempBooking.bikeID << "\n";
    cout << "Date/Time : " << tempBooking.bookingDate << " " << tempBooking.bookingTime << "\n";
    cout << "Return at : " << tempBooking.returnTime << "\n";
    cout << "Pickup at : " << tempBooking.pickupStation << "\n";
    waitForEnter();
}

void ViewMyBookings(const vector<Booking>& bookings, const string& customerID) {
    clearScreen();

    vector<const Booking*> myBookings;
    for (const auto& b : bookings) {
        if (b.customerID == customerID) {
            myBookings.push_back(&b);
        }
    }
    if (myBookings.empty()) {
        cout << "No bookings found for this customer.\n";
        waitForEnter();
        return;
    }

    const int PER_PAGE = 5;
    int page = 0;
    int totalPages = (myBookings.size() + PER_PAGE - 1) / PER_PAGE;

    while (true) {
        clearScreen();
        cout << "===== MY BOOKING DETAILS =====\n";
        cout << left
            << setw(12) << "Booking ID"
            << setw(12) << "Date"
            << setw(8) << "Time"
            << setw(20) << "Pickup Station"
            << setw(8) << "Return"
            << setw(16) << "Payment Method"
            << "\n";
        cout << string(76, '-') << "\n";

        size_t first = page * PER_PAGE;
        size_t last = min(first + PER_PAGE, myBookings.size());
        for (size_t i = first; i < last; ++i) {
            const Booking& b = *myBookings[i];
            cout << left
                << setw(12) << b.bookingID
                << setw(12) << b.bookingDate
                << setw(8) << b.bookingTime
                << setw(20) << b.pickupStation
                << setw(8) << b.returnTime
                << setw(16) << b.paymentMethod
                << "\n";
        }

        cout << string(76, '-') << "\n";
        cout << "Page " << page + 1 << "/" << totalPages << "\n\n";

        cout << "[1] previous  [2] Next Page  [0] Back\n";
        string input;
        cout << "Enter your option (0-2): ";
        getline(cin, input);
        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid input. Please enter 0, 1 or 2.\n";
            waitForEnter();
            continue;
        }
        int opt = input[0] - '0';
        if (opt == 0) {
            // CONFIRM BEFORE GOING BACK
            if (confirmAction("Are you sure you want to go back?")) {
                return;   // Y -> exit pagination
            }
            // N -> stay on current page (loop continues)
        }
        else if (opt == 1) {
            if (page > 0) page--;
            else { cout << "This is the first page.\n"; waitForEnter(); }
        }
        else if (opt == 2) {
            if (page + 1 < totalPages) page++;
            else { cout << "This is the last page.\n"; waitForEnter(); }
        }
    }
}

void ReturnBicycle(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID) {
    clearScreen();
    cout << "===== RETURN BICYCLE =====\n";

    vector<Booking*> active;
    for (auto& b : bookings) {
        if (b.customerID == customerID && b.status == "Active") {
            active.push_back(&b);
        }
    }
    if (active.empty()) {
        cout << "No active bookings to return.\n";
        waitForEnter();
        return;
    }

    cout << "Your active bookings:\n";
    for (size_t i = 0; i < active.size(); ++i) {
        cout << "[" << i + 1 << "] Booking " << active[i]->bookingID
            << " | Bike: " << active[i]->bikeID
            << " | Return by: " << active[i]->returnTime << "\n";
    }
    cout << "[0] Back\n";

    string choiceInput;
    cout << "Select booking to return (1-" << active.size() << " or 0): ";
    getline(cin, choiceInput);

    if (choiceInput == "0") {
        // CONFIRM BEFORE GOING BACK
        if (confirmAction("Are you sure you want to go back?")) {
            cout << "Return cancelled.\n";
            waitForEnter();
            return;
        }
        // If N, we could loop back; for simplicity we return.
        return;
    }

    int idx;
    if (choiceInput.length() != 1 || !isdigit(choiceInput[0])) {
        cout << "[X] Invalid choice.\n";
        waitForEnter();
        return;
    }
    idx = choiceInput[0] - '0';
    if (idx < 1 || idx > static_cast<int>(active.size())) {
        cout << "[X] Invalid choice.\n";
        waitForEnter();
        return;
    }

    Booking& selected = *active[idx - 1];

    // Ask for return station
    string returnStation = getStationFromUser();

    // CONFIRM RETURN
    clearScreen();
    cout << "===== RETURN SUMMARY =====\n";
    cout << "Booking ID    : " << selected.bookingID << "\n";
    cout << "Bicycle ID    : " << selected.bikeID << "\n";
    cout << "Pickup Station: " << selected.pickupStation << "\n";
    cout << "Return Station: " << returnStation << "\n";
    cout << "Return Time   : " << selected.returnTime << "\n";
    cout << "--------------------------\n";

    if (!confirmAction("Do you confirm the return of this bicycle?")) {
        cout << "\nReturn cancelled.\n";
        waitForEnter();
        return;
    }

    // Update bike status and station
    for (auto& bike : inventory) {
        if (bike.bikeID == selected.bikeID) {
            bike.status = "Available";
            bike.station = returnStation;
            break;
        }
    }

    selected.status = "Completed";
    SaveBookingsToFile(bookings, BOOKING_FILE);

    cout << "\nBicycle returned successfully.\n";
    cout << "Booking ID: " << selected.bookingID << " completed.\n";
    cout << "Bicycle now at: " << returnStation << "\n";
    waitForEnter();
}

void SaveBookingsToFile(const vector<Booking>& bookings, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error opening file " << filename << " for writing.\n";
        return;
    }
    for (const auto& b : bookings) {
        outFile << b.bookingID << ","
            << b.customerID << ","
            << b.bikeID << ","
            << b.bookingDate << ","
            << b.bookingTime << ","
            << b.pickupStation << ","
            << b.returnTime << ","
            << b.paymentMethod << ","
            << b.membership << ","
            << b.duration << ","
            << b.status << "\n";
    }
    outFile.close();
}

void LoadBookingsFromFile(vector<Booking>& bookings, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) return;
    bookings.clear();
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        Booking b;
        getline(ss, token, ','); b.bookingID = trim(token);
        getline(ss, token, ','); b.customerID = trim(token);
        getline(ss, token, ','); b.bikeID = trim(token);
        getline(ss, token, ','); b.bookingDate = trim(token);
        getline(ss, token, ','); b.bookingTime = trim(token);
        getline(ss, token, ','); b.pickupStation = trim(token);
        getline(ss, token, ','); b.returnTime = trim(token);
        getline(ss, token, ','); b.paymentMethod = trim(token);
        getline(ss, token, ','); b.membership = trim(token);
        getline(ss, token, ',');
        if (!isInteger(token)) {
            cerr << "Invalid duration in booking.csv: " << token << endl;
            continue;
        }
        b.duration = stoi(token);
        getline(ss, token, ','); b.status = trim(token);
        bookings.push_back(b);
    }
    inFile.close();
}

void SyncInventoryWithBookings(const vector<Booking>& bookings, vector<Bicycle>& inventory) {
    for (auto& bike : inventory) {
        if (bike.status != "Repair") {
            bike.status = "Available";
        }
    }
    for (const auto& b : bookings) {
        if (b.status == "Active") {
            for (auto& bike : inventory) {
                if (bike.bikeID == b.bikeID && bike.status != "Repair") {
                    bike.status = "Rented";
                    break;
                }
            }
        }
    }
}

bool checkAvailability(const string& bikeID, const vector<Bicycle>& inventory) {
    for (const auto& bike : inventory) {
        if (bike.bikeID == bikeID) {
            return bike.status == "Available";
        }
    }
    return false;
}

bool hasActiveBookingForBike(const string& bikeID, const vector<Booking>& bookings) {
    for (const auto& b : bookings) {
        if (b.bikeID == bikeID && b.status == "Active") {
            return true;
        }
    }
    return false;
}

// ==================================================================
// PAYMENT FUNCTIONS
// ==================================================================

double CalculateRentalFee(double hours, double baseRate) {
    return hours * baseRate;
}

double CalculateOvertimeFee(double overtimeHours) {
    return overtimeHours * OVERTIME_RATE;
}

bool isValidCardNumber(const string& cardNum) {
    if (cardNum.length() != 16) return false;
    for (char c : cardNum)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

bool isValidCVV(const string& cvv) {
    if (cvv.length() != 3) return false;
    for (char c : cvv)
        if (!isdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

string ProcessPaymentForBooking(const Booking& booking, double rentalHours,
    double baseRate, const string& customerName) {
    double baseFee = CalculateRentalFee(rentalHours, baseRate);
    double overtimeFee = 0.0;
    if (rentalHours > 24.0)
        overtimeFee = CalculateOvertimeFee(rentalHours - 24.0);

    double subtotal = baseFee + overtimeFee;
    if (booking.membership == "Member") {
        subtotal -= subtotal * MEMBER_DISCOUNT;
    }
    double tax = subtotal * TAX_RATE;
    double total = subtotal + tax;

    clearScreen();
    cout << fixed << setprecision(2);
    cout << "\n========== PAYMENT SUMMARY ==========\n";
    cout << "Booking ID   : " << booking.bookingID << "\n";
    cout << "Bike ID      : " << booking.bikeID << "\n";
    cout << "Customer     : " << customerName << "\n";
    cout << "Duration     : " << rentalHours << " hours\n";
    cout << "Base Rate    : RM " << baseRate << "/hour\n";
    cout << "Base Fee     : RM " << baseFee << "\n";
    if (overtimeFee > 0)
        cout << "Overtime Fee : RM " << overtimeFee << " (over 24 hrs)\n";
    if (booking.membership == "Member")
        cout << "Member Disc : -RM " << (baseFee + overtimeFee) * MEMBER_DISCOUNT << "\n";
    cout << "Subtotal     : RM " << subtotal << "\n";
    cout << "Tax (6%)     : RM " << tax << "\n";
    cout << "Total Amount : RM " << total << "\n";
    cout << "=====================================\n\n";

    double paid;
    while (true) {
        cout << "Enter payment amount (RM): ";
        string input;
        getline(cin, input);
        if (input.empty()) {
            cout << "[X] Amount cannot be empty.\n";
            continue;
        }
        bool valid = true;
        for (char c : input) {
            if (!isdigit(static_cast<unsigned char>(c)) && c != '.') {
                valid = false;
                break;
            }
        }
        if (!valid) {
            cout << "[X] Please enter a valid number.\n";
            continue;
        }
        paid = stod(input);
        if (paid < total) {
            cout << "[X] Insufficient. Must enter at least RM " << total << "\n";
        }
        else {
            break;
        }
    }

    int methodChoice; string method;
    cout << "Payment method:\n1. Debit Card\n2. Credit Card\n3. Touch'n Go\nChoice: ";
    string methInput;
    getline(cin, methInput);
    if (methInput.empty() || methInput.length() != 1 || !isdigit(methInput[0])) {
        cout << "Invalid choice. Defaulting to Debit Card.\n";
        methodChoice = 1;
    }
    else {
        methodChoice = methInput[0] - '0';
        if (methodChoice < 1 || methodChoice > 3) methodChoice = 1;
    }
    switch (methodChoice) {
    case 1: method = "Debit Card"; break;
    case 2: method = "Credit Card"; break;
    case 3: method = "Touch'n Go"; break;
    default: method = "Unknown";
    }

    if (methodChoice == 1 || methodChoice == 2) {
        string cardNum, expiry, cvv;

        bool cardOk = false;
        do {
            cout << "Card number (e.g., 1111222233334444): ";
            getline(cin, cardNum);
            if (!isValidCardNumber(cardNum)) {
                cout << "[X] Invalid card number. The card number must be 16 digits.\n";
            }
            else {
                cardOk = true;
            }
        } while (!cardOk);

        bool expiryOk = false;
        do {
            cout << "Expiry (MM/YY): ";
            getline(cin, expiry);
            if (!isValidExpiry(expiry)) {
                cout << "[X] Invalid expiry date (must be MM/YY and not expired).\n";
            }
            else {
                expiryOk = true;
            }
        } while (!expiryOk);

        bool cvvOk = false;
        do {
            cout << "CVV (3 digits): ";
            getline(cin, cvv);
            if (!isValidCVV(cvv)) {
                cout << "[X] Invalid CVV.\n";
            }
            else {
                cvvOk = true;
            }
        } while (!cvvOk);

        cout << "Card verified.\n";
    }
    else if (methodChoice == 3) {
        cout << "Touch'n Go selected.\n";
    }

    Payment newP;
    newP.paymentID = generatePaymentID(transactions);
    newP.bookingID = booking.bookingID;
    newP.bikeID = booking.bikeID;
    newP.customerName = customerName;
    newP.paymentAmount = paid;
    newP.paymentMethod = method;
    string date, time;
    getCurrentDateTime(date, time);
    newP.paymentDate = date;

    transactions.push_back(newP);
    SavePaymentToFile();

    cout << "\nPayment successful! Payment ID: " << newP.paymentID << "\n";
    GenerateReceipt(newP.paymentID);
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return method;
}

bool isValidExpiry(const string& expiry) {
    if (expiry.length() != 5 || expiry[2] != '/') return false;
    for (int i = 0; i < 5; ++i) {
        if (i == 2) continue; 
        if (!isdigit(static_cast<unsigned char>(expiry[i]))) return false;
    }

    int month = stoi(expiry.substr(0, 2));
    int year = stoi(expiry.substr(3, 2));

    if (month < 1 || month > 12) return false;

    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm localTime;
    localtime_s(&localTime, &tt);
    int currentYear = localTime.tm_year % 100;   
    int currentMonth = localTime.tm_mon + 1;

    if (year < currentYear) return false;
    if (year == currentYear && month < currentMonth) return false;

    return true;
}

void RequestRefund(vector<Account>& users, int currentIdx, vector<Booking>& bookings, vector<RepairReport>& repairs, vector<Payment>& transactions) {
    const string indent = string(0, ' ');
    clearScreen();
    cout << indent << "==========================\n";
    cout << indent << "      REQUEST REFUND      \n";
    cout << indent << "==========================\n";

    const Account& user = users[currentIdx];

    vector<Booking> userBookings;
    for (const auto& b : bookings) {
        if (b.customerID == user.accountID && (b.status == "Active" || b.status == "Completed")) {
            bool alreadyRefunded = false;
            for (const auto& p : transactions) {
                if (p.bookingID == b.bookingID && p.paymentAmount < 0) {
                    alreadyRefunded = true;
                    break;
                }
            }
            if (!alreadyRefunded) {
                userBookings.push_back(b);
            }
        }
    }

    if (userBookings.empty()) {
        cout << indent << "No bookings found.\n";
        waitForEnter(indent);
        return;
    }

    while (true) {
        cout << indent << "Select a booking to request refund:\n";
        for (size_t i = 0; i < userBookings.size(); ++i) {
            cout << indent << "[" << i + 1 << "] BookingID: " << userBookings[i].bookingID
                << " | BikeID: " << userBookings[i].bikeID
                << " | Duration: " << userBookings[i].duration << "h"
                << " | Status: " << userBookings[i].status << "\n";
        }
        cout << indent << "[0] Cancel\n";

        int choice = getValidOption(0, static_cast<int>(userBookings.size()), indent);

        if (choice == 0) {
            if (confirmAction("Are you sure want to cancel the request? ")) {
                cout << indent << "Refund cancelled.\n";
                waitForEnter(indent);
                return;    
            }
            else {
                continue;     
            }
        }

        const Booking& selected = userBookings[choice - 1];

        bool damageReported = false;
        for (const auto& r : repairs) {
            if (r.userID == user.accountID && r.bicycleID == selected.bikeID
                && (r.status == "Completed" || r.status == "In Progress")) {
                damageReported = true;
                break;
            }
        }

        if (!damageReported) {
            cout << indent << "[X] This bicycle is not found in the reports. Refund not allowed.\n";
            cout << indent << "Only reports with status 'Completed' or 'In Progress' can request refunds.\n";
            waitForEnter(indent);
            continue;  
        }

        double paidAmount = 0.0;
        for (const auto& p : transactions) {
            if (p.bookingID == selected.bookingID && p.paymentAmount > 0) {
                paidAmount = p.paymentAmount;
                break;
            }
        }

        if (paidAmount == 0.0) {
            cout << indent << "[X] No payment record found for this booking.\n";
            waitForEnter(indent);
            continue;
        }

        cout << indent << "Booking " << selected.bookingID
            << " originally paid RM " << fixed << setprecision(2) << paidAmount << "\n";
        cout << indent << "Are you sure want to request a full refund? (Y/N): ";
        string confirm;
        getline(cin, confirm);
        if (confirm != "Y" && confirm != "y") {
            cout << indent << "Refund cancelled.\n";
            waitForEnter(indent);
            continue;
        }

        // ---- Process refund ----
        Payment refund;
        refund.paymentID = generatePaymentID(transactions);
        refund.bookingID = selected.bookingID;
        refund.bikeID = selected.bikeID;
        refund.customerName = user.name;
        refund.paymentAmount = -paidAmount;
        refund.paymentMethod = "Refund";
        string date, time;
        getCurrentDateTime(date, time);
        refund.paymentDate = date;

        transactions.push_back(refund);
        SavePaymentToFile();

        cout << indent << "\nRefund of RM " << fixed << setprecision(2) << paidAmount << " processed successfully.\n";
        cout << indent << "Refund ID: " << refund.paymentID << "\n";
        waitForEnter(indent);
        return;   
    }
}

void DisplayUserPaymentHistory(const string& customerName, const vector<Booking>& bookings) {
    clearScreen();
    if (transactions.empty()) {
        cout << "No payment records found.\n";
        waitForEnter();
        return;
    }

    bool found = false;
    cout << "\n" << string(24, ' ') << "========================================";
    cout << "\n" << string(24, ' ') << "           MY PAYMENT HISTORY           ";
    cout << "\n" << string(24, ' ') << "========================================\n";
    cout << left << setw(12) << "PaymentID" << setw(12) << "Booking" << setw(12) << "Bike" << setw(10) << "Hours" << setw(12) << "Amount"
        << setw(18) << "Method" << setw(12) << "Date" << "\n";
    cout << string(88, '-') << "\n";

    for (const auto& p : transactions) {
        if (p.customerName == customerName) {
            string durationStr = "-";
            for (const auto& b : bookings) {
                if (b.bookingID == p.bookingID) {
                    durationStr = to_string(b.duration) + "h";
                    break;
                }
            }

            found = true;
            cout << left << setw(12) << p.paymentID << setw(12) << p.bookingID << setw(12) << p.bikeID << setw(10) << durationStr
                << setw(12) << fixed << setprecision(2) << p.paymentAmount << setw(18) << p.paymentMethod << setw(12) << p.paymentDate << "\n";
        }
    }
    if (!found)
        cout << "No payment records found for this customer.\n";
    waitForEnter();
}

void DisplayAllPayments(const vector<Booking>& bookings) {
    clearScreen();
    if (transactions.empty()) {
        cout << "No payment records found.\n";
        waitForEnter();
        return;
    }

    cout << "\n" << string(35, ' ') << "===================================";
    cout << "\n" << string(35, ' ') << "          ALL TRANSACTIONS          ";
    cout << "\n" << string(35, ' ') << "===================================\n";
    cout << left << setw(12) << "PaymentID" << setw(12) << "Booking" << setw(12) << "Bike" << setw(20) << "Customer"
        << setw(10) << "Hours" << setw(12) << "Amount" << setw(18) << "Method" << setw(12) << "Date" << "\n";
    cout << string(108, '-') << "\n";

    for (const auto& p : transactions) {
        string durationStr = "-";
        for (const auto& b : bookings) {
            if (b.bookingID == p.bookingID) {
                durationStr = to_string(b.duration) + "h";
                break;
            }
        }

        cout << left << setw(12) << p.paymentID
            << setw(12) << p.bookingID << setw(12) << p.bikeID << setw(20) << p.customerName << setw(10) << durationStr << setw(12)
            << fixed << setprecision(2) << p.paymentAmount << setw(18) << p.paymentMethod << setw(12) << p.paymentDate << "\n";
    }
    waitForEnter();
}

void GenerateReceipt(const string& paymentID) {
    for (const auto& p : transactions) {
        if (p.paymentID == paymentID) {
            cout << "\n========== RECEIPT ==========\n";
            cout << "Payment ID   : " << p.paymentID << "\n";
            cout << "Booking ID   : " << p.bookingID << "\n";
            cout << "Bike ID      : " << p.bikeID << "\n";
            cout << "Customer     : " << p.customerName << "\n";
            cout << "Amount Paid  : RM " << fixed << setprecision(2) << p.paymentAmount << "\n";
            cout << "Method       : " << p.paymentMethod << "\n";
            cout << "Date         : " << p.paymentDate << "\n";
            cout << "Thank you for using Bike 2U!\n";
            cout << string(32, '=') << "\n";
            return;
        }
    }
    cout << "[X] Payment ID not found.\n";
}

void printQRCode(const string& data)
{
    QrCode qr = QrCode::encodeText(data.c_str(), QrCode::Ecc::MEDIUM);
    int size = qr.getSize();
    int border = 2;

    cout << "\n";
    cout << "==================== QR CODE ====================\n\n";

    for (int y = -border; y < size + border; y++)
    {
        for (int x = -border; x < size + border; x++)
        {
            bool black = false;
            if (x >= 0 && x < size && y >= 0 && y < size)
                black = qr.getModule(x, y);

            cout << (black ? "██" : "  ");
        }
        cout << '\n';
    }
    cout << "\n==================================================\n";
}

void generateQRCodeSVG(const string& data, const string& filename)
{
    QrCode qr = QrCode::encodeText(data.c_str(), QrCode::Ecc::MEDIUM);
    int border = 4;
    int size = qr.getSize();

    ofstream file(filename);
    if (!file)
    {
        cout << "\nError: Unable to create QR code file.\n";
        return;
    }

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 "
        << size + border * 2 << " " << size + border * 2 << "\">\n";
    file << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    file << "<path d=\"";

    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
            if (qr.getModule(x, y))
                file << "M" << x + border << "," << y + border << "h1v1h-1z ";

    file << "\" fill=\"black\"/>\n";
    file << "</svg>\n";
    file.close();

    cout << "Saved to: " << std::filesystem::absolute(filename) << "\n";
    cout << "\nQR Code SVG generated: " << filename << "\n";
}

void generateInvoiceHTML(
    const string& invoiceNo,
    const string& customerName,
    const string& bicycleID,
    int hours,
    double pricePerHour,
    double rentalTotal,
    double sst,
    double grandTotal,
    const string& qrData,
    const string& htmlFilename)
{
    // Generate QR code as SVG string
    QrCode qr = QrCode::encodeText(qrData.c_str(), QrCode::Ecc::MEDIUM);
    int border = 4;
    int size = qr.getSize();

    ostringstream svgStream;
    svgStream << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 "
        << size + border * 2 << " " << size + border * 2 << "\">\n";
    svgStream << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    svgStream << "<path d=\"";

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (qr.getModule(x, y)) {
                svgStream << "M" << x + border << "," << y + border << "h1v1h-1z ";
            }
        }
    }
    svgStream << "\" fill=\"black\"/>\n</svg>";

    string svgContent = svgStream.str();

    ofstream htmlFile(htmlFilename);
    if (!htmlFile) {
        cout << "Error: Could not create " << htmlFilename << "\n";
        return;
    }

    htmlFile << "<!DOCTYPE html>\n";
    htmlFile << "<html lang=\"en\">\n";
    htmlFile << "<head>\n";
    htmlFile << "    <meta charset=\"UTF-8\">\n";
    htmlFile << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    htmlFile << "    <title> Bike 2U Invoice " << invoiceNo << "</title>\n";
    htmlFile << "    <style>\n";
    htmlFile << "        body { font-family: Arial, sans-serif; max-width: 600px; margin: 40px auto; padding: 20px; background: #f9f9f9; }\n";
    htmlFile << "        .invoice { background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
    htmlFile << "        h1 { color: #333; border-bottom: 2px solid #4CAF50; padding-bottom: 10px; }\n";
    htmlFile << "        .detail { margin: 8px 0; }\n";
    htmlFile << "        .label { font-weight: bold; display: inline-block; width: 140px; }\n";
    htmlFile << "        .total { font-size: 1.2em; font-weight: bold; color: #4CAF50; margin-top: 15px; border-top: 2px solid #eee; padding-top: 15px; }\n";
    htmlFile << "        .qr-container { text-align: center; margin-top: 30px; }\n";
    htmlFile << "        .qr-container svg { max-width: 100%; height: auto; background: white; padding: 10px; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }\n";
    htmlFile << "        .footer { text-align: center; margin-top: 30px; font-size: 0.9em; color: #777; }\n";
    htmlFile << "    </style>\n";
    htmlFile << "</head>\n";
    htmlFile << "<body>\n";
    htmlFile << "    <div class=\"invoice\">\n";
    htmlFile << "        <h1> Bike 2U Invoice: " << invoiceNo << "</h1>\n";
    htmlFile << "        <div class=\"detail\"><span class=\"label\">Customer</span> " << customerName << "</div>\n";
    htmlFile << "        <div class=\"detail\"><span class=\"label\">Bicycle ID</span> " << bicycleID << "</div>\n";
    htmlFile << "        <div class=\"detail\"><span class=\"label\">Rental Hours</span> " << hours << "</div>\n";
    htmlFile << "        <div class=\"detail\"><span class=\"label\">Price per Hour</span> RM " << fixed << setprecision(2) << pricePerHour << "</div>\n";
    htmlFile << "        <div class=\"detail\"><span class=\"label\">Rental Total</span> RM " << fixed << setprecision(2) << rentalTotal << "</div>\n";
    htmlFile << "        <div class=\"detail\"><span class=\"label\">SST (6%)</span> RM " << fixed << setprecision(2) << sst << "</div>\n";
    htmlFile << "        <div class=\"detail total\"><span class=\"label\">Grand Total</span> RM " << fixed << setprecision(2) << grandTotal << "</div>\n";
    htmlFile << "        <div class=\"qr-container\">\n";
    htmlFile << "            <h3>Scan to view invoice data</h3>\n";
    htmlFile << "            " << svgContent << "\n";
    htmlFile << "        </div>\n";
    htmlFile << "    </div>\n";
    htmlFile << "    <div class=\"footer\">Generated by Bike 2U System</div>\n";
    htmlFile << "</body>\n";
    htmlFile << "</html>\n";

    htmlFile.close();
    cout << "HTML invoice generated: " << htmlFilename << "\n";
}

void printInvoice(
    const string& invoiceNo,
    const string& customerName,
    const string& bicycleID,
    int hours,
    double pricePerHour,
    const string& baseFilename)
{
    const double SST_RATE = 0.06;

    double rentalTotal = hours * pricePerHour;
    double sst = rentalTotal * SST_RATE;
    double grandTotal = rentalTotal + sst;

    // Print invoice to terminal
    cout << "\n\n";
    cout << "============================================\n";
    cout << "              INVOICE SYSTEM                \n";
    cout << "============================================\n";
    cout << "Invoice No : " << invoiceNo << "\n";
    cout << "Customer   : " << customerName << "\n";
    cout << "--------------------------------------------\n";

    cout << "BICYCLE RENTAL\n";
    cout << "  Bicycle ID : " << bicycleID << "\n";
    cout << "  Hours      : " << hours << "\n";
    cout << "  Price/Hour : RM " << fixed << setprecision(2) << pricePerHour << "\n";
    cout << "  Rental Total: RM " << fixed << setprecision(2) << rentalTotal << "\n";
    cout << "--------------------------------------------\n";

    cout << right
        << setw(32) << "Rental Total: RM "
        << fixed << setprecision(2) << rentalTotal << "\n";
    cout << right
        << setw(32) << "SST (6%): RM "
        << fixed << setprecision(2) << sst << "\n";
    cout << right
        << setw(32) << "Grand Total: RM "
        << fixed << setprecision(2) << grandTotal << "\n";
    cout << "============================================\n";

    // Build QR data
    ostringstream priceStream, totalStream;
    priceStream << fixed << setprecision(2) << pricePerHour;
    totalStream << fixed << setprecision(2) << grandTotal;

    string qrData =
        "Invoice ID: " + invoiceNo +
        "\nCustomer: " + customerName +
        "\nBicycle ID: " + bicycleID +
        "\nHours: " + to_string(hours) +
        "\nPrice per hour: RM " + priceStream.str() +
        "\nTotal: RM " + totalStream.str();

    cout << "\nQR CODE INFORMATION\n";
    cout << "--------------------------------------------\n";
    cout << qrData << "\n";
    cout << "--------------------------------------------\n";

    printQRCode(qrData);

    // Generate SVG and HTML files with unique names
    string svgFilename = baseFilename + ".svg";
    string htmlFilename = baseFilename + ".html";

    generateQRCodeSVG(qrData, svgFilename);
    generateInvoiceHTML(
        invoiceNo,
        customerName,
        bicycleID,
        hours,
        pricePerHour,
        rentalTotal,
        sst,
        grandTotal,
        qrData,
        htmlFilename
    );
}

void SavePaymentToFile() {
    ofstream out(PAYMENT_FILE);
    if (!out) return;
    out << "PaymentID,BookingID,BikeID,CustomerName,Amount,Method,Date\n";
    for (const auto& p : transactions) {
        out << p.paymentID << "," << p.bookingID << "," << p.bikeID << "," << p.customerName << ","
            << fixed << setprecision(2) << p.paymentAmount << "," << p.paymentMethod << "," << p.paymentDate << "\n";
    }
    out.close();
}

void LoadPaymentFromFile() {
    ifstream in(PAYMENT_FILE);
    if (!in) return;
    string line;
    getline(in, line); // skip header
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        Payment p;
        getline(ss, token, ','); p.paymentID = trim(token);
        getline(ss, token, ','); p.bookingID = trim(token);
        getline(ss, token, ','); p.bikeID = trim(token);
        getline(ss, token, ','); p.customerName = trim(token);
        getline(ss, token, ',');
        if (!isDouble(token)) {
            cerr << "Invalid amount in payment.csv: " << token << endl;
            continue;
        }
        p.paymentAmount = stod(token);
        getline(ss, token, ','); p.paymentMethod = trim(token);
        getline(ss, token, ','); p.paymentDate = trim(token);
        transactions.push_back(p);
    }
    in.close();
}

void generateInvoiceForUser(
    vector<Account>& users,
    int currentIdx,
    vector<Booking>& bookings,
    vector<Bicycle>& inventory)
{
    const string indent = string(0, ' ');
    clearScreen();
    cout << indent << "===== GENERATE INVOICE =====\n";

    const Account& user = users[currentIdx];

    // Collect all bookings for this user (with status Active or Completed)
    vector<const Booking*> userBookings;
    for (const auto& b : bookings) {
        if (b.customerID == user.accountID &&
            (b.status == "Active" || b.status == "Completed")) {
            userBookings.push_back(&b);
        }
    }

    if (userBookings.empty()) {
        cout << indent << "You have no bookings to generate an invoice for.\n";
        waitForEnter(indent);
        return;
    }

    // Display list
    cout << indent << "Select a booking to generate invoice:\n";
    for (size_t i = 0; i < userBookings.size(); ++i) {
        const Booking& b = *userBookings[i];
        cout << indent << "[" << i + 1 << "] Booking " << b.bookingID
            << " | Bike: " << b.bikeID
            << " | Date: " << b.bookingDate
            << " | Duration: " << b.duration << "h"
            << " | Status: " << b.status << "\n";
    }
    cout << indent << "[0] Cancel\n";

    int choice = getValidOption(0, static_cast<int>(userBookings.size()), indent);
    if (choice == 0) {
        cout << indent << "Operation cancelled.\n";
        waitForEnter(indent);
        return;
    }

    const Booking& selected = *userBookings[choice - 1];

    // Retrieve rental rate per hour from inventory
    double ratePerHour = 0.0;
    for (const auto& bike : inventory) {
        if (bike.bikeID == selected.bikeID) {
            ratePerHour = bike.rentalRatePerHour;
            break;
        }
    }

    if (ratePerHour == 0.0) {
        cout << indent << "[X] Could not find rate for bicycle " << selected.bikeID
            << ". Invoice cannot be generated.\n";
        waitForEnter(indent);
        return;
    }

    // Build invoice data
    string invoiceNo = selected.bookingID;   // use booking ID as invoice number

    // Generate base filename: invoice_<bookingID>
    string baseFilename = "invoice_" + selected.bookingID;

    // Call the printInvoice function
    printInvoice(
        invoiceNo,
        user.name,
        selected.bikeID,
        selected.duration,
        ratePerHour,
        baseFilename
    );

    cout << "\n" << indent << "Invoice generated successfully!\n";
    cout << indent << "Files created: " << baseFilename << ".svg  and  " << baseFilename << ".html\n";
    waitForEnter(indent);
}

void memberTypeAnalytics(const vector<Account>& users) {
    int memberCount = 0, nonMemberCount = 0;
    for (const auto& u : users) {
        if (u.membership == "Member") ++memberCount;
        else ++nonMemberCount;
    }

    int total = users.size();
    clearScreen();
    cout << "============================================================\n";
    cout << "                 MEMBER TYPE ANALYTICS\n";
    cout << "============================================================\n\n";
    cout << left << setw(18) << "Member Type" << setw(15) << "Users"
        << setw(15) << "Percentage (%)" << "Level\n";
    cout << string(70, '-') << '\n';

    auto printRow = [&](const string& type, int count) {
        double pct = total == 0 ? 0 : (count * 100.0 / total);
        cout << left << setw(18) << type << setw(15) << count
            << setw(14) << fixed << setprecision(2) << pct << "%";
        if (type == "Member") cout << "Premium membership\n";
        else cout << "Basic membership\n";
        };

    printRow("Member", memberCount);
    printRow("Non-Member", nonMemberCount);
    cout << string(70, '-') << '\n';
    cout << left << setw(18) << "Total Members" << setw(15) << total << "100.00%\n";

    cout << "\n---------------- MEMBER SUMMARY ----------------\n";
    if (total > 0) {
        string largest = (memberCount >= nonMemberCount) ? "Member" : "Non-Member";
        int largestCount = max(memberCount, nonMemberCount);
        cout << "Most common member type : " << largest
            << " (" << largestCount << " users)\n";
    }
    else {
        cout << "Most common member type : None (0 users)\n";
    }
    cout << "Member users    : " << memberCount << " users\n";
    cout << "Non-Member users: " << nonMemberCount << " users\n";
    waitForEnter();
}

void bicycleAvailabilityAnalytics(const vector<Bicycle>& inventory) {
    const string types[] = { "city", "mountain", "road", "electric" };
    clearScreen();
    cout << "====================================================================\n";
    cout << "                 BICYCLE AVAILABILITY ANALYTICS\n";
    cout << "====================================================================\n\n";
    cout << left << setw(16) << "Bicycle Type" << setw(12) << "Total"
        << setw(14) << "Available" << setw(14) << "Rented" << "Availability %\n";
    cout << string(70, '-') << '\n';

    int grandTotal = 0, grandAvailable = 0;
    for (const string& type : types) {
        int total = 0, available = 0;
        for (const auto& b : inventory) {
            if (b.type == type) {
                ++total;
                if (b.status == "Available") ++available;
            }
        }
        int rented = total - available;
        double pct = total == 0 ? 0 : (available * 100.0 / total);
        grandTotal += total;
        grandAvailable += available;
        cout << left << setw(16) << type << setw(12) << total
            << setw(14) << available << setw(14) << rented
            << fixed << setprecision(1) << pct << "%\n";
    }
    cout << string(70, '-') << '\n';
    int grandRented = grandTotal - grandAvailable;
    double grandPct = grandTotal == 0 ? 0 : (grandAvailable * 100.0 / grandTotal);
    cout << left << setw(16) << "TOTAL" << setw(12) << grandTotal
        << setw(14) << grandAvailable << setw(14) << grandRented
        << fixed << setprecision(1) << grandPct << "%\n";

    cout << "\n---------------- AVAILABILITY OVERVIEW ----------------\n";
    cout << "Available bicycles : " << grandAvailable << "\n";
    cout << "Currently rented   : " << grandRented << "\n";
    cout << "Availability       : " << fixed << setprecision(1) << grandPct << "%\n";

    cout << "\n---------------- QUICK VIEW ----------------\n";
    cout << "Type            Available / Total\n";
    cout << "---------------------------------\n";
    for (const string& type : types) {
        int total = 0, available = 0;
        for (const auto& b : inventory) {
            if (b.type == type) {
                ++total;
                if (b.status == "Available") ++available;
            }
        }
        cout << left << setw(16) << type << available << " / " << total << '\n';
    }
    waitForEnter();
}

string getMonth(const string& date) {
    if (date.size() >= 7) return date.substr(0, 7);
    return "Unknown";
}

string getYear(const string& date) {
    if (date.size() >= 4) return date.substr(0, 4);
    return "Unknown";
}

string monthName(const string& month) {
    static const string names[] = { "Jan","Feb","Mar","Apr","May","Jun",
                                   "Jul","Aug","Sep","Oct","Nov","Dec" };
    if (month.size() != 7) return month;
    int m = stoi(month.substr(5, 2));
    if (m >= 1 && m <= 12) return names[m - 1];
    return month;
}

string money(double value) {
    ostringstream out;
    out << "RM " << fixed << setprecision(2) << value;
    return out.str();
}

void revenueAnalytics(const vector<Payment>& transactions) {
    double total = 0;
    double highest = -1, lowest = numeric_limits<double>::max();
    int highestID = -1;
    string highestCustomer;

    map<string, double> monthly;
    map<string, pair<int, double>> annual;
    map<string, pair<int, double>> methods;

    for (const auto& p : transactions) {
        total += p.paymentAmount;
        monthly[getMonth(p.paymentDate)] += p.paymentAmount;
        string year = getYear(p.paymentDate);
        annual[year].first++;
        annual[year].second += p.paymentAmount;
        methods[p.paymentMethod].first++;
        methods[p.paymentMethod].second += p.paymentAmount;
        if (p.paymentAmount > highest) {
            highest = p.paymentAmount;
            highestID = stoi(p.paymentID.substr(3)); // extract numeric part
            highestCustomer = p.customerName;
        }
        if (p.paymentAmount < lowest) lowest = p.paymentAmount;
    }

    double average = transactions.empty() ? 0 : total / transactions.size();

    clearScreen();
    cout << "====================================================================\n";
    cout << "                    REVENUE ANALYTICS\n";
    cout << "====================================================================\n\n";

    cout << "+----------------------+----------------------+\n";
    cout << "| Total Revenue        | " << left << setw(20) << money(total) << "|\n";
    cout << "| Total Transactions   | " << left << setw(20) << transactions.size() << "|\n";
    cout << "| Average Transaction  | " << left << setw(20) << money(average) << "|\n";
    cout << "| Highest Transaction  | " << left << setw(20) << money(highest < 0 ? 0 : highest) << "|\n";
    cout << "+----------------------+----------------------+\n";

    // Monthly revenue
    cout << "\n---------------- MONTHLY REVENUE ----------------\n";
    cout << left << setw(15) << "Month" << setw(18) << "Transactions" << "Revenue\n";
    cout << string(50, '-') << '\n';
    // Generate months from available data
    vector<string> months;
    for (const auto& m : monthly) months.push_back(m.first);
    sort(months.begin(), months.end());
    if (months.empty()) {
        cout << "No monthly revenue data.\n";
    }
    else {
        for (const string& m : months) {
            int cnt = 0;
            for (const auto& p : transactions) {
                if (getMonth(p.paymentDate) == m) ++cnt;
            }
            cout << left << setw(15) << monthName(m) << setw(18) << cnt << money(monthly[m]) << '\n';
        }
    }

    // Annual revenue
    cout << "\n---------------- ANNUAL REVENUE ----------------\n";
    cout << left << setw(15) << "Year" << setw(18) << "Transactions" << "Revenue\n";
    cout << string(50, '-') << '\n';
    if (annual.empty()) {
        cout << "No annual revenue data available.\n";
    }
    else {
        for (const auto& item : annual) {
            cout << left << setw(15) << item.first
                << setw(18) << item.second.first
                << money(item.second.second) << '\n';
        }
    }

    // Payment method analysis
    cout << "\n---------------- PAYMENT METHOD ANALYSIS ----------------\n";
    cout << left << setw(20) << "Payment Method" << setw(15) << "Transactions"
        << setw(18) << "Revenue" << "Percentage\n";
    cout << string(75, '-') << '\n';
    const string payMethods[] = { "Debit Card", "Credit Card", "Touch'n Go" };
    for (const string& m : payMethods) {
        int cnt = methods[m].first;
        double rev = methods[m].second;
        double share = total == 0 ? 0 : (rev * 100.0 / total);
        cout << left << setw(20) << m << setw(15) << cnt
            << setw(18) << money(rev) << fixed << setprecision(2) << share << "%\n";
    }

    // Revenue insights
    cout << "\n---------------- REVENUE INSIGHTS ----------------\n";
    if (!transactions.empty()) {
        string bestMonth = "";
        double bestMonthRev = -1;
        for (const auto& item : monthly) {
            if (item.second > bestMonthRev) {
                bestMonthRev = item.second;
                bestMonth = item.first;
            }
        }
        string bestMethod = "";
        double bestMethodRev = -1;
        for (const auto& item : methods) {
            if (item.second.second > bestMethodRev) {
                bestMethodRev = item.second.second;
                bestMethod = item.first;
            }
        }
        string bestYear = "";
        double bestYearRev = -1;
        for (const auto& item : annual) {
            if (item.second.second > bestYearRev) {
                bestYearRev = item.second.second;
                bestYear = item.first;
            }
        }
        cout << "Best revenue month    : " << monthName(bestMonth)
            << " (" << money(bestMonthRev) << ")\n";
        cout << "Best revenue year     : " << bestYear
            << " (" << money(bestYearRev) << ")\n";
        cout << "Top payment method    : " << bestMethod
            << " (" << money(bestMethodRev) << ")\n";
        cout << "Highest payment       : " << money(highest)
            << " by " << highestCustomer
            << " (Payment #" << highestID << ")\n";
        cout << "Average payment       : " << money(average) << "\n";
        cout << "Lowest payment        : " << money(lowest) << "\n";
    }
    else {
        cout << "No payment records are available for analysis.\n";
    }
    waitForEnter();
}

void bicycleAnalysis(const vector<Bicycle>& inventory) {
    const string types[] = { "city", "mountain", "road", "electric" };
    map<string, int> counts;
    for (const string& t : types) counts[t] = 0;
    for (const auto& b : inventory) {
        if (counts.find(b.type) != counts.end()) ++counts[b.type];
    }

    int maxCount = 0;
    for (const auto& t : types) maxCount = max(maxCount, counts[t]);

    clearScreen();
    cout << "====================================================================\n";
    cout << "                     BICYCLE ANALYSIS\n";
    cout << "====================================================================\n\n";
    cout << left << setw(18) << "Bicycle Type" << setw(12) << "Quantity" << "Percentage\n";
    cout << string(55, '-') << '\n';
    for (const string& t : types) {
        double pct = inventory.empty() ? 0 : (counts[t] * 100.0 / inventory.size());
        cout << left << setw(18) << t << setw(12) << counts[t]
            << fixed << setprecision(1) << pct << "%\n";
    }
    cout << string(55, '-') << '\n';
    cout << left << setw(18) << "Total" << setw(12) << inventory.size() << "100.0%\n";

    // Bar chart
    cout << "\n---------------- BICYCLE TYPE BAR CHART ----------------\n";
    cout << "Each # represents approximately 1 bicycle.\n\n";
    for (const string& t : types) {
        cout << left << setw(11) << t << " | ";
        for (int i = 0; i < counts[t]; ++i) cout << '#';
        cout << "  " << counts[t] << '\n';
    }

    // Analysis summary
    cout << "\n---------------- ANALYSIS SUMMARY ----------------\n";
    if (!inventory.empty()) {
        string mostCommon = types[0], leastCommon = types[0];
        for (const string& t : types) {
            if (counts[t] > counts[mostCommon]) mostCommon = t;
            if (counts[t] < counts[leastCommon]) leastCommon = t;
        }
        cout << "Largest bicycle category : " << mostCommon
            << " (" << counts[mostCommon] << " bicycles)\n";
        cout << "Smallest bicycle category: " << leastCommon
            << " (" << counts[leastCommon] << " bicycles)\n";
    }
    else {
        cout << "Largest bicycle category : None (0 bicycles)\n";
        cout << "Smallest bicycle category: None (0 bicycles)\n";
    }
    cout << "Total bicycles           : " << inventory.size() << '\n';
    waitForEnter();
}

void reportingMenu(vector<Account>& users, vector<Bicycle>& inventory, vector<Payment>& transactions) {
    while (true) {
        clearScreen();
        cout << "====================================================================\n";
        cout << "                     REPORTING & ANALYTICS\n";
        cout << "====================================================================\n\n";
        cout << "[1] Member Type Analytics\n";
        cout << "[2] Bicycle Availability Analytics\n";
        cout << "[3] Revenue Analytics\n";
        cout << "[4] Bicycle Analysis & Bar Chart\n";
        cout << "[0] Back\n\n";

        int op = getValidOption(0, 4);
        if (op == -1) continue;

        switch (op) {
        case 1: memberTypeAnalytics(users); break;
        case 2: bicycleAvailabilityAnalytics(inventory); break;
        case 3: revenueAnalytics(transactions); break;
        case 4: bicycleAnalysis(inventory); break;
        case 0: 
            if (confirmAction("Are you sure want to go back?"))
            return;
        }
    }
}

// ==================================================================
// MAIN
// ==================================================================
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
    vector<Bicycle> inventory;
    vector<Booking> bookings;

    User_LoadFile(users);
    Repair_LoadFile(repairs);
    SyncInventoryWithBookings(bookings, inventory);
    try {
        LoadInventoryFromFile(inventory);
        LoadBookingsFromFile(bookings, BOOKING_FILE);
        LoadPaymentFromFile();
    }
    catch (const exception& e) {
        cerr << "Error loading data: " << e.what() << endl;

    }

    Menu(admin, users, repairs, inventory, bookings);

    SaveInventoryToFile(inventory);
    SaveBookingsToFile(bookings, BOOKING_FILE);
    SavePaymentToFile();

    return 0;
}