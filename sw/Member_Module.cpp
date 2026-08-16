#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cctype>

using namespace std;

// ============================================================
// CUSTOMER PROFILE STRUCT
// ============================================================

struct Customer
{
    string customerID;
    string customerName;
};

// ============================================================
// MEMBERSHIP STRUCT
// ============================================================

struct Membership
{
    string membershipID;
    string customerID;
    string customerName;

    string membershipType;
    int discount;

    string startDate;
    string expiryDate;

    int durationYears;

    string status;
};

// ============================================================
// CURRENT LOGGED-IN CUSTOMER
// ============================================================

// In your complete system, these values should come from
// the customer's existing profile / login information.

Customer currentCustomer =
{
    "C001",
    "John Tan"
};

// ============================================================
// GLOBAL MEMBERSHIP DATA
// ============================================================

Membership membership;

bool membershipExists = false;

// ============================================================
// FUNCTION DECLARATIONS
// ============================================================

void membershipManagement();

void viewMembershipDetails();

void registerMembership();

void renewMembership();

void membershipSummary();

void membershipRenewalSummary(string previousExpiryDate);

string getCurrentDate();

string calculateExpiryDate(string startDate, int years);

string calculateExpiryFromExistingDate(
    string expiryDate,
    int years
);

string getMembershipStatus(string expiryDate);

bool isLeapYear(int year);

int daysInMonth(int month, int year);

string formatDate(
    int day,
    int month,
    int year
);

void clearScreen();

void pauseScreen();

int getIntegerInput(
    string prompt,
    int minValue,
    int maxValue
);

char getYesNoInput(string prompt);

string generateMembershipID();


// ============================================================
// CLEAR SCREEN
// ============================================================

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


// ============================================================
// PAUSE SCREEN
// ============================================================

void pauseScreen()
{
    cout << "\nPress Enter to continue...";
    cin.get();
}


// ============================================================
// INTEGER INPUT VALIDATION
// ============================================================

int getIntegerInput(
    string prompt,
    int minValue,
    int maxValue)
{
    string input;

    while (true)
    {
        cout << prompt;

        getline(cin, input);

        // ----------------------------------------------------
        // Check empty input / spaces
        // ----------------------------------------------------

        size_t start =
            input.find_first_not_of(" \t");

        if (start == string::npos)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue
                << "-"
                << maxValue
                << ")\n\n";

            continue;
        }

        // ----------------------------------------------------
        // Remove trailing spaces
        // ----------------------------------------------------

        size_t end =
            input.find_last_not_of(" \t");

        string trimmedInput =
            input.substr(
                start,
                end - start + 1
            );

        // ----------------------------------------------------
        // Check every character
        // ----------------------------------------------------

        bool valid = true;

        for (char c : trimmedInput)
        {
            if (!isdigit(static_cast<unsigned char>(c)))
            {
                valid = false;
                break;
            }
        }

        // ----------------------------------------------------
        // Invalid characters
        // ----------------------------------------------------

        if (!valid)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue
                << "-"
                << maxValue
                << ")\n\n";

            continue;
        }

        // ----------------------------------------------------
        // Convert to integer
        // ----------------------------------------------------

        int value;

        try
        {
            value = stoi(trimmedInput);
        }
        catch (...)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue
                << "-"
                << maxValue
                << ")\n\n";

            continue;
        }

        // ----------------------------------------------------
        // Check range
        // ----------------------------------------------------

        if (value < minValue || value > maxValue)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue
                << "-"
                << maxValue
                << ")\n\n";

            continue;
        }

        return value;
    }
}


// ============================================================
// YES / NO INPUT VALIDATION
// ============================================================

