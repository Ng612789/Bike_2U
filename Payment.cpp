#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <cctype>

using namespace std;

// Constants
const double OVERTIME_RATE = 5.50;
const double TAX_RATE = 0.06;

// Payment structure
struct Payment {
    int paymentID;
    int bookingID;
    double paymentAmount;
    string paymentMethod;
    string paymentDate;
};

// ------------------------------------------------------------
// PaymentManager class – encapsulates all data and operations
// ------------------------------------------------------------
class PaymentManager {
private:
    vector<Payment> transactions;
    int nextPaymentID;

    // Helper: get current date as string YYYY-MM-DD (Windows safe)
    string getCurrentDate() {
        time_t now = time(NULL);
        tm local_tm;
        localtime_s(&local_tm, &now);   // Visual Studio safe version
        char dateBuf[11];
        strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", &local_tm);
        return string(dateBuf);
    }

    // Helper: validate card number (16 digits)
    bool isValidCardNumber(const string& cardNum) {
        if (cardNum.length() != 16) return false;
        for (char c : cardNum)
            if (!isdigit(c)) return false;
        return true;
    }

    // Helper: validate CVV (3 digits)
    bool isValidCVV(const string& cvv) {
        if (cvv.length() != 3) return false;
        for (char c : cvv)
            if (!isdigit(c)) return false;
        return true;
    }

    // Helper: calculate rental fee
    double calculateRentalFee(double hours, double baseRate) {
        return hours * baseRate;
    }

    // Helper: calculate overtime fee
    double calculateOvertimeFee(double overtimeHours) {
        return overtimeHours * OVERTIME_RATE;
    }

    // Internal receipt generation
    void generateReceipt(const Payment& p, const string& customerName) {
        cout << "\n========== RECEIPT ==========\n";
        cout << "Payment ID   : " << p.paymentID << "\n";
        cout << "Booking ID   : " << p.bookingID << "\n";
        cout << "Customer     : " << customerName << "\n";
        cout << "Amount Paid  : RM " << fixed << setprecision(2) << p.paymentAmount << "\n";
        cout << "Method       : " << p.paymentMethod << "\n";
        cout << "Date         : " << p.paymentDate << "\n";
        cout << "Thank you for using Bike 2U!\n";
        cout << "===============================\n";
    }

public:
    // Constructor
    PaymentManager() : nextPaymentID(1) {}

