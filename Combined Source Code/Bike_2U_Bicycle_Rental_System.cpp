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

const double RATE_CITY = 5.00;
const double RATE_MOUNTAIN = 7.00;
const double RATE_ROAD = 4.00;
const double RATE_ELECTRIC = 8.00;
const int MAX_RENTAL_HOURS = 24;

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
    // Membership fields (tiered)
    string membershipType;   // "Bronze", "Silver", "Gold", "None"
    double annualFee;        // 50, 100, 150
    int discount;            // 2, 4, 6 (percentage)
    string startDate;        // YYYY-MM-DD
    string expiryDate;       // YYYY-MM-DD
    string membershipStatus; // "Active" or "Expired"
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
    string membershipType;   // tier at time of booking
    int discount;            // discount at time of booking
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
    string refundID;
    string refundStatus;
    string requestDate;
    string rejectReason;
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
void changeMembership(vector<Account>& users, int currentIdx);

int getValidOption(int minVal, int maxVal, const string& indent = "");
bool confirmAction(const string& message);
string generateIDFromList(const vector<string>& existingIDs, const string& prefix);
string generateAccountID(const vector<Account>& users);
string generateRepairID(const vector<RepairReport>& repairs);
string generateBicycleID(const vector<Bicycle>& inventory);
string generateBookingID(const vector<Booking>& bookings);
string generatePaymentID(const vector<Payment>& payments);
string generateRefundID(const vector<Payment>& payments);

bool validateEmail(const string& email);
bool validatePhone(const string& phone);
bool validatePassword(const string& password);
string getPasswordInput(const string& prompt);
bool emailExist(const vector<Account>& users, const string& email);
int registerUser(vector<Account>& users);
bool matchAdmin(const Account& admin, const string& email, const string& password);
int matchUser(const vector<Account>& users, const string& email, const string& password);

void repairService(const Account& user, vector<RepairReport>& repairs, const vector<Booking>& bookings, const vector<Bicycle>& inventory);
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
string getStationFromUser(bool allowCancel = false, const string& purpose = "pickup");

// Rental booking functions
void CreateBooking(vector<Booking>& bookings, vector<Bicycle>& inventory,
    vector<Account>& users, int currentIdx,
    const string& customerID, const string& customerName,
    const string& membershipType, int discount);
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
string ProcessPaymentForBooking(const Booking& booking, double rentalHours, double baseRate, const string& customerName, const string& bikeType);
bool isValidExpiry(const string& expiry);
void DisplayUserPaymentHistory(const string& customerName, const vector<Booking>& bookings, const vector<Bicycle>& inventory);
void adminRefundManagement(vector<Payment>& transactions);
void RequestRefund(vector<Account>& users, int currentIdx, vector<Booking>& bookings, vector<Payment>& transactions, const vector<Bicycle>& inventory);
void DisplayAllPayments(const vector<Booking>& bookings);
void GenerateReceipt(const string& paymentID, const string& bookingID, const string& bikeID, const string& customerName, double amount, const string& method, const string& date, const string& bookingDate, const string& bookingTime, const string& returnTime, const string& pickupStation);
bool isValidCardNumber(const string& cardNum);
string readCardNumberWithFormat();
bool isValidCVV(const string& cvv);
void generateInvoiceForUser(
    vector<Account>& users,
    int currentIdx,
    vector<Booking>& bookings,
    vector<Bicycle>& inventory
);

// Membership Management
void membershipManagement(vector<Account>& users, int currentIdx);
void viewMembershipDetails(const Account& user);
void registerMembership(vector<Account>& users, int currentIdx);
void renewMembership(vector<Account>& users, int currentIdx);
bool processMembershipPayment(double amount, string& method);

// Analytics (from third file)
void memberTypeAnalytics(const vector<Account>& users);
void bicycleAvailabilityAnalytics(const vector<Bicycle>& inventory);
void revenueAnalytics(const vector<Payment>& transactions);
void bicycleAnalysis(const vector<Bicycle>& inventory);
void reportingMenu(vector<Account>& users, vector<Bicycle>& inventory, vector<Payment>& transactions);

// Date utilities for membership
string getCurrentDate();
string formatDate(int day, int month, int year);
bool isLeapYear(int year);
int daysInMonth(int month, int year);
string calculateExpiryDate(const string& startDate, int years);
string calculateExpiryFromExistingDate(const string& expiryDate, int years);
string getMembershipStatus(const string& expiryDate);

// Analytics helper functions (must be declared before use)
string money(double value);
string getMonth(const string& date);
string monthName(const string& month);
string getYear(const string& date);

double hoursDifference(const string& date1, const string& time1,
    const string& date2, const string& time2);
double roundToTwo(double value) {
    return round(value * 100) / 100;
}

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

string generateRefundID(const vector<Payment>& payments) {
    vector<string> ids;
    for (const auto& p : payments) {
        if (p.refundID.rfind("RRP", 0) == 0)
            ids.push_back(p.refundID);
    }
    return generateIDFromList(ids, "RRP");
}

// ------------------------------------------------------------------
// Analytics Helper Functions (must be defined BEFORE they are used)
// ------------------------------------------------------------------
string money(double value) {
    ostringstream out;
    out << "RM " << fixed << setprecision(2) << value;
    return out.str();
}

string getMonth(const string& date) {
    if (date.size() >= 7) return date.substr(0, 7);
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

string getYear(const string& date) {
    if (date.size() >= 4) return date.substr(0, 4);
    return "Unknown";
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
        cout << indent << "          Welcome to Bike 2U Bicycle Rental System!\n";
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
        cout << indent << "==============================\n";
        cout << indent << "          ADMIN MENU          \n";
        cout << indent << "==============================\n";
        cout << indent << "[1] View Customer Details\n";
        cout << indent << "[2] View Reported Issues\n";
        cout << indent << "[3] View Bicycle Inventory\n";
        cout << indent << "[4] View All Transactions\n";
        cout << indent << "[5] View Refunds Management\n";
        cout << indent << "[6] View Reports and Analytics\n";
        cout << indent << "[0] Log out\n";

        int option = getValidOption(0, 6, indent);
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
            adminRefundManagement(transactions);
            break;
        case 6:
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
        cout << indent << "==============================\n";
        cout << indent << "          REPAIR MENU         \n";
        cout << indent << "==============================\n";
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
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            break;
        }
    }
}