char getYesNoInput(string prompt)
{
    string input;

    while (true)
    {
        cout << prompt;

        getline(cin, input);

        // ----------------------------------------------------
        // Remove leading spaces
        // ----------------------------------------------------

        size_t start =
            input.find_first_not_of(" \t");

        if (start == string::npos)
        {
            cout << "Invalid option. Please enter Y or N: (Y/N)\n\n";
            continue;
        }

        // ----------------------------------------------------
        // Remove trailing spaces
        // ----------------------------------------------------

        size_t end =
            input.find_last_not_of(" \t");

        string trimmedInput =
            input.substr(
                start,
                end - start + 1
            );

        // ----------------------------------------------------
        // Only one character allowed
        // ----------------------------------------------------

        if (trimmedInput.length() != 1)
        {
            cout << "Invalid option. Please enter Y or N: (Y/N)\n\n";
            continue;
        }

        char choice =
            toupper(
                static_cast<unsigned char>(
                    trimmedInput[0]
                    )
            );

        // ----------------------------------------------------
        // Accept Y / N
        // ----------------------------------------------------

        if (choice == 'Y' || choice == 'N')
        {
            return choice;
        }

        cout << "Invalid option. Please enter Y or N: (Y/N)\n\n";
    }
}


// ============================================================
// GET CURRENT DATE
// ============================================================

string getCurrentDate()
{
    time_t now = time(nullptr);

    tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localTime = *localtime(&now);
#endif

    int day =
        localTime.tm_mday;

    int month =
        localTime.tm_mon + 1;

    int year =
        localTime.tm_year + 1900;

    return formatDate(
        day,
        month,
        year
    );
}


// ============================================================
// FORMAT DATE
// ============================================================

string formatDate(
    int day,
    int month,
    int year)
{
    string date;

    date += (day < 10 ? "0" : "");
    date += to_string(day);

    date += "/";

    date += (month < 10 ? "0" : "");
    date += to_string(month);

    date += "/";

    date += to_string(year);

    return date;
}


// ============================================================
// CHECK LEAP YEAR
// ============================================================

bool isLeapYear(int year)
{
    if (year % 400 == 0)
        return true;

    if (year % 100 == 0)
        return false;

    return year % 4 == 0;
}


// ============================================================
// DAYS IN MONTH
// ============================================================

int daysInMonth(
    int month,
    int year)
{
    switch (month)
    {
    case 1:
        return 31;

    case 2:
        return isLeapYear(year) ? 29 : 28;

    case 3:
        return 31;

    case 4:
        return 30;

    case 5:
        return 31;

    case 6:
        return 30;

    case 7:
        return 31;

    case 8:
        return 31;

    case 9:
        return 30;

    case 10:
        return 31;

    case 11:
        return 30;

    case 12:
        return 31;

    default:
        return 30;
    }
}


// ============================================================
// CALCULATE EXPIRY DATE
// ============================================================

string calculateExpiryDate(
    string startDate,
    int years)
{
    int day;
    int month;
    int year;

    day =
        stoi(startDate.substr(0, 2));

    month =
        stoi(startDate.substr(3, 2));

    year =
        stoi(startDate.substr(6, 4));

    int newYear =
        year + years;

    // Handle 29 February
    if (month == 2 &&
        day == 29 &&
        !isLeapYear(newYear))
    {
        day = 28;
    }

    return formatDate(
        day,
        month,
        newYear
    );
}


// ============================================================
// CALCULATE EXPIRY FROM EXISTING EXPIRY
// ============================================================

string calculateExpiryFromExistingDate(
    string expiryDate,
    int years)
{
    return calculateExpiryDate(
        expiryDate,
        years
    );
}


// ============================================================
// GET MEMBERSHIP STATUS
// ============================================================

string getMembershipStatus(
    string expiryDate)
{
    string currentDate =
        getCurrentDate();

    if (currentDate <= expiryDate)
    {
        return "Active";
    }
    else
    {
        return "Expired";
    }
}


// ============================================================
// GENERATE MEMBERSHIP ID
// ============================================================

string generateMembershipID()
{
    return "M001";
}


// ============================================================
// MEMBERSHIP MANAGEMENT MENU
// ============================================================