    // Load records from CSV
    void loadFromFile() {
        ifstream in("payment.csv");
        if (!in) return;
        string line;
        getline(in, line); // skip header
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string token;
            Payment p;
            getline(ss, token, ','); p.paymentID = atoi(token.c_str());
            getline(ss, token, ','); p.bookingID = atoi(token.c_str());
            getline(ss, token, ','); p.paymentAmount = atof(token.c_str());
            getline(ss, token, ','); p.paymentMethod = token;
            getline(ss, token, ','); p.paymentDate = token;
            transactions.push_back(p);
            if (p.paymentID >= nextPaymentID)
                nextPaymentID = p.paymentID + 1;
        }
        in.close();
    }

    // Save records to CSV
    void saveToFile() {
        ofstream out("payment.csv");
        if (!out) return;
        out << "PaymentID,BookingID,Amount,Method,Date\n";
        for (const auto& p : transactions) {
            out << p.paymentID << ","
                << p.bookingID << ","
                << fixed << setprecision(2) << p.paymentAmount << ","
                << p.paymentMethod << ","
                << p.paymentDate << "\n";
        }
        out.close();
    }

    // Process a new payment
    void processPayment(int bookingID, double rentalHours, double baseRate, const string& customerName) {
        double baseFee = calculateRentalFee(rentalHours, baseRate);
        double overtimeFee = 0.0;
        if (rentalHours > 24.0)
            overtimeFee = calculateOvertimeFee(rentalHours - 24.0);

        double subtotal = baseFee + overtimeFee;
        double tax = subtotal * TAX_RATE;
        double total = subtotal + tax;

        cout << fixed << setprecision(2);
        cout << "\n--- Payment Summary ---\n";
        cout << "Base Fee: RM " << baseFee << "\n";
        cout << "Overtime: RM " << overtimeFee << "\n";
        cout << "Subtotal: RM " << subtotal << "\n";
        cout << "Tax (6%): RM " << tax << "\n";
        cout << "Total:    RM " << total << "\n";

        double paid;
        do {
            cout << "Enter payment amount (RM): ";
            cin >> paid;
            if (cin.fail() || paid < total) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Insufficient. Must be at least RM " << total << "\n";
            }
            else break;
        } while (true);

        int methodChoice;
        string method;
        cout << "Payment method:\n1. Debit Card\n2. Credit Card\n3. Touch'n Go\nChoice: ";
        cin >> methodChoice;
        switch (methodChoice) {
        case 1: method = "Debit Card"; break;
        case 2: method = "Credit Card"; break;
        case 3: method = "Touch'n Go"; break;
        default: method = "Unknown";
        }

        // Validate card details for card payments
        if (methodChoice == 1 || methodChoice == 2) {
            string cardNum, expiry, cvv;
            bool ok = false;
            do {
                cout << "Card number (16 digits): "; cin >> cardNum;
                if (!isValidCardNumber(cardNum)) { cout << "Invalid.\n"; continue; }
                cout << "Expiry (MM/YY): "; cin >> expiry;
                if (expiry.length() != 5 || expiry[2] != '/') { cout << "Invalid format.\n"; continue; }
                cout << "CVV (3 digits): "; cin >> cvv;
                if (!isValidCVV(cvv)) { cout << "Invalid CVV.\n"; continue; }
                ok = true;
            } while (!ok);
            cout << "Card verified.\n";
        }
        else if (methodChoice == 3) {
            cout << "Touch'n Go selected.\n";
        }

        // Create and store payment
        Payment newP;
        newP.paymentID = nextPaymentID++;
        newP.bookingID = bookingID;
        newP.paymentAmount = paid;
        newP.paymentMethod = method;
        newP.paymentDate = getCurrentDate();

        transactions.push_back(newP);
        saveToFile();  // persist immediately

        cout << "\nPayment successful! Payment ID: " << newP.paymentID << "\n";
        generateReceipt(newP, customerName);
    }

    // Display all payment history
    void displayHistory() {
        if (transactions.empty()) {
            cout << "No records.\n";
            return;
        }
        cout << "\n========== PAYMENT HISTORY ==========\n";
        cout << left << setw(10) << "ID"
            << setw(12) << "Booking"
            << setw(12) << "Amount"
            << setw(18) << "Method"
            << setw(12) << "Date" << "\n";
        cout << string(64, '-') << "\n";
        for (const auto& p : transactions) {
            cout << left << setw(10) << p.paymentID
                << setw(12) << p.bookingID
                << setw(12) << fixed << setprecision(2) << p.paymentAmount
                << setw(18) << p.paymentMethod
                << setw(12) << p.paymentDate << "\n";
        }
    }

    // Generate receipt for a given payment ID (public)
    void generateReceiptByID(int paymentID, const string& customerName) {
        for (const auto& p : transactions) {
            if (p.paymentID == paymentID) {
                generateReceipt(p, customerName);
                return;
            }
        }
        cout << "Payment ID not found.\n";
    }

    // Display the interactive menu
    void displayMenu() {
        int choice;
        do {
            cout << "\n========== BILLING & PAYMENT MENU ==========\n";
            cout << "1. Process Payment\n";
            cout << "2. View Payment History\n";
            cout << "3. Generate Receipt (by Payment ID)\n";
            cout << "4. Return to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (cin.fail() || choice < 1 || choice > 4) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice.\n";
                continue;
            }

            switch (choice) {
            case 1: {
                int bid; double hours, rate; string name;
                cout << "Booking ID: "; cin >> bid;
                cout << "Rental hours: "; cin >> hours;
                cout << "Base rate (RM/hour): "; cin >> rate;
                cout << "Customer name: "; cin.ignore(); getline(cin, name);
                processPayment(bid, hours, rate, name);
                break;
            }
            case 2:
                displayHistory();
                break;
            case 3: {
                int pid;
                string name;
                cout << "Payment ID: "; cin >> pid;
                cout << "Customer name: "; cin.ignore(); getline(cin, name);
                generateReceiptByID(pid, name);
                break;
            }
            case 4:
                cout << "Returning...\n";
                break;
            }
        } while (choice != 4);
    }
};

// ------------------------------------------------------------
// Main program
// ------------------------------------------------------------
int main() {
    PaymentManager pm;
    pm.loadFromFile();

    cout << "=== Bike 2U Billing and Payment Module ===\n";
    pm.displayMenu();

    pm.saveToFile();
    cout << "Data saved. Goodbye!\n";
    return 0;
}