// ------------------------------------------------------------------
// User Menu (NEW LAYOUT)
// ------------------------------------------------------------------
void userMenu(vector<Account>& users, int currentIdx, vector<RepairReport>& repairs,
    vector<Bicycle>& inventory, vector<Booking>& bookings) {
    const string indent = string(0, ' ');
    while (true) {
        clearScreen();
        cout << indent << "==============================\n";
        cout << indent << "          USER MENU\n";
        cout << indent << "==============================\n";
        cout << indent << "[1] Rent a Bicycle\n";
        cout << indent << "[2] View Booking Details\n";
        cout << indent << "[3] Return Bicycle\n";
        cout << indent << "[4] View Payment History\n";
        cout << indent << "[5] Request Refund\n";
        cout << indent << "[6] Report an Issue\n";
        cout << indent << "[7] Membership\n";
        cout << indent << "[8] View Profile\n";
        cout << indent << "[0] Log Out\n";
        cout << indent << "==============================\n";

        int option = getValidOption(0, 8, indent);
        if (option == -1) continue;

        switch (option) {
        case 1:
            CreateBooking(bookings, inventory,
                users, currentIdx,
                users[currentIdx].accountID,
                users[currentIdx].name,
                users[currentIdx].membershipType,
                users[currentIdx].discount);
            break;
        case 2:
            ViewMyBookings(bookings, users[currentIdx].accountID);
            break;
        case 3:
            ReturnBicycle(bookings, inventory, users[currentIdx].accountID);
            break;
        case 4:
            DisplayUserPaymentHistory(users[currentIdx].name, bookings, inventory);
            break;
        case 5:
            RequestRefund(users, currentIdx, bookings, transactions, inventory);
            break;
        case 6:
            repairService(users[currentIdx], repairs, bookings, inventory);
            break;
        case 7:
            membershipManagement(users, currentIdx);
            break;
        case 8:
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
// Membership Management
// ------------------------------------------------------------------
void membershipManagement(vector<Account>& users, int currentIdx) {
    int choice;
    do {
        clearScreen();
        cout << "====================================\n";
        cout << "        MEMBERSHIP MANAGEMENT\n";
        cout << "====================================\n";
        cout << "[1] View Membership Details\n";
        cout << "[2] Register Membership\n";
        cout << "[3] Renew Membership\n";
        cout << "[4] Change Membership Type\n";
        cout << "[0] Back\n";
        cout << "====================================\n";

        choice = getValidOption(0, 4, "");
        if (choice == -1) {
            continue;
        }

        switch (choice) {
        case 1:
            viewMembershipDetails(users[currentIdx]);
            break;
        case 2:
            registerMembership(users, currentIdx);
            break;
        case 3:
            renewMembership(users, currentIdx);
            break;
        case 4:
            changeMembership(users, currentIdx);
            break;
        case 0:
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
            }
            else {
                choice = -1;
            }
            break;
        }
    } while (choice != 0);
}

void changeMembership(vector<Account>& users, int currentIdx) {
    Account& user = users[currentIdx];

    if (user.membershipType == "None" || user.membershipType.empty()) {
        clearScreen();
        cout << "You don't have a membership. Please register first.\n";
        waitForEnter();
        return;
    }
    if (user.membershipStatus != "Active") {
        clearScreen();
        cout << "Your membership has expired. Please renew before changing type.\n";
        waitForEnter();
        return;
    }

    while (true) {
        clearScreen();
        cout << "========================================\n";
        cout << "       CHANGE MEMBERSHIP TYPE\n";
        cout << "========================================\n";
        cout << "Current Type : " << user.membershipType << "\n";
        cout << "Current Fee  : RM " << fixed << setprecision(2) << user.annualFee << "\n";
        cout << "Discount     : " << user.discount << "%\n\n";

        cout << "Select new membership type:\n";
        cout << "  [1] Bronze  (2% discount, RM50/year)\n";
        cout << "  [2] Silver  (4% discount, RM100/year)\n";
        cout << "  [3] Gold    (6% discount, RM150/year)\n";
        cout << "  [0] Back\n";

        int choice = getValidOption(0, 3, "");

        // ---- HANDLE INVALID INPUT ----
        if (choice == -1) {
            // getValidOption already printed error and waited
            continue;   // re‑show the menu
        }

        // ---- HANDLE CANCEL ----
        if (choice == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            continue;   // user said "No" – re‑show menu
        }

        // ---- VALID CHOICE (1-3) ----
        string newType;
        int newDiscount;
        double newFee = 0.0;

        switch (choice) {
        case 1: newType = "Bronze"; newDiscount = 2; newFee = 50.00; break;
        case 2: newType = "Silver"; newDiscount = 4; newFee = 100.00; break;
        case 3: newType = "Gold";   newDiscount = 6; newFee = 150.00; break;
        }

        // Check if same as current
        if (newType == user.membershipType) {
            cout << "You are already a " << newType << " member. No change needed.\n";
            waitForEnter();
            return;   // or continue, but return is fine
        }

        // Handle upgrade/downgrade payment
        double feeDifference = newFee - user.annualFee;
        if (feeDifference > 0) {
            cout << "\nUpgrading to " << newType << " costs an additional RM "
                << fixed << setprecision(2) << feeDifference << ".\n";
            if (!confirmAction("Proceed with payment?")) {
                cout << "Change cancelled.\n";
                waitForEnter();
                continue;
            }

            string method;
            if (!processMembershipPayment(feeDifference, method)) {
                cout << "Payment failed or cancelled. Change not applied.\n";
                waitForEnter();
                continue;
            }
            cout << "Payment successful.\n";
        }
        else if (feeDifference < 0) {
            cout << "\nDowngrading to " << newType << " (no refund for the difference).\n";
            if (!confirmAction("Confirm downgrade?")) {
                cout << "Change cancelled.\n";
                waitForEnter();
                continue;
            }
        }

        // Apply changes
        user.membershipType = newType;
        user.discount = newDiscount;
        user.annualFee = newFee;

        User_SaveFile(users);
        clearScreen();
        cout << "\n========================================\n";
        cout << "     MEMBERSHIP TYPE CHANGED!\n";
        cout << "========================================\n";
        cout << "You are now a " << newType << " member.\n";
        cout << "New discount: " << newDiscount << "%\n";
        cout << "Expiry date remains: " << user.expiryDate << "\n";
        cout << "========================================\n";
        waitForEnter();
        return;
    }
}

// ---------- Date Utilities for Membership ----------
string getCurrentDate() {
    time_t now = time(nullptr);
    tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localTime = *localtime(&now);
#endif
    int day = localTime.tm_mday;
    int month = localTime.tm_mon + 1;
    int year = localTime.tm_year + 1900;
    return formatDate(day, month, year);
}

string formatDate(int day, int month, int year) {
    ostringstream oss;
    oss << year << "-"
        << setw(2) << setfill('0') << month << "-"
        << setw(2) << setfill('0') << day;
    return oss.str();
}

bool isLeapYear(int year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return year % 4 == 0;
}

int daysInMonth(int month, int year) {
    switch (month) {
    case 1: return 31;
    case 2: return isLeapYear(year) ? 29 : 28;
    case 3: return 31;
    case 4: return 30;
    case 5: return 31;
    case 6: return 30;
    case 7: return 31;
    case 8: return 31;
    case 9: return 30;
    case 10: return 31;
    case 11: return 30;
    case 12: return 31;
    default: return 30;
    }
}

string calculateExpiryDate(const string& startDate, int years) {
    int day = stoi(startDate.substr(8, 2));
    int month = stoi(startDate.substr(5, 2));
    int year = stoi(startDate.substr(0, 4));
    int newYear = year + years;
    if (month == 2 && day == 29 && !isLeapYear(newYear)) {
        day = 28;
    }
    return formatDate(day, month, newYear);
}

string calculateExpiryFromExistingDate(const string& expiryDate, int years) {
    return calculateExpiryDate(expiryDate, years);
}

string getMembershipStatus(const string& expiryDate) {
    string current = getCurrentDate();
    return (current <= expiryDate) ? "Active" : "Expired";
}

// ---------- View Membership Details ----------
void viewMembershipDetails(const Account& user) {
    clearScreen();
    cout << "========================================\n";
    cout << "        MEMBERSHIP DETAILS\n";
    cout << "========================================\n";

    if (user.membershipType == "None" || user.membershipType.empty()) {
        cout << "No membership registered.\n";
        cout << "Please register for a membership.\n";
        cout << "========================================\n";
        waitForEnter();
        return;
    }

    cout << "Membership Type : " << user.membershipType << "\n";
    cout << "Discount        : " << user.discount << "%\n";
    cout << "Annual Fee      : RM " << fixed << setprecision(2) << user.annualFee << "\n";
    cout << "Start Date      : " << user.startDate << "\n";
    cout << "Expiry Date     : " << user.expiryDate << "\n";
    cout << "Status          : " << user.membershipStatus << "\n";
    cout << "========================================\n";
    waitForEnter();
}

// ---------- Register Membership ----------
void registerMembership(vector<Account>& users, int currentIdx) {
    clearScreen();
    Account& user = users[currentIdx];

    if (user.membershipType != "None" && user.membershipStatus == "Active") {
        cout << "You already have an active " << user.membershipType << " membership.\n";
        cout << "Please use the Renew option to extend it.\n";
        waitForEnter();
        return;
    }

    cout << "========================================\n";
    cout << "        REGISTER MEMBERSHIP\n";
    cout << "========================================\n";
    cout << "\nCustomer Information:\n";
    cout << "  Name : " << user.name << "\n";
    cout << "  ID   : " << user.accountID << "\n";
    cout << "\nMembership Types:\n";
    cout << "  [1] Bronze  (2% discount, RM50/year)\n";
    cout << "  [2] Silver  (4% discount, RM100/year)\n";
    cout << "  [3] Gold    (6% discount, RM150/year)\n";
    cout << "  [0] Back\n";

    int choice = getValidOption(0, 3, "");
    if (choice == 0) {
        cout << "Registration cancelled.\n";
        waitForEnter();
        return;
    }

    string type;
    int disc;
    double fee;
    switch (choice) {
    case 1: type = "Bronze"; disc = 2; fee = 50.00; break;
    case 2: type = "Silver"; disc = 4; fee = 100.00; break;
    case 3: type = "Gold";   disc = 6; fee = 150.00; break;
    }

    string startDate = getCurrentDate();
    string expiryDate = calculateExpiryDate(startDate, 1);

    clearScreen();
    cout << "========================================\n";
    cout << "        MEMBERSHIP SUMMARY\n";
    cout << "========================================\n";
    cout << "Type         : " << type << "\n";
    cout << "Discount     : " << disc << "%\n";
    cout << "Annual Fee   : RM " << fixed << setprecision(2) << fee << "\n";
    cout << "Start Date   : " << startDate << "\n";
    cout << "Expiry Date  : " << expiryDate << "\n";
    cout << "Total        : RM " << fixed << setprecision(2) << fee << "\n";
    cout << "========================================\n";

    if (!confirmAction("Proceed to payment?")) {
        cout << "Registration cancelled.\n";
        waitForEnter();
        return;
    }

    string method;
    if (!processMembershipPayment(fee, method)) {
        cout << "Payment failed or cancelled. Registration not completed.\n";
        waitForEnter();
        return;
    }

    user.membershipType = type;
    user.discount = disc;
    user.annualFee = fee;
    user.startDate = startDate;
    user.expiryDate = expiryDate;
    user.membershipStatus = "Active";

    User_SaveFile(users);

    cout << "\n========================================\n";
    cout << "   MEMBERSHIP REGISTRATION SUCCESSFUL!\n";
    cout << "========================================\n";
    cout << "You are now a " << type << " member.\n";
    cout << "Enjoy your " << disc << "% discount on rentals!\n";
    cout << "========================================\n";
    waitForEnter();
}

// ---------- Renew Membership ----------
void renewMembership(vector<Account>& users, int currentIdx) {
    clearScreen();
    Account& user = users[currentIdx];

    if (user.membershipType == "None" || user.membershipType.empty()) {
        cout << "You don't have a membership. Please register first.\n";
        waitForEnter();
        return;
    }

    cout << "========================================\n";
    cout << "        RENEW MEMBERSHIP\n";
    cout << "========================================\n";
    cout << "Current Membership: " << user.membershipType << "\n";
    cout << "Current Expiry    : " << user.expiryDate << "\n";
    cout << "Status            : " << user.membershipStatus << "\n";
    cout << "Renewal Fee       : RM " << fixed << setprecision(2) << user.annualFee << "\n";
    cout << "========================================\n";

    if (!confirmAction("Renew for another year?")) {
        cout << "Renewal cancelled.\n";
        waitForEnter();
        return;
    }

    string method;
    if (!processMembershipPayment(user.annualFee, method)) {
        cout << "Payment failed or cancelled. Renewal not completed.\n";
        waitForEnter();
        return;
    }

    string newExpiry = calculateExpiryFromExistingDate(user.expiryDate, 1);
    user.expiryDate = newExpiry;
    user.membershipStatus = getMembershipStatus(newExpiry);

    User_SaveFile(users);

    cout << "\n========================================\n";
    cout << "        RENEWAL SUCCESSFUL!\n";
    cout << "========================================\n";
    cout << "New Expiry Date : " << user.expiryDate << "\n";
    cout << "Status          : " << user.membershipStatus << "\n";
    cout << "========================================\n";
    waitForEnter();
}

// ---------- Membership Payment Processing ----------
bool processMembershipPayment(double amount, string& method) {
    clearScreen();
    cout << "========================================\n";
    cout << "          MEMBERSHIP PAYMENT\n";
    cout << "========================================\n";
    cout << "Amount to Pay : RM " << fixed << setprecision(2) << amount << "\n\n";
    cout << "[1] Debit Card\n";
    cout << "[2] Credit Card\n";
    cout << "[3] TnG eWallet\n";
    cout << "[0] Back\n";

    int choice = getValidOption(0, 3, "");
    if (choice == 0) {
        cout << "Payment cancelled.\n";
        waitForEnter();
        return false;
    }

    // ---------- CARD PAYMENT (Debit / Credit) ----------
    if (choice == 1 || choice == 2) {
        string cardNum, expiry, cvv;

        // Card number – with formatting and backspace support
        bool validCard = false;
        do {
            cardNum = readCardNumberWithFormat();  

            if (cardNum == "0") {
                cout << "Payment cancelled by user.\n";
                waitForEnter();
                return false;
            }

            if (cardNum.length() != 16) {
                cout << "[X] Invalid card number. Must be exactly 16 digits.\n";
            }
            else {
                validCard = true;
            }
        } while (!validCard);

        // Expiry
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

        // CVV
        bool cvvOk = false;
        do {
            cout << "CVV (3 digits): ";
            getline(cin, cvv);
            if (!isValidCVV(cvv)) {
                cout << "[X] Invalid CVV. Only 3 digits allowed.\n";
            }
            else {
                cvvOk = true;
            }
        } while (!cvvOk);

        cout << "Card verified.\n";
        waitForEnter();
        clearScreen();

        method = (choice == 1) ? "Debit Card" : "Credit Card";
    }
    // ---------- TOUCH 'N GO ----------
    else if (choice == 3) {
        string phone;
        bool phoneValid = false;
        do {
            cout << "Touch'n Go phone number (e.g., 0123456789): ";
            getline(cin, phone);
            if (phone.length() < 10 || phone.length() > 11) {
                cout << "[X] Invalid phone number. Must be 10 or 11 digits.\n";
                continue;
            }
            bool ok = true;
            for (char c : phone) if (!isdigit(c)) { ok = false; break; }
            if (!ok || phone.substr(0, 2) != "01") {
                cout << "[X] Invalid phone number. Must start with '01' and contain only digits.\n";
                continue;
            }
            phoneValid = true;
        } while (!phoneValid);

        // 6‑digit PIN
        string password;
        bool pwdValid = false;
        do {
            cout << "Touch'n Go pin numbers (6 digits): ";
            getline(cin, password);
            if (password.length() != 6) {
                cout << "[X] Password must be exactly 6 digits.\n";
                continue;
            }
            bool allDigits = true;
            for (char c : password) if (!isdigit(c)) { allDigits = false; break; }
            if (!allDigits) {
                cout << "[X] Password must contain only digits.\n";
                continue;
            }
            pwdValid = true;
        } while (!pwdValid);

        cout << "Touch'n Go validated.\n";
        waitForEnter();
        clearScreen();

        method = "TnG eWallet";
    }

    // ---------- Payment success ----------
    cout << "\n========================================\n";
    cout << "          PAYMENT SUCCESSFUL\n";
    cout << "========================================\n";
    cout << "Amount Paid : RM " << fixed << setprecision(2) << amount << "\n";
    cout << "Method      : " << method << "\n";
    cout << "========================================\n";
    waitForEnter();
    clearScreen();
    return true;
}

// ------------------------------------------------------------------
// Account Management
// ------------------------------------------------------------------
void accountManagementMenu(vector<RepairReport>& repairs, vector<Account>& users) {
    const string indent = string(6, ' ');
    while (true) {
        clearScreen();
        cout << "========================================\n";
        cout << "          ACCOUNT MANAGEMENT\n";
        cout << "========================================\n";
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
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
            }
            return;
            break;
        }
    }
}