void membershipManagement()
{
    int option;

    do
    {
        clearScreen();

        cout << "====================================\n";
        cout << "        MEMBERSHIP MANAGEMENT\n";
        cout << "====================================\n";
        cout << "[1] View Membership Details\n";
        cout << "[2] Register Membership\n";
        cout << "[3] Renew Membership (Annual)\n";
        cout << "[0] Return\n";
        cout << "====================================\n";

        option =
            getIntegerInput(
                "Enter option (0-3): ",
                0,
                3
            );

        switch (option)
        {
        case 1:
            viewMembershipDetails();
            break;

        case 2:
            registerMembership();
            break;

        case 3:
            renewMembership();
            break;

        case 0:
            break;
        }

    } while (option != 0);
}


// ============================================================
// VIEW MEMBERSHIP DETAILS
// ============================================================

void viewMembershipDetails()
{
    clearScreen();

    cout << "========================================\n";
    cout << "        MEMBERSHIP DETAILS\n";
    cout << "========================================\n";

    if (!membershipExists)
    {
        cout << "No membership record found.\n";
        cout << "Please register a membership first.\n";

        cout << "========================================\n";

        pauseScreen();

        return;
    }

    membership.status =
        getMembershipStatus(
            membership.expiryDate
        );

    cout << "Membership ID : "
        << membership.membershipID
        << "\n";

    cout << "Customer ID   : "
        << membership.customerID
        << "\n";

    cout << "Name          : "
        << membership.customerName
        << "\n";

    cout << "Membership    : "
        << membership.membershipType
        << "\n";

    cout << "Discount      : "
        << membership.discount
        << "%\n";

    cout << "Start Date    : "
        << membership.startDate
        << "\n";

    cout << "Expiry Date   : "
        << membership.expiryDate
        << "\n";

    cout << "Status        : "
        << membership.status
        << "\n";

    cout << "========================================\n";

    pauseScreen();
}


// ============================================================
// REGISTER MEMBERSHIP
// ============================================================

