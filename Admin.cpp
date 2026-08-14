#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <limits>
#include <cmath>
using namespace std;



const string DATA_FILE = "admin.csv";
const int PAGE_SIZE = 8;

struct Customer {
    string id;
    string name;
    string memberType; // Bronze, Silver, Gold
    string city;
};

struct Bicycle {
    int id;
    string type;       // City, Electric, Mountain, Road            
    bool available;
};

struct Payment {
    int id;
    string customer;
    double amount;
    string method;
    string date;       // YYYY-MM-DD
};

vector<Customer> customers;
vector<Bicycle> bicycles;
vector<Payment> payments;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[H";
#endif
}

void pauseScreen() {
    string input;
    while (true) {
        cout << "\nPress Enter to continue...";
        getline(cin, input);

        if (input.empty()) {
            break; // Only Enter key pressed - success!
        }
        else {
            cout << "\n[Error] Invalid input! Please press only the ENTER key to continue.\n";
            // Clear any remaining input just in case
            cin.clear();
        }
    }
}

string trim(string s) {
    while (!s.empty() && isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    return s;
}

string money(double value) {
    ostringstream out;
    out << "RM " << fixed << setprecision(2) << value;
    return out.str();
}

bool isInteger(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

int getOption(int minValue, int maxValue) {
    string input;
    getline(cin, input);
    input = trim(input);

    if (!isInteger(input)) {
        cout << "\nInvalid Option. Please select an option from "
            << minValue << " to " << maxValue << ".\n";
        return -1;
    }

    int value = stoi(input);
    if (value < minValue || value > maxValue) {
        cout << "\nInvalid Option. Please select an option from "
            << minValue << " to " << maxValue << ".\n";
        return -1;
    }
    return value;
}

string safeField(string s) {
    replace(s.begin(), s.end(), ',', ' ');
    return s;
}

vector<string> splitCSV(const string& line) {
    vector<string> result;
    string value;
    stringstream ss(line);
    while (getline(ss, value, ',')) result.push_back(trim(value));
    return result;
}

void saveData() {
    ofstream out(DATA_FILE);
    if (!out) {
        cout << "\n[File Error] Unable to save data to admin.csv.\n";
        return;
    }

    out << "CUSTOMER,id,name,memberType,city\n";
    for (const auto& c : customers) {
        out << "CUSTOMER," << safeField(c.id) << ',' << safeField(c.name) << ','
            << safeField(c.memberType) << ',' << safeField(c.city) << '\n';
    }

    out << "BICYCLE,id,type,available\n";
    for (const auto& b : bicycles) {
        out << "BICYCLE," << b.id << ',' << safeField(b.type) << ','
            << (b.available ? "Yes" : "No") << '\n';
    }

    out << "PAYMENT,id,customer,amount,method,date\n";
    for (const auto& p : payments) {
        out << "PAYMENT," << p.id << ',' << safeField(p.customer) << ','
            << fixed << setprecision(2) << p.amount << ','
            << safeField(p.method) << ',' << p.date << '\n';
    }
}

bool isValidPaymentMethod(const string& method) {
    return method == "Debit" ||
        method == "Credit" ||
        method == "TnG eWallet";
}

string normalizePaymentMethod(const string& method) {
    // Convert old sample/payment data to the new three allowed methods.
    if (method == "Credit") return "Credit";
    if (method == "Debit") return "Debit";
    if (method == "TnG eWallet") return "TnG eWallet";
    return method;
}

bool loadData() {
    ifstream in(DATA_FILE);
    if (!in) return false;

    customers.clear();
    bicycles.clear();
    payments.clear();

    string line;
    while (getline(in, line)) {
        vector<string> p = splitCSV(line);
        if (p.empty()) continue;

        try {
            if (p[0] == "CUSTOMER" && p.size() >= 5 && p[1] != "id") {
                customers.push_back({ p[1], p[2], p[3], p[4] });
            }
            else if (p[0] == "BICYCLE" && p.size() >= 4 && p[1] != "id") {
                bicycles.push_back({ stoi(p[1]), p[2], p[3] == "Yes" });
            }
            else if (p[0] == "PAYMENT" && p.size() >= 6 && p[1] != "id") {
                payments.push_back({ stoi(p[1]), p[2], stod(p[3]), normalizePaymentMethod(p[4]), p[5] });
            }
        }
        catch (...) {
            // Ignore invalid rows.
        }
    }

    bool validCustomerData = !customers.empty();
    for (const auto& c : customers) {
        if (c.memberType != "Bronze" && c.memberType != "Silver" && c.memberType != "Gold") {
            validCustomerData = false;
            break;
        }
    }

    bool validBicycleData = !bicycles.empty();
    for (const auto& b : bicycles) {
        if (b.type != "City" && b.type != "Electric" &&
            b.type != "Mountain" && b.type != "Road") {
            validBicycleData = false;
            break;
        }
    }

    bool validPaymentData = !payments.empty();
    for (const auto& p : payments) {
        if (!isValidPaymentMethod(p.method)) {
            validPaymentData = false;
            break;
        }
    }

    return validCustomerData && validBicycleData && validPaymentData;
}


string getMonth(const string& date) {
    if (date.size() >= 7) return date.substr(0, 7);
    return "Unknown";
}

string monthName(const string& month) {
    static const string names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    if (month.size() != 7) return month;
    int m = stoi(month.substr(5, 2));
    if (m >= 1 && m <= 12) return names[m - 1];
    return month;
}

// 1. Member Type Analytics
void memberTypeAnalytics() {
    map<string, int> count;
    count["Bronze"] = 0;
    count["Silver"] = 0;
    count["Gold"] = 0;

    for (const auto& c : customers) {
        if (count.find(c.memberType) != count.end()) count[c.memberType]++;
    }

    int total = count["Bronze"] + count["Silver"] + count["Gold"];

    clearScreen();
    cout << "============================================================\n";
    cout << "                 MEMBER TYPE ANALYTICS\n";
    cout << "============================================================\n\n";

    cout << left << setw(18) << "Member Type"
        << setw(15) << "Users"
        << setw(15) << "Percentage (%)"
        << "Customer Level\n";
    cout << string(70, '-') << '\n';

    const string types[] = { "Bronze", "Silver", "Gold" };
    for (const string& type : types) {
        double share = total == 0 ? 0 : (count[type] * 100.0 / total);
        cout << left << setw(18) << type
            << setw(15) << count[type]
            << setw(14) << fixed << setprecision(2) << share << "%";
        if (type == "Bronze") cout << "Basic membership\n";
        else if (type == "Silver") cout << "Regular membership\n";
        else cout << "Premium membership\n";
    }

    cout << string(70, '-') << '\n';
    cout << left << setw(18) << "Total Members" << setw(15) << total << "100.00%\n";

    cout << "\n---------------- MEMBER SUMMARY ----------------\n";

    // Only show "Most common member type" if there's actual data
    if (total > 0) {
        string largest = "Bronze";
        if (count["Silver"] > count[largest]) largest = "Silver";
        if (count["Gold"] > count[largest]) largest = "Gold";
        cout << "Most common member type : " << largest << " (" << count[largest] << " users)\n";
    }
    else {
        cout << "Most common member type : None (0 users)\n";
    }

    cout << "Bronze members          : " << count["Bronze"] << " users\n";
    cout << "Silver members          : " << count["Silver"] << " users\n";
    cout << "Gold members            : " << count["Gold"] << " users\n";

    pauseScreen();
}

// 2. Bicycle Availability Analytics
void bicycleAvailabilityAnalytics() {
    const string types[] = { "City", "Electric", "Mountain", "Road" };

    clearScreen();
    cout << "====================================================================\n";
    cout << "                 BICYCLE AVAILABILITY ANALYTICS\n";
    cout << "====================================================================\n\n";

    cout << left << setw(16) << "Bicycle Type"
        << setw(12) << "Total"
        << setw(14) << "Available"
        << setw(14) << "Rented"
        << "Availability %\n";
    cout << string(70, '-') << '\n';

    int grandTotal = 0, grandAvailable = 0;

    for (const string& type : types) {
        int total = 0, available = 0;
        for (const auto& b : bicycles) {
            if (b.type == type) {
                ++total;
                if (b.available) ++available;
            }
        }

        int rented = total - available;
        double percent = total == 0 ? 0 : available * 100.0 / total;
        grandTotal += total;
        grandAvailable += available;

        cout << left << setw(16) << type
            << setw(12) << total
            << setw(14) << available
            << setw(14) << rented
            << fixed << setprecision(1) << percent << "%\n";
    }

    cout << string(70, '-') << '\n';
    int grandRented = grandTotal - grandAvailable;
    double grandPercent = grandTotal == 0 ? 0 : grandAvailable * 100.0 / grandTotal;
    cout << left << setw(16) << "TOTAL"
        << setw(12) << grandTotal
        << setw(14) << grandAvailable
        << setw(14) << grandRented
        << fixed << setprecision(1) << grandPercent << "%\n";

    cout << "\n---------------- AVAILABILITY OVERVIEW ----------------\n";
    cout << "Available bicycles : " << grandAvailable << "\n";
    cout << "Currently rented   : " << grandRented << "\n";
    cout << "Bicycle availability : " << fixed << setprecision(1) << grandPercent << "%\n";

    cout << "\n---------------- QUICK VIEW ----------------\n";
    cout << "Type            Available / Total\n";
    cout << "---------------------------------\n";
    for (const string& type : types) {
        int total = 0, available = 0;
        for (const auto& b : bicycles) {
            if (b.type == type) {
                ++total;
                if (b.available) ++available;
            }
        }
        cout << left << setw(16) << type << available << " / " << total << '\n';
    }

    pauseScreen();
}

// 3. Revenue Analytics
void revenueAnalytics() {
    double total = 0;
    double highest = -1;
    double lowest = payments.empty() ? 0 : numeric_limits<double>::max();
    int highestID = -1;
    string highestCustomer;

    map<string, double> monthly;
    map<string, pair<int, double>> methods;

    for (const auto& p : payments) {
        total += p.amount;
        monthly[getMonth(p.date)] += p.amount;
        methods[p.method].first++;
        methods[p.method].second += p.amount;

        if (p.amount > highest) {
            highest = p.amount;
            highestID = p.id;
            highestCustomer = p.customer;
        }
        if (p.amount < lowest) lowest = p.amount;
    }

    double average = payments.empty() ? 0 : total / payments.size();

    clearScreen();
    cout << "====================================================================\n";
    cout << "                    REVENUE ANALYTICS\n";
    cout << "====================================================================\n\n";

    // KPI section
    cout << "+----------------------+----------------------+\n";
    cout << "| Total Revenue        | " << left << setw(20) << money(total) << "|\n";
    cout << "| Total Transactions   | " << left << setw(20) << payments.size() << "|\n";
    cout << "| Average Transaction  | " << left << setw(20) << money(average) << "|\n";
    cout << "| Highest Transaction  | " << left << setw(20) << money(highest < 0 ? 0 : highest) << "|\n";
    cout << "+----------------------+----------------------+\n";

    // Monthly revenue
    cout << "\n---------------- MONTHLY REVENUE ----------------\n";
    cout << left << setw(15) << "Month" << setw(18) << "Transactions" << "Revenue\n";
    cout << string(50, '-') << '\n';

    const string months[] = { "2026-01", "2026-02", "2026-03", "2026-04", "2026-05", "2026-06", "2026-07", "2026-08" };
    for (const string& month : months) {
        int transactions = 0;
        for (const auto& p : payments) if (getMonth(p.date) == month) ++transactions;
        cout << left << setw(15) << monthName(month)
            << setw(18) << transactions
            << money(monthly[month]) << '\n';
    }

    // Payment method analysis
    cout << "\n---------------- PAYMENT METHOD ANALYSIS ----------------\n";
    cout << left << setw(20) << "Payment Method"
        << setw(15) << "Transactions"
        << setw(18) << "Revenue"
        << "Percentage\n";
    cout << string(75, '-') << '\n';

    const string paymentMethods[] = { "Debit", "Credit", "TnG eWallet" };
    for (const string& method : paymentMethods) {
        int transactionCount = methods[method].first;
        double methodRevenue = methods[method].second;
        double share = total == 0 ? 0 : methodRevenue * 100.0 / total;

        cout << left << setw(20) << method
            << setw(15) << transactionCount
            << setw(18) << money(methodRevenue)
            << fixed << setprecision(2) << share << "%\n";
    }

    // Business insights
    cout << "\n---------------- REVENUE INSIGHTS ----------------\n";
    if (!payments.empty()) {
        string bestMonth = months[0];
        for (const string& month : months) {
            if (monthly[month] > monthly[bestMonth]) bestMonth = month;
        }

        string bestMethod;
        double bestMethodRevenue = -1;
        for (const auto& item : methods) {
            if (item.second.second > bestMethodRevenue) {
                bestMethodRevenue = item.second.second;
                bestMethod = item.first;
            }
        }

        cout << "Best revenue month    : " << monthName(bestMonth)
            << " (" << money(monthly[bestMonth]) << ")\n";
        cout << "Top payment method    : " << bestMethod
            << " (" << money(bestMethodRevenue) << ")\n";
        cout << "Highest payment       : " << money(highest)
            << " by " << highestCustomer << " (Payment #" << highestID << ")\n";
        cout << "Average payment       : " << money(average) << "\n";
        cout << "Lowest payment        : " << money(lowest) << "\n";
    }
    else {
        cout << "No payment records are available for analysis.\n";
    }

    pauseScreen();
}

// 4. Bicycle Analysis + ASCII Bar Chart
void bicycleAnalysis() {
    const string types[] = { "City", "Electric", "Mountain", "Road" };
    map<string, int> counts;

    for (const string& type : types) counts[type] = 0;
    for (const auto& b : bicycles) {
        if (counts.find(b.type) != counts.end()) counts[b.type]++;
    }

    int maximum = 0;
    for (const string& type : types) maximum = max(maximum, counts[type]);

    clearScreen();
    cout << "====================================================================\n";
    cout << "                     BICYCLE ANALYSIS\n";
    cout << "====================================================================\n\n";

    cout << left << setw(18) << "Bicycle Type" << setw(12) << "Quantity" << "Percentage\n";
    cout << string(55, '-') << '\n';

    for (const string& type : types) {
        double percentage = bicycles.empty() ? 0 : counts[type] * 100.0 / bicycles.size();
        cout << left << setw(18) << type
            << setw(12) << counts[type]
            << fixed << setprecision(1) << percentage << "%\n";
    }

    cout << string(55, '-') << '\n';
    cout << left << setw(18) << "Total" << setw(12) << bicycles.size() << "100.0%\n";

    // Advanced console bar chart
    cout << "\n---------------- BICYCLE TYPE BAR CHART ----------------\n";
    cout << "Each # represents approximately 1 bicycle.\n\n";

    for (const string& type : types) {
        cout << left << setw(11) << type << " | ";
        int barLength = counts[type];
        for (int i = 0; i < barLength; ++i) cout << '#';
        cout << "  " << counts[type] << '\n';
    }

    cout << "\n---------------- ANALYSIS SUMMARY ----------------\n";

    if (bicycles.size() > 0) {
        string mostCommon = types[0];
        string leastCommon = types[0];

        for (const string& type : types) {
            if (counts[type] > counts[mostCommon]) mostCommon = type;
            if (counts[type] < counts[leastCommon]) leastCommon = type;
        }

        cout << "Largest bicycle category : " << mostCommon << " (" << counts[mostCommon] << " bicycles)\n";
        cout << "Smallest bicycle category: " << leastCommon << " (" << counts[leastCommon] << " bicycles)\n";
    }
    else {
        cout << "Largest bicycle category : None (0 bicycles)\n";
        cout << "Smallest bicycle category: None (0 bicycles)\n";   
    }

    cout << "Total bicycles           : " << bicycles.size() << '\n';

    pauseScreen();
}

void reportingMenu() {
    while (true) {
        clearScreen();
        cout << "====================================================================\n";
        cout << "                     REPORTING & ANALYTICS\n";
        cout << "====================================================================\n\n";

        cout << "[1] Member Type Analytics\n";
        cout << "[2] Bicycle Availability Analytics\n";
        cout << "[3] Revenue Analytics\n";
        cout << "[4] Bicycle Analysis & Bar Chart\n";
        cout << "[0] Exit\n\n";

        cout << "Enter option (0-4): ";
        int op = getOption(0, 4);

        if (op == -1) {
            pauseScreen();
            continue;
        }

        switch (op) {
        case 1: memberTypeAnalytics(); break;
        case 2: bicycleAvailabilityAnalytics(); break;
        case 3: revenueAnalytics(); break;
        case 4: bicycleAnalysis(); break;
        case 0: return;
        }
    }
}

    
int main() {
   

    reportingMenu();
    saveData();

    return 0;
}