// ---------- Utility functions ----------
int getValidOption(int minVal, int maxVal, const string& indent) {
    while (true) {
        string input;
        cout << indent << "Enter your option (" << minVal << "-" << maxVal << "): ";
        getline(cin, input);

        // Remove leading/trailing spaces
        input = trim(input);

        // Check if the input is empty
        if (input.empty()) {
            cout << indent << "[X] Input cannot be empty. Please enter option (" << minVal << " - " << maxVal << ").\n";
            waitForEnter(indent);
            return -1;
        }

        // Check if the input consists only of digits
        bool isNumber = true;
        for (char c : input) {
            if (!isdigit(static_cast<unsigned char>(c))) {
                isNumber = false;
                break;
            }
        }

        if (!isNumber) {
            cout << indent << "[X] Please enter a valid number between " << minVal << " and " << maxVal << ".\n";
            waitForEnter(indent);
            return -1;
        }

        // Convert to integer
        int option = stoi(input);

        // Check range
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

    cout << et << "Email (e.g user@gmail.com)" << string(3, ' ') << ": ";
    getline(cin, newAcc.email);
    if (trim(newAcc.email) == "0")
        return -2;
    if (!validateEmail(newAcc.email)) {
        cout << et << "[X] Invalid email format. (e.g., user@gmail.com)\n";
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
    // Default membership: None
    newAcc.membershipType = "None";
    newAcc.annualFee = 0.0;
    newAcc.discount = 0;
    newAcc.startDate = "";
    newAcc.expiryDate = "";
    newAcc.membershipStatus = "";
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

double hoursDifference(const string& date1, const string& time1,
    const string& date2, const string& time2) {
    struct tm tm1 = {}, tm2 = {};
    istringstream ds1(date1), ts1(time1), ds2(date2), ts2(time2);

    ds1 >> get_time(&tm1, "%Y-%m-%d");
    ts1 >> get_time(&tm1, "%H:%M");
    ds2 >> get_time(&tm2, "%Y-%m-%d");
    ts2 >> get_time(&tm2, "%H:%M");

    // get_time stores full year and month 1‑12
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    double diffSeconds = difftime(t2, t1);
    return diffSeconds / 3600.0;   // hours
}

void repairService(const Account& user, vector<RepairReport>& repairs,
    const vector<Booking>& bookings, const vector<Bicycle>& inventory) {
    // --------------------------------------------------------------
    // 1. Build list of unique bikes the user has rented
    // --------------------------------------------------------------
    struct BikeInfo {
        string bikeID;
        string type;
        string pickupStation;
        string date;
        string time;
    };
    map<string, BikeInfo> bikeMap;  // key: bikeID, value: info

    for (const auto& b : bookings) {
        if (b.customerID == user.accountID) {
            if (bikeMap.find(b.bikeID) == bikeMap.end()) {
                string bikeType = "Unknown";
                for (const auto& bike : inventory) {
                    if (bike.bikeID == b.bikeID) {
                        bikeType = bike.type;
                        break;
                    }
                }
                BikeInfo info;
                info.bikeID = b.bikeID;
                info.type = bikeType;
                info.pickupStation = b.pickupStation;
                info.date = b.bookingDate;
                info.time = b.bookingTime;   // start time
                bikeMap[b.bikeID] = info;
            }
        }
    }

    vector<BikeInfo> bikeList;
    for (auto& pair : bikeMap) {
        bikeList.push_back(pair.second);
    }

    if (bikeList.empty()) {
        clearScreen();
        cout << "===== REPAIR SERVICE =====\n";
        cout << "[X] You have no rental records. You can only report damage for a bicycle you have rented.\n";
        waitForEnter();
        return;
    }

    // --------------------------------------------------------------
    // 2. Let the user select a bicycle (list format)
    // --------------------------------------------------------------
    string selectedBikeID;
    string selectedBikeType;
    string repairStation;
    while (true) {
        clearScreen();
        cout << "==============================\n";
        cout << "       REPAIR SERVICE\n";
        cout << "==============================\n";
        cout << "\nSelect the bicycle you want to report damage for:\n\n";

        for (size_t i = 0; i < bikeList.size(); ++i) {
            cout << "[" << (i + 1) << "] Bike ID: " << bikeList[i].bikeID
                << " | Type: " << bikeList[i].type
                << " | Date: " << bikeList[i].date
                << " | Time: " << bikeList[i].time << "\n";
        }
        cout << "[0] Cancel\n\n";
        cout << "Enter your option (0-" << bikeList.size() << ") : ";

        string input;
        getline(cin, input);
        if (input.empty() || !all_of(input.begin(), input.end(), ::isdigit)) {
            cout << "[X] Invalid option. Please enter a number (0-" << bikeList.size() << ").\n";
            waitForEnter();
            continue;
        }

        int choice = stoi(input);
        if (choice == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            continue;
        }

        if (choice < 1 || choice > static_cast<int>(bikeList.size())) {
            cout << "[X] Invalid choice. Please select a number between 1 and "
                << bikeList.size() << ".\n";
            waitForEnter();
            continue;
        }

        selectedBikeID = bikeList[choice - 1].bikeID;
        selectedBikeType = bikeList[choice - 1].type;
        repairStation = bikeList[choice - 1].pickupStation;   // auto-set from booking
        break;
    }

    // --------------------------------------------------------------
    // 3. Damage type selection (unchanged)
    // --------------------------------------------------------------
    string damageType;
    bool validInput = false;
    while (!validInput) {
        clearScreen();
        cout << "==============================\n";
        cout << "       REPAIR SERVICE\n";
        cout << "==============================\n";
        cout << "\n----- User Information ------\n";
        cout << "ID           : " << user.accountID << "\n";
        cout << "Name         : " << user.name << "\n";
        cout << "Bicycle ID   : " << selectedBikeID << "\n";
        cout << "Bicycle Type : " << selectedBikeType << "\n";
        cout << "Repair Station : " << repairStation << "\n";
        cout << "-----------------------------\n";
        cout << "Note: Please send the bicycle to the repair station.\n";

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
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            continue;
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
            cout << "[X] Invalid option. Please enter numbers between 1 and " << NUM_OPTIONS << " (e.g., 1,2,3).\n";
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

    // --------------------------------------------------------------
    // 4. Submit confirmation with 3 attempts (unchanged)
    // --------------------------------------------------------------
    int attempts = 0;
    bool submitted = false;
    while (attempts < 3) {
        clearScreen();
        cout << "==================================\n";
        cout << "       REPAIR REPORT SUMMARY\n";
        cout << "==================================\n";
        cout << "ID          : " << user.accountID << "\n";
        cout << "Name        : " << user.name << "\n";
        cout << "Bicycle ID  : " << selectedBikeID << "\n";
        cout << "Bicycle Type: " << selectedBikeType << "\n";
        cout << "Damage Type : " << damageType << "\n";
        cout << "Station     : " << repairStation << "\n";

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
                repair.station = repairStation;   // auto-set from booking

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
        if (confirmAction("Are you sure want to return back?")) {
            cout << "Returning...\n";
            waitForEnter();
            return false;
        }
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
        int option = getValidOption(0, 4, "");
        if (option == -1)
            continue;
        if (option == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
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
        cout << left << setw(12) << "Report ID"
            << setw(14) << "Customer ID"
            << setw(12) << "Bicycle ID"
            << setw(12) << "Date"
            << "Status" << "\n";
        cout << string(65, '-') << "\n";
        for (const auto& repair : repairs) {
            cout << left << setw(12) << repair.repairID
                << setw(14) << repair.userID
                << setw(12) << repair.bicycleID
                << setw(12) << repair.date
                << repair.status << "\n";
        }
        cout << string(65, '-') << "\n\n";

        cout << "[Enter 0 to return back]" << endl;
        cout << "Enter Report ID to update status: ";
        string repairID;
        getline(cin, repairID);
        repairID = trim(repairID);

        if (repairID == "0") {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...";
                waitForEnter();
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
        cout << "Customer ID     : " << repair.userID << "\n";
        cout << "Bicycle ID      : " << repair.bicycleID << "\n";
        cout << "Date            : " << repair.date << "\n";
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
        cout << "==================================\n";
        cout << "           USER PROFILE           \n";
        cout << "==================================\n";
        cout << "ID          : " << user.accountID << "\n";
        cout << "Name        : " << user.name << "\n";
        cout << "Email       : " << user.email << "\n";
        cout << "Phone       : " << user.phone << "\n";
        cout << "Password    : " << user.password << "\n";
        cout << "Membership  : " << (user.membershipType == "None" ? "None" : user.membershipType + " (" + user.membershipStatus + ")") << "\n";
        cout << "\n[1] Edit Name\n";
        cout << "[2] Edit Email\n";
        cout << "[3] Edit Phone\n";
        cout << "[4] Change Password\n";
        cout << "[0] Back\n";

        int option = getValidOption(0, 4);
        if (option == -1)
            continue;

        if (option == 0) {
            // Ask for confirmation before leaving
            if (confirmAction("Are you sure want to return back?")) {
                if (changed) {
                    User_SaveFile(users);
                    cout << "Profile updated and saved successfully.\n";
                }
                else {
                    cout << "Returning...\n";
                }
                waitForEnter();
                return;
            }
            else {
                // User cancelled – stay in the loop
                continue;
            }
        }
        else if (option == 1) {
            cout << "New Name (leave empty to keep current): ";
            getline(cin, input);
            string trimmed = trim(input);
            if (trimmed.empty()) {
                cout << "Name unchanged (empty input).\n";
                waitForEnter();
            }
            else if (trimmed != user.name) {
                user.name = trimmed;
                changed = true;
                cout << "Name updated.\n";
                waitForEnter();
            }
            else {
                cout << "Name unchanged (same as current).\n";
                waitForEnter();
            }
        }
        else if (option == 2) {
            cout << "New Email (leave empty to keep current): ";
            getline(cin, input);
            if (!input.empty()) {
                string trimmed = trim(input);
                if (!validateEmail(trimmed)) {
                    cout << "[X] Invalid email format.\n";
                    waitForEnter();
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
                    waitForEnter();
                    continue;
                }
                string domain = trimmed.substr(trimmed.find('@'));
                if (domain != "@gmail.com" && domain != "@yahoo.com") {
                    cout << "[X] Only Gmail (@gmail.com) and Yahoo (@yahoo.com) email addresses are allowed.\n";
                    waitForEnter();
                    continue;
                }
                user.email = trimmed;
                changed = true;
                cout << "Email updated.\n";
                waitForEnter();
            }
            else {
                cout << "Email unchanged.\n";
                waitForEnter();
            }
        }
        else if (option == 3) {
            cout << "New Phone (leave empty to keep current): ";
            getline(cin, input);
            if (!input.empty()) {
                string trimmed = trim(input);
                if (!validatePhone(trimmed)) {
                    cout << "[X] Invalid phone number. Please enter 10-11 digits.\n";
                    waitForEnter();
                    continue;
                }
                user.phone = trimmed;
                changed = true;
                cout << "Phone updated.\n";
                waitForEnter();
            }
            else {
                cout << "Phone unchanged.\n";
                waitForEnter();
            }
        }
        else if (option == 4) {
            cout << "Current Password: ";
            string oldPwd;
            getline(cin, oldPwd);
            if (oldPwd != user.password) {
                cout << "[X] Incorrect current password.\n";
                waitForEnter();
                continue;
            }

            cout << "New Password: ";
            string newPwd;
            getline(cin, newPwd);
            if (!validatePassword(newPwd)) {
                cout << "[X] Password must be more than " << MIN_PWD << "+ characters\n";
                waitForEnter();
                continue;
            }

            cout << "Confirm New Password: ";
            string confirm;
            getline(cin, confirm);
            if (newPwd != confirm) {
                cout << "[X] Passwords do not match.\n";
                waitForEnter();
                continue;
            }
            user.password = newPwd;
            changed = true;
            cout << "Password updated.\n";
            waitForEnter();
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
    cout << "==========================\n";
    cout << "      DELETE ACCOUNT      \n";
    cout << "==========================\n";

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
        cout << "Enter Email or Customer ID of user to delete: ";
        getline(cin, input);
        string trimmedInput = trim(input);

        if (trimmedInput == "0") {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
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
        outFile << user.accountID << ','
            << user.name << ','
            << user.email << ','
            << user.phone << ','
            << user.password << ','
            << user.registrationDate << ','
            << user.membershipType << ','
            << user.annualFee << ','
            << user.discount << ','
            << user.startDate << ','
            << user.expiryDate << ','
            << user.membershipStatus << '\n';
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
        getline(ss, user.registrationDate, ',');
        if (!getline(ss, user.membershipType, ',')) user.membershipType = "None";
        string feeStr; getline(ss, feeStr, ','); user.annualFee = feeStr.empty() ? 0.0 : stod(feeStr);
        string discStr; getline(ss, discStr, ','); user.discount = discStr.empty() ? 0 : stoi(discStr);
        getline(ss, user.startDate, ',');
        getline(ss, user.expiryDate, ',');
        getline(ss, user.membershipStatus, ',');

        user.accountID = trim(id);
        user.name = trim(user.name);
        user.email = trim(user.email);
        user.phone = trim(user.phone);
        user.password = trim(user.password);
        user.registrationDate = trim(user.registrationDate);
        user.membershipType = trim(user.membershipType);
        user.startDate = trim(user.startDate);
        user.expiryDate = trim(user.expiryDate);
        user.membershipStatus = trim(user.membershipStatus);

        if (user.membershipType == "None") {
            user.annualFee = 0.0;
            user.discount = 0;
            user.startDate = "";
            user.expiryDate = "";
            user.membershipStatus = "";
        }
        else {
            if (!user.expiryDate.empty())
                user.membershipStatus = getMembershipStatus(user.expiryDate);
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
        cout << "      BICYCLE INVENTORY MENU       \n";
        cout << "===================================\n";
        cout << "[1] View all bicycles\n";
        cout << "[2] Add new bicycle\n";
        cout << "[3] Update bicycle\n";
        cout << "[4] Remove bicycle\n";
        cout << "[5] Filter bicycle type\n";
        cout << "[0] Back\n";

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
                cout << "Enter the number between (0-4): ";

                string input;
                getline(cin, input);
                input = trim(input);

                if (input == "0") {
                    if (confirmAction("Are you sure want to return back?")) {
                        cout << "Returning...\n";
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
                cout << left << setw(12) << "ID" << setw(15) << "Type" << setw(15) << "Brand" << setw(15) << "Rate (RM/h)" << setw(12) << "Status" << setw(20) << "Station" << "\n";
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
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
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

string getStationFromUser(bool allowCancel, const string& purpose) {
    while (true) {
        clearScreen();

        // ---- Purpose message ----
        if (purpose == "return") {
            cout << "Step 1 : Choose the station where you will return the bicycle.\n";
        }
        else if (purpose == "pickup") {
            cout << "Step 1 : Choose the station where you will pick up the bicycle.\n";
        }
        else {
            cout << "Choose the station to store the bicycle.\n";
        }

        cout << "\n";
        cout << "===================================\n";
        cout << "    AVAILABLE STATIONS (PENANG)    \n";
        cout << "===================================\n";
        for (size_t i = 0; i < PENANG_STATIONS.size(); ++i) {
            cout << "[" << (i + 1) << "] " << PENANG_STATIONS[i] << "\n";
        }
        if (allowCancel) {
            cout << "[0] Back\n";
        }

        // ---- Prompt with correct range ----
        if (allowCancel) {
            cout << "Enter station by number (0-" << PENANG_STATIONS.size() << "): ";
        }
        else {
            cout << "Enter station by number (1-" << PENANG_STATIONS.size() << "): ";
        }

        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "\n[X] Please enter a number.\n";
            waitForEnter();
            continue;
        }

        // ---- Validate numeric input ----
        bool isNumber = true;
        for (char c : input) {
            if (!isdigit(static_cast<unsigned char>(c))) {
                isNumber = false;
                break;
            }
        }
        if (!isNumber) {
            cout << "\n[X] Invalid input. Please enter a number.\n";
            waitForEnter();
            continue;
        }

        int choice = stoi(input);

        // ---- Handle cancellation ----
        if (allowCancel && choice == 0) {
            return "";   // caller checks for empty string
        }

        // ---- Validate choice range ----
        if (choice >= 1 && choice <= static_cast<int>(PENANG_STATIONS.size())) {
            return PENANG_STATIONS[choice - 1];
        }

        // ---- Invalid option ----
        cout << "\n[X] Invalid option. Please select "
            << (allowCancel ? "0" : "1")
            << "-" << PENANG_STATIONS.size() << ".\n";
        waitForEnter();
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
        cout << "(Enter 0 to return back)\n";
        cout << "Enter Bicycle Type (city, mountain, road, electric): ";
        getline(cin, typeInput);

        if (typeInput == "0") {
            if (confirmAction("Are you sure want to return back? ")) {
                cout << "Returning...\n";
                waitForEnter();
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

    bike.station = getStationFromUser(false, "store");

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
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
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
            string newStation = getStationFromUser(false, "store");
            inventory[idx].station = newStation;
            cout << "Station updated.\n";
            waitForEnter();
            break;
        }
        case 0:
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
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
            if (confirmAction("Are you sure want to return back? ")) {
                cout << "Returning...\n";
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
        waitForEnter(); 
        return;
    }

    cout << "\n===================================\n";
    cout << "             ALL BICYCLES          \n";
    cout << "===================================\n";
    cout << left
        << setw(12) << "Bicycle ID"
        << setw(12) << "Type"
        << setw(14) << "Brand"
        << setw(10) << "Rate (RM/h)"
        << setw(18) << "Station"
        << setw(10) << "Status" << "\n";
    cout << string(80, '-') << "\n";

    for (const auto& bike : inventory) {
        cout << left
            << setw(12) << bike.bikeID
            << setw(12) << bike.type
            << setw(14) << bike.brand
            << setw(10) << fixed << setprecision(2) << bike.rentalRatePerHour
            << setw(18) << bike.station
            << setw(10) << bike.status << "\n";
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
            continue;
        }
        bike.rentalRatePerHour = stod(token);
        getline(ss, token, ','); bike.station = trim(token);
        getline(ss, token, ','); bike.status = trim(token);

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

bool offerMembershipDuringBooking(vector<Account>& users, int currentIdx, string& membershipType, int& discount) {
    Account& user = users[currentIdx];

    // If already a member, skip the prompt
    if (user.membershipType != "None" && user.membershipStatus == "Active") {
        membershipType = user.membershipType;
        discount = user.discount;
        return true; // continue booking
    }

    while (true) {
        clearScreen();
        cout << "\n========================================" << endl;
        cout << "      MEMBERSHIP REGISTRATION" << endl;
        cout << "              (OPTIONAL)" << endl;
        cout << "========================================" << endl;
        cout << endl;

        cout << "Select Membership Type:" << endl;
        cout << "----------------------------------------" << endl;
        cout << " Become a member and enjoy exclusive" << endl;
        cout << "          membership benefits!" << endl;
        cout << endl;

        cout << "  [1] Bronze Membership      RM 50.00" << endl;
        cout << "  [2] Silver Membership      RM 100.00" << endl;
        cout << "  [3] Gold Membership        RM 150.00" << endl;
        cout << "  [4] Skip Membership" << endl;
        cout << "  [0] Back" << endl;

        cout << "----------------------------------------" << endl;
        cout << "Enter choice (0-4): ";

        string input;
        getline(cin, input);

        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "\n[X] Invalid choice. Please enter 0-4.\n";
            waitForEnter();
            continue;
        }

        int choice = input[0] - '0';
        if (choice < 0 || choice > 4) {
            cout << "\n[X] Invalid choice. Please enter 0-4.\n";
            waitForEnter();
            continue;
        }

        if (choice == 0) {
            return false;
        }

        // User chose to skip
        if (choice == 4) {
            membershipType = "None";
            discount = 0;
            return true; // continue booking
        }

        // User chose a membership (1-3)
        string type;
        int disc;
        double fee;
        switch (choice) {
        case 1: type = "Bronze"; disc = 2; fee = 50.00; break;
        case 2: type = "Silver"; disc = 4; fee = 100.00; break;
        case 3: type = "Gold";   disc = 6; fee = 150.00; break;
        }

        // Show membership summary
        clearScreen();
        cout << "========================================\n";
        cout << "        MEMBERSHIP SUMMARY\n";
        cout << "========================================\n";
        cout << "Type         : " << type << "\n";
        cout << "Discount     : " << disc << "%\n";
        cout << "Annual Fee   : RM " << fixed << setprecision(2) << fee << "\n";
        cout << "========================================\n";

        if (!confirmAction("Proceed to membership payment?")) {
            cout << "Membership registration cancelled.\n";
            waitForEnter();
            continue; // show membership menu again
        }

        // Process payment
        string method;
        if (!processMembershipPayment(fee, method)) {
            cout << "Payment failed or cancelled. Membership not registered.\n";
            waitForEnter();
            continue; // show membership menu again
        }

        // Save membership to user account
        string startDate = getCurrentDate();
        string expiryDate = calculateExpiryDate(startDate, 1);
        user.membershipType = type;
        user.discount = disc;
        user.annualFee = fee;
        user.startDate = startDate;
        user.expiryDate = expiryDate;
        user.membershipStatus = "Active";
        User_SaveFile(users);

        // Return the new membership data
        membershipType = type;
        discount = disc;

        cout << "\n========================================\n";
        cout << "   MEMBERSHIP REGISTRATION SUCCESSFUL!\n";
        cout << "========================================\n";
        cout << "You are now a " << type << " member.\n";
        cout << "Enjoy your " << disc << "% discount on this rental!\n";
        cout << "========================================\n";
        waitForEnter();

        return true; // continue booking
    }
}

void CreateBooking(vector<Booking>& bookings, vector<Bicycle>& inventory,
    vector<Account>& users, int currentIdx,
    const string& customerID, const string& customerName,
    const string& membershipType, int discount) {
    clearScreen();

    // ------------------------------------------------------------
    // 1. Select pickup station (loop until valid or cancel)
    // ------------------------------------------------------------
    string pickupStation;
    while (true) {
        pickupStation = getStationFromUser(true, "pickup");
        if (pickupStation.empty()) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
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

    // ------------------------------------------------------------
    // 2. Choose bicycle type – loop until a bike is assigned or user cancels
    // ------------------------------------------------------------
    string assignedBikeID = "";
    string selectedType = "";
    bool bikeFound = false;

    while (!bikeFound) {
        clearScreen();

        // ----- HEADER -----
        cout << "=======================================\n";
        cout << "   Available Bicycles at " << pickupStation << "\n";
        cout << "=======================================\n\n";

        int cityCount = 0, mountainCount = 0, roadCount = 0, electricCount = 0;
        for (const auto& bike : inventory) {
            if (bike.station == pickupStation && bike.status == "Available") {
                if (bike.type == "city") cityCount++;
                else if (bike.type == "mountain") mountainCount++;
                else if (bike.type == "road") roadCount++;
                else if (bike.type == "electric") electricCount++;
            }
        }

        struct BikeOption {
            string type;
            double rate;
            int count;
            int number;
        };
        vector<BikeOption> options = {
            {"Road", RATE_ROAD, roadCount, 1},
            {"City", RATE_CITY, cityCount, 2},
            {"Mountain", RATE_MOUNTAIN, mountainCount, 3},
            {"Electric", RATE_ELECTRIC, electricCount, 4}
        };

        cout << left << setw(8) << "Option"
            << setw(14) << "Type"
            << setw(16) << "Price / Hour"
            << "Available\n";
        cout << string(45, '-') << "\n";

        for (const auto& opt : options) {
            cout << "[" << opt.number << "]"
                << setw(5) << " "
                << setw(14) << opt.type
                << "RM " << fixed << setprecision(2) << setw(12) << opt.rate
                << opt.count << "\n";
        }
        cout << "[0] Back\n";
        cout << "Select bike type (0-4): ";

        string typeInput;
        getline(cin, typeInput);

        if (typeInput == "0") {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            else {
                continue;
            }
        }

        if (typeInput.length() != 1 || !isdigit(typeInput[0])) {
            cout << "[X] Invalid choice. Please enter a number between 0 and 4.\n";
            waitForEnter();
            continue;
        }
        int typeChoice = typeInput[0] - '0';
        if (typeChoice < 1 || typeChoice > 4) {
            cout << "[X] Invalid choice. Please enter 1-4 or 0 to cancel.\n";
            waitForEnter();
            continue;
        }

        switch (typeChoice) {
        case 1: selectedType = "road"; break;
        case 2: selectedType = "city"; break;
        case 3: selectedType = "mountain"; break;
        case 4: selectedType = "electric"; break;
        }

        for (const auto& bike : inventory) {
            if (bike.type == selectedType && bike.station == pickupStation && bike.status == "Available") {
                assignedBikeID = bike.bikeID;
                break;
            }
        }

        if (assignedBikeID.empty()) {
            cout << "\n[X] No available " << selectedType << " bicycles at " << pickupStation << ".\n";
            cout << "Please select another type.\n";
            waitForEnter();
        }
        else {
            bikeFound = true;
            cout << "Assigned bicycle: " << assignedBikeID << " (" << selectedType << ")\n";
            waitForEnter();
        }
    }

    // ------------------------------------------------------------
    // 3. Offer membership registration (OPTIONAL)
    // ------------------------------------------------------------
    string finalMembershipType = membershipType;
    int finalDiscount = discount;
    bool membershipResult = offerMembershipDuringBooking(users, currentIdx, finalMembershipType, finalDiscount);
    if (!membershipResult) {
        cout << "Returning...\n";
        waitForEnter();
        return;
    }

    // ------------------------------------------------------------
    // 4. Get start and end times – with immediate validation
    // ------------------------------------------------------------
    string startTime, endTime;
    int duration = 0;
    bool validTimes = false;
    do {
        // Validate start time
        bool startValid = false;
        while (!startValid) {
            cout << "Enter start time (HH:MM, 24-hour format, e.g., 14:30): ";
            getline(cin, startTime);
            if (startTime.length() != 5 || startTime[2] != ':' ||
                !isdigit(startTime[0]) || !isdigit(startTime[1]) ||
                !isdigit(startTime[3]) || !isdigit(startTime[4])) {
                cout << "Invalid format. Please use HH:MM.\n";
                continue;
            }
            int sh = stoi(startTime.substr(0, 2)), sm = stoi(startTime.substr(3, 2));
            if (sh < 0 || sh > 23 || sm < 0 || sm > 59) {
                cout << "Invalid hour or minute values. Hour must be 0-23, minute 0-59.\n";
                continue;
            }
            startValid = true;
        }

        // Validate end time
        bool endValid = false;
        while (!endValid) {
            cout << "Enter end time (HH:MM, 24-hour format, e.g., 14:30): ";
            getline(cin, endTime);
            if (endTime.length() != 5 || endTime[2] != ':' ||
                !isdigit(endTime[0]) || !isdigit(endTime[1]) ||
                !isdigit(endTime[3]) || !isdigit(endTime[4])) {
                cout << "Invalid format. Please use HH:MM.\n";
                continue;
            }
            int eh = stoi(endTime.substr(0, 2)), em = stoi(endTime.substr(3, 2));
            if (eh < 0 || eh > 23 || em < 0 || em > 59) {
                cout << "Invalid hour or minute values. Hour must be 0-23, minute 0-59.\n";
                continue;
            }
            endValid = true;
        }

        // Calculate duration
        int sh = stoi(startTime.substr(0, 2)), sm = stoi(startTime.substr(3, 2));
        int eh = stoi(endTime.substr(0, 2)), em = stoi(endTime.substr(3, 2));
        int startMinutes = sh * 60 + sm;
        int endMinutes = eh * 60 + em;
        int diffMinutes = endMinutes - startMinutes;
        if (diffMinutes <= 0) diffMinutes += 24 * 60;

        duration = (diffMinutes + 59) / 60;
        if (duration < 1) {
            cout << "Rental duration must be at least 1 hour.\n";
            continue;
        }
        if (duration > MAX_RENTAL_HOURS) {
            cout << "Rental duration cannot exceed " << MAX_RENTAL_HOURS << " hours.\n";
            continue;
        }
        validTimes = true;
    } while (!validTimes);

    // ------------------------------------------------------------
    // 5. Get current booking date and time
    // ------------------------------------------------------------
    string bookingDate, bookingTime;
    getCurrentDateTime(bookingDate, bookingTime);

    // ------------------------------------------------------------
    // 6. Compute base rental rate
    // ------------------------------------------------------------
    double baseRate = 0.0;
    for (const auto& bike : inventory) {
        if (bike.bikeID == assignedBikeID) {
            baseRate = bike.rentalRatePerHour;
            break;
        }
    }

    // ------------------------------------------------------------
    // 7. Build the Booking object
    // ------------------------------------------------------------
    Booking tempBooking;
    tempBooking.bookingID = generateBookingID(bookings);
    tempBooking.customerID = customerID;
    tempBooking.bikeID = assignedBikeID;
    tempBooking.bookingDate = bookingDate;
    tempBooking.bookingTime = startTime;
    tempBooking.pickupStation = pickupStation;
    tempBooking.returnTime = endTime;
    tempBooking.paymentMethod = "";
    tempBooking.membershipType = finalMembershipType;
    tempBooking.discount = finalDiscount;
    tempBooking.duration = duration;
    tempBooking.status = "Active";

    // ------------------------------------------------------------
    // 8. Show booking summary and confirm
    // ------------------------------------------------------------
    clearScreen();
    cout << "==============================\n";
    cout << "      BOOKING SUMMARY      \n";
    cout << "==============================\n";
    cout << "Booking ID     : " << tempBooking.bookingID << "\n";
    cout << "Customer ID    : " << tempBooking.customerID << "\n";
    cout << "Bicycle ID     : " << tempBooking.bikeID << "\n";
    cout << "Date           : " << tempBooking.bookingDate << "\n";
    cout << "Start Time     : " << tempBooking.bookingTime << "\n";
    cout << "End Time       : " << tempBooking.returnTime << "\n";
    cout << "Duration       : " << tempBooking.duration << " hours\n";
    cout << "Pickup Station : " << tempBooking.pickupStation << "\n";
    cout << "Membership     : " << tempBooking.membershipType << " (" << tempBooking.discount << "% discount)\n";
    cout << "==============================\n";

    if (!confirmAction("Do you confirm all details are correct?")) {
        cout << "\nBooking cancelled.\n";
        waitForEnter();
        return;
    }

    // ------------------------------------------------------------
    // 9. Process payment
    // ------------------------------------------------------------
    string bikeType;
    for (const auto& bike : inventory) {
        if (bike.bikeID == assignedBikeID) {
            bikeType = bike.type;
            break;
        }
    }

    string paymentMethod = ProcessPaymentForBooking(tempBooking, duration, baseRate, customerName, bikeType);
    if (paymentMethod.empty()) {
        cout << "Payment failed or cancelled. Booking not created.\n";
        waitForEnter();
        return;
    }
    tempBooking.paymentMethod = paymentMethod;

    // ------------------------------------------------------------
    // 10. Mark bike as rented
    // ------------------------------------------------------------
    for (auto& bike : inventory) {
        if (bike.bikeID == assignedBikeID) {
            bike.status = "Rented";
            break;
        }
    }

    // ------------------------------------------------------------
    // 11. Save booking
    // ------------------------------------------------------------
    bookings.push_back(tempBooking);
    SaveBookingsToFile(bookings, BOOKING_FILE);
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
        cout << "==============================\n";
        cout << "      MY BOOKING DETAILS      \n";
        cout << "==============================\n";
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

        cout << "[1] Previous Page  [2] Next Page  [0] Back\n";
        string input;
        cout << "Enter your option (0-2): ";
        getline(cin, input);
        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid input. Please enter (0-2).\n";
            waitForEnter();
            continue;
        }
        int opt = input[0] - '0';
        if (opt == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
        }
        else if (opt == 1) {
            if (page > 0) page--;
            else {
                cout << "This is the first page.\n";
                waitForEnter();
            }
        }
        else if (opt == 2) {
            if (page + 1 < totalPages) page++;
            else {
                cout << "This is the last page.\n";
                waitForEnter();
            }
        }
        else {
            cout << "Invalid option. Please enter 0, 1, or 2.\n";
            waitForEnter();
        }
    }
}

void ReturnBicycle(vector<Booking>& bookings, vector<Bicycle>& inventory,
    const string& customerID) {
    clearScreen();
    cout << "==============================\n";
    cout << "        RETURN BICYCLE        \n";
    cout << "==============================\n";

    while (true) { // Outer loop – booking selection
        // ------------------------------------------------------------
        // 1. Find all active bookings for this customer
        // ------------------------------------------------------------
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

        // ------------------------------------------------------------
        // 2. Display active bookings in table format
        // ------------------------------------------------------------
        clearScreen();
        cout << "==============================\n";
        cout << "        RETURN BICYCLE        \n";
        cout << "==============================\n";
		cout << "Your active bookings:\n\n";
        cout << left << setw(6) << "No."
            << setw(14) << "Booking ID"
            << setw(12) << "Bike ID"
            << setw(16) << "Bicycle Type"
            << setw(16) << "Total Hours"
            << "Scheduled Return\n";
        cout << string(80, '-') << "\n";

        for (size_t i = 0; i < active.size(); ++i) {
            // Look up bicycle type from inventory
            string bikeType = "Unknown";
            for (const auto& bike : inventory) {
                if (bike.bikeID == active[i]->bikeID) {
                    bikeType = bike.type;
                    break;
                }
            }

            cout << left << setw(6) << (i + 1)
                << setw(14) << active[i]->bookingID
                << setw(12) << active[i]->bikeID
                << setw(16) << bikeType
                << setw(16) << active[i]->duration
                << active[i]->returnTime << "\n";
        }
        cout << string(80, '-') << "\n";
        cout << "[0] Back\n\n";

        // ------------------------------------------------------------
        // 3. Let the user choose which booking to return
        // ------------------------------------------------------------
        string choiceInput;
        cout << "Enter your option to return (0-" << active.size() << "): ";
        getline(cin, choiceInput);

        if (choiceInput == "0") {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            continue;
        }

        int idx;
        if (choiceInput.length() != 1 || !isdigit(choiceInput[0])) {
            cout << "[X] Invalid choice.\n";
            waitForEnter();
            continue;
        }
        idx = choiceInput[0] - '0';
        if (idx < 1 || idx > static_cast<int>(active.size())) {
            cout << "[X] Invalid choice.\n";
            waitForEnter();
            continue;
        }

        Booking& selected = *active[idx - 1];

        // ------------------------------------------------------------
        // Inner loop – station selection and confirmation
        // ------------------------------------------------------------
        bool returnCompleted = false;
        while (!returnCompleted) {
            // ------------------------------------------------------------
            // 4. Compute actual hours used (from start time to now)
            // ------------------------------------------------------------
            string currentDate, currentTime;
            getCurrentDateTime(currentDate, currentTime);
            double actualHours = hoursDifference(selected.bookingDate, selected.bookingTime,
                currentDate, currentTime);
            if (actualHours < 0) actualHours = 0;

            double overtimeHours = 0;
            if (actualHours > selected.duration) {
                overtimeHours = actualHours - selected.duration;
            }

            // ------------------------------------------------------------
            // 5. Ask for return station – allows cancellation with "0"
            // ------------------------------------------------------------
            string returnStation = getStationFromUser(true, "return");
            if (returnStation.empty()) {
                cout << "Return cancelled. Please select another booking.\n";
                waitForEnter();
                break; // exit inner loop, goes to outer loop (booking list)
            }

            // ------------------------------------------------------------
            // 6. Show return summary
            // ------------------------------------------------------------
            clearScreen();
            cout << "==============================\n";
            cout << "       RETURN SUMMARY\n";
            cout << "==============================\n";
            cout << "Booking ID    : " << selected.bookingID << "\n";
            cout << "Bicycle ID    : " << selected.bikeID << "\n";
            cout << "Pickup Station: " << selected.pickupStation << "\n";
            cout << "Return Station: " << returnStation << "\n";
            cout << "Scheduled Duration : " << selected.duration << " hours\n";
            cout << "Actual Duration    : " << fixed << setprecision(1) << actualHours << " hours\n";
            if (overtimeHours > 0) {
                double overtimeFee = overtimeHours * OVERTIME_RATE;
                cout << "Overtime Hours : " << fixed << setprecision(1) << overtimeHours << " hours\n";
                cout << "Overtime Fee   : RM " << fixed << setprecision(2) << overtimeFee << "\n";
            }
            cout << "--------------------------\n";

            if (confirmAction("Do you confirm the return of this bicycle?")) {
                // ------------------------------------------------------------
                // 7. Process overtime payment if any
                // ------------------------------------------------------------
                if (overtimeHours > 0) {
                    double overtimeFee = overtimeHours * OVERTIME_RATE;
                    cout << "\nYou have overtime charges of RM " << fixed << setprecision(2) << overtimeFee << ".\n";
                    string method;
                    if (processMembershipPayment(overtimeFee, method)) {
                        cout << "Overtime payment successful.\n";
                        Payment otPayment;
                        otPayment.paymentID = generatePaymentID(transactions);
                        otPayment.bookingID = selected.bookingID;
                        otPayment.bikeID = selected.bikeID;
                        otPayment.customerName = currentCustomer;
                        otPayment.paymentAmount = overtimeFee;
                        otPayment.paymentMethod = method;
                        string date, time;
                        getCurrentDateTime(date, time);
                        otPayment.paymentDate = date;
                        transactions.push_back(otPayment);
                        SavePaymentToFile();
                    }
                    else {
                        cout << "Overtime payment failed or cancelled. Return process aborted.\n";
                        waitForEnter();
                        continue; // stay in inner loop, re-ask station?
                    }
                }

                // ------------------------------------------------------------
                // 8. Update bike status and station
                // ------------------------------------------------------------
                for (auto& bike : inventory) {
                    if (bike.bikeID == selected.bikeID) {
                        bike.status = "Available";
                        bike.station = returnStation;
                        break;
                    }
                }

                // ------------------------------------------------------------
                // 9. Mark booking as completed
                // ------------------------------------------------------------
                selected.status = "Completed";
                SaveBookingsToFile(bookings, BOOKING_FILE);

                cout << "\nBicycle returned successfully.\n";
                cout << "Booking ID: " << selected.bookingID << " completed.\n";
                cout << "Bicycle now at: " << returnStation << "\n";
                waitForEnter();
                return; // exit the whole function
            }
            else {
                // User declined the return
                cout << "\nDo you want to change the return station? (Y/N): ";
                string change;
                getline(cin, change);
                if (change == "Y" || change == "y") {
                    // Loop back to station selection
                    continue;
                }
                else {
                    // Go back to booking list
                    cout << "Return cancelled.\n";
                    waitForEnter();
                    break; // exit inner loop, goes to outer loop
                }
            }
        } // end inner while (!returnCompleted)
    }
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
            << b.membershipType << ","
            << b.discount << ","
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
        getline(ss, token, ','); b.membershipType = trim(token);
        string discStr; getline(ss, discStr, ','); b.discount = discStr.empty() ? 0 : stoi(discStr);
        string durStr; getline(ss, durStr, ','); b.duration = durStr.empty() ? 0 : stoi(durStr);
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
string readCardNumberWithFormat() {
    string raw;
    const string prompt = "Card number (e.g., 1111 2222 3333 4444): ";
    cout << prompt << flush;

    while (true) {
        int ch = _getch();

        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }

        if (ch == '\r' || ch == '\n') {
            cout << endl;
            break;
        }

        if (ch == '\b') {
            if (!raw.empty()) {
                raw.pop_back();
                cout << '\r' << string(80, ' ') << '\r';
                cout << prompt;
                string formatted;
                for (size_t i = 0; i < raw.length(); ++i) {
                    if (i > 0 && i % 4 == 0) formatted += ' ';
                    formatted += raw[i];
                }
                cout << formatted << flush;
            }
            continue;
        }

        if (ch >= '0' && ch <= '9') {
            if (raw.length() < 16) {
                raw.push_back(static_cast<char>(ch));
                cout << '\r' << string(80, ' ') << '\r';
                cout << prompt;
                string formatted;
                for (size_t i = 0; i < raw.length(); ++i) {
                    if (i > 0 && i % 4 == 0) formatted += ' ';
                    formatted += raw[i];
                }
                cout << formatted << flush;
            }
        }
    }
    return raw;
}

string ProcessPaymentForBooking(const Booking& booking, double rentalHours,
    double baseRate, const string& customerName, const string& bikeType) {
    double baseFee = CalculateRentalFee(rentalHours, baseRate);
    double overtimeFee = 0.0;
    if (rentalHours > 24.0)
        overtimeFee = CalculateOvertimeFee(rentalHours - 24.0);

    double subtotal = baseFee + overtimeFee;
    if (booking.membershipType != "None" && !booking.membershipType.empty()) {
        double disc = booking.discount / 100.0;
        subtotal -= subtotal * disc;
    }
    // ---- ROUNDING FIX ----
    subtotal = roundToTwo(subtotal);
    double tax = roundToTwo(subtotal * TAX_RATE);
    double total = roundToTwo(subtotal + tax);

    clearScreen();
    cout << fixed << setprecision(2);
    cout << "\n==============================\n";
    cout << "       PAYMENT SUMMARY\n";
    cout << "==============================\n";
    cout << "Booking ID   : " << booking.bookingID << "\n";
    cout << "Bike ID      : " << booking.bikeID << "\n";
    cout << "Bike Type    : " << bikeType << "\n";
    cout << "Duration     : " << rentalHours << " hours\n";
    cout << "Base Rate    : RM " << baseRate << "/hour\n";
    cout << "Base Fee     : RM " << baseFee << "\n";
    if (overtimeFee > 0)
        cout << "Overtime Fee : RM " << overtimeFee << " (over 24 hrs)\n";
    if (booking.membershipType != "None")
        cout << "Membership   : " << booking.membershipType << " (" << booking.discount << "% off)\n";
    cout << "Subtotal     : RM " << subtotal << "\n";
    cout << "Tax (6%)     : RM " << tax << "\n";
    cout << "Total Amount : RM " << total << "\n";
    cout << "=================================\n\n";

    // ---------- PAYMENT AMOUNT REMOVED – use total directly ----------
    double paid = total;   // amount paid is exactly the total

    // ---------- PAYMENT METHOD SELECTION – no default, loop until valid ----------
    int methodChoice;
    string method;
    while (true) {
        cout << "Payment method:\n1. Debit Card\n2. Credit Card\n3. Touch'n Go\nPlease select the payment method: ";
        string methInput;
        getline(cin, methInput);
        if (methInput.empty() || methInput.length() != 1 || !isdigit(methInput[0])) {
            cout << "[X] Invalid input. Please enter a number between 1 and 3.\n\n";
            continue;
        }
        methodChoice = methInput[0] - '0';
        if (methodChoice < 1 || methodChoice > 3) {
            cout << "[X] Invalid choice. Please enter 1, 2, or 3.\n\n";
            continue;
        }
        break;
    }
    switch (methodChoice) {
    case 1: method = "Debit Card"; break;
    case 2: method = "Credit Card"; break;
    case 3: method = "Touch'n Go"; break;
    }

    // ---------- CARD PAYMENT ----------
    if (methodChoice == 1 || methodChoice == 2) {
        string cardNum, expiry, cvv;

        bool validCard = false;
        do {
            cardNum = readCardNumberWithFormat();

            if (cardNum == "0") {
                cout << "Payment cancelled by user.\n";
                return "";
            }

            if (cardNum.length() != 16) {
                cout << "[X] Invalid card number. Must be exactly 16 digits.\n";
            }
            else {
                validCard = true;
            }
        } while (!validCard);

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
                cout << "[X] Invalid CVV. Only 3 digits allowed.\n";
            }
            else {
                cvvOk = true;
            }
        } while (!cvvOk);

        cout << "Card verified.\n";
        waitForEnter();
        clearScreen();
    }
    // ---------- TOUCH 'N GO ----------
    else if (methodChoice == 3) {
        string phone;
        bool phoneValid = false;
        do {
            cout << "Touch'n Go phone number (e.g., 0123456789): ";
            getline(cin, phone);
            if (phone.length() < 10 || phone.length() > 11) {
                cout << "[X] Invalid phone number. Must be 10 or 11 digits.\n";
                continue;
            }
            bool ok = true;
            for (char c : phone) if (!isdigit(c)) { ok = false; break; }
            if (!ok || phone.substr(0, 2) != "01") {
                cout << "[X] Invalid phone number. Must start with '01' and contain only digits.\n";
                continue;
            }
            phoneValid = true;
        } while (!phoneValid);

        // ---- NEW: password prompt (6 digits) ----
        string password;
        bool pwdValid = false;
        do {
            cout << "Touch'n Go pin numbers (6 digits): ";
            getline(cin, password);
            if (password.length() != 6) {
                cout << "[X] Password must be exactly 6 digits.\n";
                continue;
            }
            bool allDigits = true;
            for (char c : password) if (!isdigit(c)) { allDigits = false; break; }
            if (!allDigits) {
                cout << "[X] Password must contain only digits.\n";
                continue;
            }
            pwdValid = true;
        } while (!pwdValid);

        cout << "Touch'n Go validated.\n";
        waitForEnter();
        clearScreen();
    }

    // ---------- RECORD PAYMENT ----------
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
    newP.refundID = generateRefundID(transactions);
    newP.refundStatus = "";
    newP.requestDate = "";

    transactions.push_back(newP);
    SavePaymentToFile();

    cout << "\nPayment successful! Payment ID: " << newP.paymentID << "\n";
    GenerateReceipt(newP.paymentID, booking.bookingID, booking.bikeID, customerName, paid, method, date, booking.bookingDate, booking.bookingTime, booking.returnTime, booking.pickupStation);
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

void DisplayUserPaymentHistory(const string& customerName, const vector<Booking>& bookings, const vector<Bicycle>& inventory) {
    clearScreen();

    vector<Payment> customerPayments;
    for (const auto& p : transactions) {
        if (p.customerName == customerName) {
            customerPayments.push_back(p);
        }
    }

    if (customerPayments.empty()) {
        cout << "No payment records found for this customer.\n";
        waitForEnter();
        return;
    }

    const int PER_PAGE = 5;
    int page = 0;
    int totalPages = (customerPayments.size() + PER_PAGE - 1) / PER_PAGE;

    while (true) {
        clearScreen();
        cout << "==============================\n";
        cout << "      MY PAYMENT HISTORY      \n";
        cout << "==============================\n";
        cout << left
            << setw(12) << "Payment ID"
            << setw(12) << "Booking ID"
            << setw(14) << "Bike Type"          // Changed from "Bike ID"
            << setw(8) << "Hours"
            << setw(14) << "Amount (RM)"
            << setw(16) << "Method"
            << setw(12) << "Date" << "\n";
        cout << string(88, '-') << "\n";        // adjusted width

        size_t first = page * PER_PAGE;
        size_t last = min(first + PER_PAGE, customerPayments.size());
        for (size_t i = first; i < last; ++i) {
            const Payment& p = customerPayments[i];
            string durationStr = "-";
            for (const auto& b : bookings) {
                if (b.bookingID == p.bookingID) {
                    durationStr = to_string(b.duration) + "h";
                    break;
                }
            }

            // ─── Get bike type from inventory ───
            string bikeType = "Unknown";
            for (const auto& bike : inventory) {
                if (bike.bikeID == p.bikeID) {
                    bikeType = bike.type;
                    break;
                }
            }

            string status = p.paymentAmount < 0 ? p.refundStatus : "-";
            if (p.paymentAmount < 0 && p.refundStatus == "Rejected" && !p.rejectReason.empty()) {
                status += " (Reason: " + p.rejectReason + ")";
            }

            cout << left
                << setw(12) << p.paymentID
                << setw(12) << p.bookingID
                << setw(14) << bikeType           // now displays bike type
                << setw(8) << durationStr
                << setw(14) << fixed << setprecision(2) << p.paymentAmount
                << setw(16) << p.paymentMethod
                << setw(12) << p.paymentDate << "\n";
        }

        cout << string(88, '-') << "\n";
        cout << "Page " << page + 1 << "/" << totalPages << "\n\n";
        cout << "[1] Previous Page  [2] Next Page  [0] Back\n";
        cout << "Enter your option (0-2): ";
        string input;
        getline(cin, input);

        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid input. Please enter (0-2).\n";
            waitForEnter();
            continue;
        }
        int opt = input[0] - '0';

        if (opt == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
        }
        else if (opt == 1) {
            if (page > 0) page--;
            else {
                cout << "This is the first page.\n";
                waitForEnter();
            }
        }
        else if (opt == 2) {
            if (page + 1 < totalPages) page++;
            else {
                cout << "This is the last page.\n";
                waitForEnter();
            }
        }
        else {
            cout << "Invalid option. Please enter 0, 1, or 2.\n";
            waitForEnter();
        }
    }
}

void RequestRefund(vector<Account>& users, int currentIdx,
    vector<Booking>& bookings,
    vector<Payment>& transactions,
    const vector<Bicycle>& inventory)
{
    clearScreen();

    const Account& user = users[currentIdx];

    // 1. Find all completed bookings for this user
    vector<Booking> completed;
    for (const auto& b : bookings) {
        if (b.customerID == user.accountID && b.status == "Completed") {
            completed.push_back(b);
        }
    }

    if (completed.empty()) {
        cout << "You have no completed bookings to request a refund for.\n";
        waitForEnter();
        return;
    }

    while (true) {
        map<string, string> refundStatusMap;
        map<string, string> refundReasonMap;
        map<string, string> paymentIDMap;

        for (const auto& b : completed) {
            // Find the original payment (positive amount) for this booking
            for (const auto& p : transactions) {
                if (p.bookingID == b.bookingID && p.paymentAmount > 0) {
                    paymentIDMap[b.bookingID] = p.paymentID;
                    break;
                }
            }
            // Find any refund request (negative amount) for this booking
            for (const auto& p : transactions) {
                if (p.bookingID == b.bookingID && p.paymentAmount < 0 && !p.refundStatus.empty()) {
                    refundStatusMap[b.bookingID] = p.refundStatus;
                    refundReasonMap[b.bookingID] = p.rejectReason;
                    break;
                }
            }
        }

        // 3. Display the list
        clearScreen();
        cout << "==============================\n";
        cout << "         REQUEST REFUND       \n";
        cout << "==============================\n";
        cout << "\nSelect a booking to request refund:\n\n";
        cout << left << setw(6) << "No."
            << setw(14) << "Payment ID"
            << setw(14) << "Booking ID"
            << setw(14) << "Bike Type"
            << setw(12) << "Date"
            << "Refund Status\n";
        cout << string(80, '-') << "\n";

        for (size_t i = 0; i < completed.size(); ++i) {
            const auto& b = completed[i];

            // Get payment ID
            string paymentID = paymentIDMap.count(b.bookingID) ? paymentIDMap[b.bookingID] : "N/A";

            // Get bike type from inventory
            string bikeType = "Unknown";
            for (const auto& bike : inventory) {
                if (bike.bikeID == b.bikeID) {
                    bikeType = bike.type;
                    break;
                }
            }

            // Get refund status
            string status = refundStatusMap.count(b.bookingID) ? refundStatusMap[b.bookingID] : "Available";
            if (status == "Rejected" && !refundReasonMap[b.bookingID].empty()) {
                status += " (Reason: " + refundReasonMap[b.bookingID] + ")";
            }

            cout << left << setw(6) << (i + 1)
                << setw(14) << paymentID
                << setw(14) << b.bookingID
                << setw(14) << bikeType
                << setw(12) << b.bookingDate
                << status << "\n";
        }
        cout << string(80, '-') << "\n";
        cout << "[0] Cancel\n\n";

        // 4. Get user choice
        int choice = getValidOption(0, (int)completed.size());
        if (choice == -1) {
            clearScreen();
            continue;
        }

        if (choice == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            else {
                clearScreen();
                continue;
            }
        }

        // 5. Valid selection
        Booking& selected = completed[choice - 1];

        // 6. Check if this booking already has a refund request
        if (refundStatusMap.count(selected.bookingID)) {
            string status = refundStatusMap[selected.bookingID];
            cout << "\nThis booking already has a refund status: " << status << ".\n";
            if (status == "Rejected" && !refundReasonMap[selected.bookingID].empty()) {
                cout << "Rejection reason: " << refundReasonMap[selected.bookingID] << "\n";
            }
            cout << "No new request can be submitted.\n";
            waitForEnter();
            continue;
        }

        // 7. Create refund request – reason is now REQUIRED
        string reason;
        while (true) {
            cout << "\nPlease provide a brief reason for the refund: ";
            getline(cin, reason);
            if (!reason.empty()) {
                break;
            }
            cout << "[X] Reason cannot be empty. Please enter a reason.\n";
        }

        Payment refundRequest;
        refundRequest.paymentID = generatePaymentID(transactions);
        refundRequest.bookingID = selected.bookingID;
        refundRequest.bikeID = selected.bikeID;
        refundRequest.customerName = user.name;
        refundRequest.paymentAmount = -1.0;          // negative to mark as refund
        refundRequest.paymentMethod = "Refund Request";
        string date, time;
        getCurrentDateTime(date, time);
        refundRequest.paymentDate = date;
        refundRequest.refundID = generateRefundID(transactions);
        refundRequest.refundStatus = "Pending";
        refundRequest.requestDate = date;
        refundRequest.rejectReason = "";

        transactions.push_back(refundRequest);
        SavePaymentToFile();

        cout << "\nRefund request submitted successfully!\n";
        cout << "Your request ID: " << refundRequest.refundID << "\n";
        waitForEnter();
        return;
    }
}

void adminRefundManagement(vector<Payment>& transactions)
{
    while (true) {
        clearScreen();
        cout << "==============================\n";
        cout << "       REFUND MANAGEMENT      \n";
        cout << "==============================\n";

        // 1. Gather all pending refund requests
        vector<Payment*> pending;
        for (auto& p : transactions) {
            if (p.paymentAmount < 0 && p.refundStatus == "Pending") {
                pending.push_back(&p);
            }
        }

        if (pending.empty()) {
            cout << "No pending refund requests.\n";
            waitForEnter();
            return;
        }

        // 2. Display pending requests in a table
        cout << "\nPlease select a refund request to manage:\n";
        cout << left
            << setw(5) << "No"
            << setw(14) << "Refund ID"
            << setw(14) << "Booking ID"
            << setw(22) << "Customer"
            << setw(12) << "Date" << "\n";
        cout << string(70, '-') << "\n";

        for (size_t i = 0; i < pending.size(); ++i) {
            string idxStr = "[" + to_string(i + 1) + "]";
            cout << setw(5) << idxStr
                << setw(14) << pending[i]->refundID
                << setw(14) << pending[i]->bookingID
                << setw(22) << pending[i]->customerName
                << setw(12) << pending[i]->requestDate << "\n";
        }
        cout << string(70, '-') << "\n";
        cout << "[0] Back\n";

        // 3. Get admin choice
        int choice = getValidOption(0, (int)pending.size());

        if (choice == -1) {
            continue;
        }

        if (choice == 0) {
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
            continue;   // user cancelled – stay on the list (outer loop)
        }

        // ---- Valid selection ----
        Payment& selected = *pending[choice - 1];

        // ----- Inner loop for details page (so cancelling "Back" stays on details) -----
        bool detailsDone = false;
        while (!detailsDone) {
            clearScreen();
            cout << "==============================\n";
            cout << "    REFUND REQUEST DETAILS    \n";
            cout << "==============================\n";
            cout << "Refund ID  : " << selected.refundID << "\n";
            cout << "Booking ID : " << selected.bookingID << "\n";
            cout << "Customer   : " << selected.customerName << "\n";
            cout << "Requested  : " << selected.requestDate << "\n";

            // Find original payment amount
            double originalAmount = 0.0;
            for (const auto& p : transactions) {
                if (p.bookingID == selected.bookingID && p.paymentAmount > 0) {
                    originalAmount = p.paymentAmount;
                    break;
                }
            }
            cout << "Original amount: RM " << fixed << setprecision(2) << originalAmount << "\n";

            // 5. Approve or reject
            cout << "\n[1] Approve refund\n";
            cout << "[2] Reject refund\n";
            cout << "[0] Back\n";

            int action = getValidOption(0, 2);

            if (action == -1) {
                continue;   
            }

            if (action == 0) {
                if (confirmAction("Are you sure want to go back?")) {
                    detailsDone = true;   
                }
                continue;   
            }

            if (action == 1) {
                selected.refundStatus = "Approved";
                cout << "\nRefund approved.\n";
                SavePaymentToFile();
                waitForEnter();
                return;   
            }
            else if (action == 2) {
                cout << "Enter reason for rejection: ";
                string reason;
                getline(cin, reason);
                selected.refundStatus = "Rejected";
                selected.rejectReason = reason;
                cout << "\nRefund rejected.\n";
                SavePaymentToFile();
                waitForEnter();
                return;  
            }
        } 
    } 
}


void DisplayAllPayments(const vector<Booking>& bookings) {
    clearScreen();
    if (transactions.empty()) {
        cout << "No payment records found.\n";
        waitForEnter();
        return;
    }

    const int PER_PAGE = 10;   // adjust as needed
    int page = 0;
    int totalPages = (transactions.size() + PER_PAGE - 1) / PER_PAGE;

    while (true) {
        clearScreen();
        cout << "\n===================================";
        cout << "\n          ALL TRANSACTIONS          ";
        cout << "\n===================================\n";
        cout << left << setw(12) << "PaymentID"
            << setw(12) << "BookingID"
            << setw(12) << "BikeID"
            << setw(20) << "Customer"
            << setw(12) << "Date"
            << setw(10) << "Hours"
            << setw(18) << "Method"
            << setw(12) << "Amount" << "\n";
        cout << string(108, '-') << "\n";

        size_t first = page * PER_PAGE;
        size_t last = min(first + PER_PAGE, transactions.size());
        for (size_t i = first; i < last; ++i) {
            const Payment& p = transactions[i];
            string durationStr = "-";
            for (const auto& b : bookings) {
                if (b.bookingID == p.bookingID) {
                    durationStr = to_string(b.duration) + "h";
                    break;
                }
            }

            cout << left << setw(12) << p.paymentID
                << setw(12) << p.bookingID
                << setw(12) << p.bikeID
                << setw(20) << p.customerName
                << setw(12) << p.paymentDate
                << setw(10) << durationStr
                << setw(18) << p.paymentMethod
                << setw(12) << fixed << setprecision(2) << p.paymentAmount << "\n";
        }

        cout << string(108, '-') << "\n";
        cout << "Page " << page + 1 << "/" << totalPages << "\n\n";
        cout << "[1] Previous Page  [2] Next Page  [0] Back\n";
        cout << "Enter your option (0-2): ";
        string input;
        getline(cin, input);

        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid input. Please enter (0-2).\n";
            waitForEnter();
            continue;
        }
        int opt = input[0] - '0';

        if (opt == 0) {
            if (confirmAction("Are you sure want to go back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
        }
        else if (opt == 1) {
            if (page > 0) page--;
            else {
                cout << "This is the first page.\n";
                waitForEnter();
            }
        }
        else if (opt == 2) {
            if (page + 1 < totalPages) page++;
            else {
                cout << "This is the last page.\n";
                waitForEnter();
            }
        }
        else {
            cout << "Invalid option. Please enter 0, 1, or 2.\n";
            waitForEnter();
        }
    }
}

void GenerateReceipt(const string& paymentID, const string& bookingID, const string& bikeID, const string& customerName, double amount, const string& method, const string& date, const string& bookingDate, const string& bookingTime, const string& returnTime, const string& pickupStation) {
    for (const auto& p : transactions) {
        if (p.paymentID == paymentID) {
            cout << "\n==============================\n";
            cout << "       RECEIPT\n";
            cout << "==============================\n";
            cout << "Payment ID   : " << paymentID << "\n";
            cout << "Booking ID   : " << bookingID << "\n";
            cout << "Bike ID      : " << bikeID << "\n";
            cout << "Customer     : " << customerName << "\n";
            cout << "Amount Paid  : RM " << fixed << setprecision(2) << amount << "\n";
            cout << "Method       : " << method << "\n";
            cout << "Date         : " << date << "\n";
            cout << "------------------------------\n";
            cout << "Booking Details:\n";
            cout << "  Start at   : " << bookingDate << " " << bookingTime << "\n";
            cout << "  Return by  : " << bookingDate << " " << returnTime << " (scheduled)\n";
            cout << "  Pickup at  : " << pickupStation << "\n";
            cout << "==============================\n";
            cout << "Booking created successfully!\n";
            cout << "Thank you for using Bike 2U!\n";
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

    string svgFilename = baseFilename + ".svg";
    string htmlFilename = baseFilename + ".html";

    generateQRCodeSVG(qrData, svgFilename);
}

void SavePaymentToFile() {
    ofstream out(PAYMENT_FILE);
    if (!out) return;
    out << "PaymentID,BookingID,BikeID,CustomerName,Amount,Method,Date,RefundStatus,RequestDate,RejectReason,RefundID\n";
    for (const auto& p : transactions) {
        out << p.paymentID << "," << p.bookingID << "," << p.bikeID << ","
            << p.customerName << "," << fixed << setprecision(2) << p.paymentAmount << ","
            << p.paymentMethod << "," << p.paymentDate << ","
            << p.refundStatus << "," << p.requestDate << ","
            << p.rejectReason << "," << p.refundID << "\n";
    }
    out.close();
}

void LoadPaymentFromFile() {
    ifstream in(PAYMENT_FILE);
    if (!in) return;
    string line;
    getline(in, line);
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        Payment p;
        getline(ss, token, ','); p.paymentID = trim(token);
        getline(ss, token, ','); p.bookingID = trim(token);
        getline(ss, token, ','); p.bikeID = trim(token);
        getline(ss, token, ','); p.customerName = trim(token);
        getline(ss, token, ','); p.paymentAmount = stod(token);
        getline(ss, token, ','); p.paymentMethod = trim(token);
        getline(ss, token, ','); p.paymentDate = trim(token);
        getline(ss, token, ','); p.refundStatus = trim(token);
        getline(ss, token, ','); p.requestDate = trim(token);
        getline(ss, token, ','); p.rejectReason = trim(token);
        getline(ss, token, ','); p.refundID = trim(token);
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
    cout << indent << "==============================\n";
    cout << indent << "       GENERATE INVOICE\n";
    cout << indent << "==============================\n";

    const Account& user = users[currentIdx];

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

    cout << indent << "Select a booking to generate invoice:\n";
    for (size_t i = 0; i < userBookings.size(); ++i) {
        const Booking& b = *userBookings[i];
        cout << indent << "[" << i + 1 << "] Booking " << b.bookingID
            << " | Bike: " << b.bikeID
            << " | Date: " << b.bookingDate
            << " | Duration: " << b.duration << "h"
            << " | Status: " << b.status << "\n";
    }
    cout << indent << "[0] Back\n";

    int choice = getValidOption(0, static_cast<int>(userBookings.size()), indent);
    if (choice == 0) {
        cout << indent << "Operation cancelled.\n";
        waitForEnter(indent);
        return;
    }

    const Booking& selected = *userBookings[choice - 1];

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

    string invoiceNo = selected.bookingID;
    string baseFilename = "invoice_" + selected.bookingID;

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

// ==================================================================
// ANALYTICS FUNCTIONS (from third file, adapted)
// ==================================================================

void memberTypeAnalytics(const vector<Account>& users) {
    map<string, int> count;
    count["Bronze"] = 0;
    count["Silver"] = 0;
    count["Gold"] = 0;
    count["None"] = 0;

    for (const auto& u : users) {
        if (u.membershipType == "Bronze") count["Bronze"]++;
        else if (u.membershipType == "Silver") count["Silver"]++;
        else if (u.membershipType == "Gold") count["Gold"]++;
        else count["None"]++;
    }

    int total = users.size();
    clearScreen();
    cout << "============================================================\n";
    cout << "                 MEMBER TYPE ANALYTICS\n";
    cout << "============================================================\n\n";
    cout << left << setw(18) << "Member Type" << setw(15) << "Users"
        << setw(15) << "Percentage (%)" << "Level\n";
    cout << string(70, '-') << '\n';

    const string types[] = { "Bronze", "Silver", "Gold", "None" };
    for (const string& type : types) {
        double pct = total == 0 ? 0 : (count[type] * 100.0 / total);
        cout << left << setw(18) << type << setw(15) << count[type]
            << setw(14) << fixed << setprecision(2) << pct;
        if (type == "Bronze") cout << "Basic membership\n";
        else if (type == "Silver") cout << "Regular membership\n";
        else if (type == "Gold") cout << "Premium membership\n";
        else cout << "No membership\n";
    }

    cout << string(70, '-') << '\n';
    cout << left << setw(18) << "Total Members" << setw(15) << total << "100.00%\n";

    cout << "\n---------------- MEMBER SUMMARY ----------------\n";
    if (total > 0) {
        string largest = "None";
        int maxCount = count["None"];
        if (count["Bronze"] > maxCount) { maxCount = count["Bronze"]; largest = "Bronze"; }
        if (count["Silver"] > maxCount) { maxCount = count["Silver"]; largest = "Silver"; }
        if (count["Gold"] > maxCount) { maxCount = count["Gold"]; largest = "Gold"; }
        cout << "Most common member type : " << largest << " (" << maxCount << " users)\n";
    }
    else {
        cout << "Most common member type : None (0 users)\n";
    }
    cout << "Bronze members : " << count["Bronze"] << " users\n";
    cout << "Silver members : " << count["Silver"] << " users\n";
    cout << "Gold members   : " << count["Gold"] << " users\n";
    cout << "Non-members    : " << count["None"] << " users\n";
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

void revenueAnalytics(const vector<Payment>& transactions) {
    double total = 0;
    double highest = -1, lowest = numeric_limits<double>::max();
    int highestID = -1;
    string highestCustomer;

    map<string, double> monthly;
    map<string, pair<int, double>> annual;
    map<string, pair<int, double>> methods;

    // Only process positive payments (revenue)
    for (const auto& p : transactions) {
        if (p.paymentAmount <= 0) continue;   // skip refunds

        total += p.paymentAmount;
        monthly[getMonth(p.paymentDate)] += p.paymentAmount;
        string year = getYear(p.paymentDate);
        annual[year].first++;
        annual[year].second += p.paymentAmount;
        methods[p.paymentMethod].first++;
        methods[p.paymentMethod].second += p.paymentAmount;

        if (p.paymentAmount > highest) {
            highest = p.paymentAmount;
            highestID = stoi(p.paymentID.substr(3));   // assumes "INVXXXX"
            highestCustomer = p.customerName;
        }
        if (p.paymentAmount < lowest) {
            lowest = p.paymentAmount;
        }
    }

    double average = (total == 0) ? 0 : total / (transactions.size() - count_if(transactions.begin(), transactions.end(),
        [](const Payment& p) { return p.paymentAmount <= 0; }));

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

    cout << "\n---------------- MONTHLY REVENUE ----------------\n";
    cout << left << setw(15) << "Month" << setw(18) << "Transactions" << "Revenue\n";
    cout << string(50, '-') << '\n';
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
                if (p.paymentAmount > 0 && getMonth(p.paymentDate) == m) ++cnt;
            }
            cout << left << setw(15) << monthName(m) << setw(18) << cnt << money(monthly[m]) << '\n';
        }
    }

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

    cout << "\n---------------- REVENUE INSIGHTS ----------------\n";
    if (!transactions.empty() && total > 0) {
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
        cout << "No revenue data available (positive payments only).\n";
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

    cout << "\n---------------- BICYCLE TYPE BAR CHART ----------------\n";
    cout << "Each # represents 10 bicycles.\n\n";
    for (const string& t : types) {
        cout << left << setw(11) << t << " | ";
        int numBars = counts[t] / 10;        
        for (int i = 0; i < numBars; ++i) cout << '#';
        cout << "  " << counts[t] << '\n';
    }

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
        cout << "===================================\n";
        cout << "       REPORTING & ANALYTICS       \n ";
        cout << "===================================\n\n";
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
            if (confirmAction("Are you sure want to return back?")) {
                cout << "Returning...\n";
                waitForEnter();
                return;
            }
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
    LoadInventoryFromFile(inventory);
    LoadBookingsFromFile(bookings, BOOKING_FILE);
    SyncInventoryWithBookings(bookings, inventory);
    LoadPaymentFromFile();

    Menu(admin, users, repairs, inventory, bookings);

    SaveInventoryToFile(inventory);
    SaveBookingsToFile(bookings, BOOKING_FILE);
    SavePaymentToFile();

    return 0;
}