void registerMembership()
{
    clearScreen();

    cout << "========================================\n";
    cout << "        REGISTER MEMBERSHIP\n";
    cout << "========================================\n\n";

    // ========================================================
    // CHECK EXISTING MEMBERSHIP
    // ========================================================

    if (membershipExists)
    {
        membership.status =
            getMembershipStatus(
                membership.expiryDate
            );

        if (membership.status == "Active")
        {
            cout << "An active membership already exists.\n";
            cout << "Please renew the existing membership instead.\n";

            pauseScreen();

            return;
        }
    }

    // ========================================================
    // RETRIEVE CUSTOMER INFORMATION FROM PROFILE
    // ========================================================

    string customerID =
        currentCustomer.customerID;

    string customerName =
        currentCustomer.customerName;

    cout << "Customer Information\n";
    cout << "----------------------------------------\n";

    cout << "Customer ID   : "
        << customerID
        << "\n";

    cout << "Name          : "
        << customerName
        << "\n";

    cout << "----------------------------------------\n";

    cout << "\nCustomer information retrieved from profile.\n";

    // ========================================================
    // SELECT MEMBERSHIP TYPE
    // ========================================================

    cout << "\nMembership Types:\n";
    cout << "[1] Bronze - 2% Discount\n";
    cout << "[2] Silver - 4% Discount\n";
    cout << "[3] Gold   - 6% Discount\n";
    cout << "[0] Return\n\n";

    int membershipChoice =
        getIntegerInput(
            "Select Membership Type (0-3): ",
            0,
            3
        );

    if (membershipChoice == 0)
    {
        cout << "\nRegistration cancelled.\n";

        pauseScreen();

        return;
    }

    string selectedType;
    int selectedDiscount;

    switch (membershipChoice)
    {
    case 1:
        selectedType = "Bronze";
        selectedDiscount = 2;
        break;

    case 2:
        selectedType = "Silver";
        selectedDiscount = 4;
        break;

    case 3:
        selectedType = "Gold";
        selectedDiscount = 6;
        break;
    }

    // ========================================================
    // SELECT MEMBERSHIP DURATION
    // ========================================================

    cout << "\nMembership Duration:\n";
    cout << "[1] 1 Year\n";
    cout << "[2] 2 Years\n";
    cout << "[3] 3 Years\n";
    cout << "[0] Return\n\n";

    int durationChoice =
        getIntegerInput(
            "Select Duration (0-3): ",
            0,
            3
        );

    if (durationChoice == 0)
    {
        cout << "\nRegistration cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // PREPARE TEMPORARY REGISTRATION DATA
    // ========================================================

    string membershipID =
        generateMembershipID();

    string startDate =
        getCurrentDate();

    string expiryDate =
        calculateExpiryDate(
            startDate,
            durationChoice
        );

    // ========================================================
    // FINAL CONFIRMATION
    // ========================================================

    clearScreen();

    cout << "========================================\n";
    cout << "       CONFIRM REGISTRATION\n";
    cout << "========================================\n\n";

    cout << "Membership ID : "
        << membershipID
        << "\n";

    cout << "Customer ID   : "
        << customerID
        << "\n";

    cout << "Name          : "
        << customerName
        << "\n";

    cout << "Membership    : "
        << selectedType
        << "\n";

    cout << "Discount      : "
        << selectedDiscount
        << "%\n";

    cout << "Duration      : "
        << durationChoice
        << " Year";

    if (durationChoice > 1)
        cout << "s";

    cout << "\n";

    cout << "Start Date    : "
        << startDate
        << "\n";

    cout << "Expiry Date   : "
        << expiryDate
        << "\n";

    cout << "\n========================================\n";

    char confirmation =
        getYesNoInput(
            "Do you want to register this membership? (Y/N): "
        );

    // ========================================================
    // CANCEL REGISTRATION
    // ========================================================

    if (confirmation == 'N')
    {
        cout << "\nMembership registration cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // SAVE MEMBERSHIP ONLY AFTER CONFIRMATION
    // ========================================================

    membership.membershipID =
        membershipID;

    membership.customerID =
        customerID;

    membership.customerName =
        customerName;

    membership.membershipType =
        selectedType;

    membership.discount =
        selectedDiscount;

    membership.durationYears =
        durationChoice;

    membership.startDate =
        startDate;

    membership.expiryDate =
        expiryDate;

    membership.status =
        getMembershipStatus(
            membership.expiryDate
        );

    membershipExists = true;

    // ========================================================
    // SHOW SUCCESS SUMMARY
    // ========================================================

    membershipSummary();
}


// ============================================================
// MEMBERSHIP SUMMARY
// ============================================================

void membershipSummary()
{
    clearScreen();

    cout << "========================================\n";
    cout << "      MEMBERSHIP SUMMARY\n";
    cout << "========================================\n\n";

    cout << "Membership ID : "
        << membership.membershipID
        << "\n";

    cout << "Customer ID   : "
        << membership.customerID
        << "\n";

    cout << "Name          : "
        << membership.customerName
        << "\n";

    cout << "Membership    : "
        << membership.membershipType
        << "\n";

    cout << "Discount      : "
        << membership.discount
        << "%\n";

    cout << "Duration      : "
        << membership.durationYears
        << " Year";

    if (membership.durationYears > 1)
        cout << "s";

    cout << "\n";

    cout << "Start Date    : "
        << membership.startDate
        << "\n";

    cout << "Expiry Date   : "
        << membership.expiryDate
        << "\n";

    cout << "Status        : "
        << membership.status
        << "\n\n";

    cout << "========================================\n";
    cout << "Membership registered successfully!\n";
    cout << "========================================\n";

    pauseScreen();
}


// ============================================================
// RENEW MEMBERSHIP
// ============================================================

void renewMembership()
{
    clearScreen();

    cout << "========================================\n";
    cout << "        RENEW MEMBERSHIP\n";
    cout << "========================================\n";

    // ========================================================
    // CHECK MEMBERSHIP
    // ========================================================

    if (!membershipExists)
    {
        cout << "\nNo membership record found.\n";
        cout << "Please register a membership first.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // UPDATE CURRENT STATUS
    // ========================================================

    membership.status =
        getMembershipStatus(
            membership.expiryDate
        );

    // ========================================================
    // DISPLAY CURRENT MEMBERSHIP
    // ========================================================

    cout << "\nMembership ID : "
        << membership.membershipID
        << "\n";

    cout << "Customer ID   : "
        << membership.customerID
        << "\n";

    cout << "Name          : "
        << membership.customerName
        << "\n";

    cout << "Membership    : "
        << membership.membershipType
        << "\n";

    cout << "Current Expiry: "
        << membership.expiryDate
        << "\n";

    cout << "Status        : "
        << membership.status
        << "\n";

    // ========================================================
    // SELECT RENEWAL DURATION
    // ========================================================

    cout << "\nRenewal Duration:\n";
    cout << "[1] 1 Year\n";
    cout << "[2] 2 Years\n";
    cout << "[3] 3 Years\n";
    cout << "[0] Return\n\n";

    int durationChoice =
        getIntegerInput(
            "Select Duration (0-3): ",
            0,
            3
        );

    if (durationChoice == 0)
    {
        cout << "\nMembership renewal cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // PREPARE TEMPORARY RENEWAL DATA
    // ========================================================

    string previousExpiryDate =
        membership.expiryDate;

    string newExpiryDate =
        calculateExpiryFromExistingDate(
            previousExpiryDate,
            durationChoice
        );

    // ========================================================
    // FINAL RENEWAL CONFIRMATION
    // ========================================================

    clearScreen();

    cout << "========================================\n";
    cout << "        CONFIRM RENEWAL\n";
    cout << "========================================\n\n";

    cout << "Membership ID : "
        << membership.membershipID
        << "\n";

    cout << "Customer ID   : "
        << membership.customerID
        << "\n";

    cout << "Name          : "
        << membership.customerName
        << "\n";

    cout << "Membership    : "
        << membership.membershipType
        << "\n";

    cout << "Renewal       : "
        << durationChoice
        << " Year";

    if (durationChoice > 1)
        cout << "s";

    cout << "\n";

    cout << "Previous Exp. : "
        << previousExpiryDate
        << "\n";

    cout << "New Expiry    : "
        << newExpiryDate
        << "\n";

    cout << "\n========================================\n";

    char confirmation =
        getYesNoInput(
            "Do you want to renew this membership? (Y/N): "
        );

    // ========================================================
    // CANCEL RENEWAL
    // ========================================================

    if (confirmation == 'N')
    {
        cout << "\nMembership renewal cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // SAVE RENEWAL ONLY AFTER CONFIRMATION
    // ========================================================

    membership.durationYears =
        durationChoice;

    membership.expiryDate =
        newExpiryDate;

    membership.status =
        getMembershipStatus(
            membership.expiryDate
        );

    // ========================================================
    // SHOW RENEWAL SUMMARY
    // ========================================================

    membershipRenewalSummary(
        previousExpiryDate
    );
}


// ============================================================
// MEMBERSHIP RENEWAL SUMMARY
// ============================================================

void membershipRenewalSummary(
    string previousExpiryDate)
{
    clearScreen();

    cout << "========================================\n";
    cout << "       MEMBERSHIP RENEWAL\n";
    cout << "========================================\n\n";

    cout << "Membership ID : "
        << membership.membershipID
        << "\n";

    cout << "Customer ID   : "
        << membership.customerID
        << "\n";

    cout << "Name          : "
        << membership.customerName
        << "\n";

    cout << "Membership    : "
        << membership.membershipType
        << "\n";

    cout << "Renewal       : "
        << membership.durationYears
        << " Year";

    if (membership.durationYears > 1)
        cout << "s";

    cout << "\n";

    cout << "Previous Exp. : "
        << previousExpiryDate
        << "\n";    

    cout << "New Expiry    : "
        << membership.expiryDate
        << "\n";

    cout << "Status        : "
        << membership.status
        << "\n\n";

    cout << "========================================\n";
    cout << "Membership renewed successfully!\n";
    cout << "========================================\n";

    pauseScreen();
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    membershipManagement();

    return 0;
}