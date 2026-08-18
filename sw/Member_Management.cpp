#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <conio.h>

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

    double annualFee;

    string startDate;
    string expiryDate;

    int durationYears;

    string status;
};

// ============================================================
// CURRENT LOGGED-IN CUSTOMER
// ============================================================

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

string getCurrentDate();

string calculateExpiryDate(
    string startDate,
    int years
);

string calculateExpiryFromExistingDate(
    string expiryDate,
    int years
);

string getMembershipStatus(
    string expiryDate
);

bool isLeapYear(int year);

int daysInMonth(
    int month,
    int year
);

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
// PAYMENT FUNCTIONS
// ============================================================

string getCardNumber();

bool validateCardNumber(string cardNumber);

bool validateCCV(string ccv);

bool validateExpiryDate(string expiryDate);

bool validateTngPhone(string phoneNumber);

bool processPayment(double amount);

void displayReceipt(
    string membershipType,
    double annualFee,
    int years,
    double totalAmount
);

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

        size_t start =
            input.find_first_not_of(" \t");

        if (start == string::npos)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue << "-"
                << maxValue << ")\n\n";

            continue;
        }

        size_t end =
            input.find_last_not_of(" \t");

        string trimmedInput =
            input.substr(
                start,
                end - start + 1
            );

        bool valid = true;

        for (char c : trimmedInput)
        {
            if (!isdigit(static_cast<unsigned char>(c)))
            {
                valid = false;
                break;
            }
        }

        if (!valid)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue << "-"
                << maxValue << ")\n\n";

            continue;
        }

        int value;

        try
        {
            value = stoi(trimmedInput);
        }
        catch (...)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue << "-"
                << maxValue << ")\n\n";

            continue;
        }

        if (value < minValue || value > maxValue)
        {
            cout << "Invalid option. Please enter a number: ("
                << minValue << "-"
                << maxValue << ")\n\n";

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

        size_t start =
            input.find_first_not_of(" \t");

        if (start == string::npos)
        {
            cout << "Invalid option. Please enter Y or N: (Y/N)\n\n";
            continue;
        }

        size_t end =
            input.find_last_not_of(" \t");

        string trimmedInput =
            input.substr(
                start,
                end - start + 1
            );

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
// CARD NUMBER INPUT WITH AUTO SPACING
// ============================================================

string getCardNumber()
{
    string cardNumber;
    char ch;

    cout << "Enter 16-digit card number: ";

    while (true)
    {
        ch = _getch();

        // ----------------------------------------------------
        // ENTER
        // ----------------------------------------------------

        if (ch == 13)
        {
            cout << endl;
            break;
        }

        // ----------------------------------------------------
        // BACKSPACE
        // ----------------------------------------------------

        if (ch == 8)
        {
            if (!cardNumber.empty())
            {
                // Remove last character
                cardNumber.pop_back();

                // If last character is an automatic space,
                // remove it as well
                if (!cardNumber.empty() &&
                    cardNumber.back() == ' ')
                {
                    cardNumber.pop_back();
                }

                // Clear the current line
                cout << "\r";
                cout << "Enter card number: ";

                // Re-display card number
                cout << cardNumber;
                cout << " ";

                // Clear any remaining characters
                cout << "\r";
                cout << "Enter card number: "
                    << cardNumber;
            }

            continue;
        }

        // ----------------------------------------------------
        // ONLY ACCEPT NUMBERS
        // ----------------------------------------------------

        if (isdigit(static_cast<unsigned char>(ch)))
        {
            int digitCount = 0;

            for (char c : cardNumber)
            {
                if (isdigit(static_cast<unsigned char>(c)))
                {
                    digitCount++;
                }
            }

            // Maximum 16 digits
            if (digitCount >= 16)
            {
                continue;
            }

            cardNumber += ch;

            cout << ch;

            // Automatically add a space
            // after every 4 digits
            if (digitCount % 4 == 3 &&
                digitCount != 15)
            {
                cardNumber += ' ';
                cout << ' ';
            }
        }
    }

    return cardNumber;
}

// ============================================================
// VALIDATE CARD NUMBER
// ============================================================

bool validateCardNumber(string cardNumber)
{
    string digits;

    // Remove spaces
    for (char c : cardNumber)
    {
        if (c != ' ')
        {
            digits += c;
        }
    }

    // Exactly 16 digits
    if (digits.length() != 16)
    {
        return false;
    }

    // Every character must be a digit
    for (char c : digits)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }

    return true;
}

// ============================================================
// VALIDATE CCV
// ============================================================

bool validateCCV(string ccv)
{
    if (ccv.length() != 3)
    {
        return false;
    }

    for (char c : ccv)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }

    return true;
}

// ============================================================
// VALIDATE CARD EXPIRY DATE
// Format: MM/YY
// ============================================================

bool validateExpiryDate(string expiryDate)
{
    if (expiryDate.length() != 5)
    {
        return false;
    }

    if (expiryDate[2] != '/')
    {
        return false;
    }

    if (!isdigit(expiryDate[0]) ||
        !isdigit(expiryDate[1]) ||
        !isdigit(expiryDate[3]) ||
        !isdigit(expiryDate[4]))
    {
        return false;
    }

    int month =
        stoi(expiryDate.substr(0, 2));

    int year =
        stoi(expiryDate.substr(3, 2));

    if (month < 1 || month > 12)
    {
        return false;
    }

    // Get current date
    time_t now = time(nullptr);

    tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localTime = *localtime(&now);
#endif

    int currentMonth =
        localTime.tm_mon + 1;

    int currentYear =
        (localTime.tm_year + 1900) % 100;

    // Expired year
    if (year < currentYear)
    {
        return false;
    }

    // Same year but expired month
    if (year == currentYear &&
        month < currentMonth)
    {
        return false;
    }

    return true;
}

// ============================================================
// VALIDATE TNG EWALLET PHONE NUMBER
// ============================================================

bool validateTngPhone(string phoneNumber)
{
    // Malaysian mobile number:
    // 0123456789
    // 01123456789

    if (phoneNumber.length() < 10 ||
        phoneNumber.length() > 11)
    {
        return false;
    }

    for (char c : phoneNumber)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }

    // Must start with 01
    if (phoneNumber[0] != '0' ||
        phoneNumber[1] != '1')
    {
        return false;
    }

    return true;
}

// ============================================================
// PROCESS PAYMENT
// ============================================================

bool processPayment(double amount)
{
    clearScreen();

    cout << "========================================\n";
    cout << "           PAYMENT METHOD\n";
    cout << "========================================\n\n";

    cout << fixed << setprecision(2);

    cout << "Amount to Pay : RM "
        << amount
        << "\n\n";

    cout << "[1] Debit Card\n";
    cout << "[2] Credit Card\n";
    cout << "[3] TnG eWallet\n";
    cout << "[0] Cancel Payment\n\n";

    int paymentChoice =
        getIntegerInput(
            "Select Payment Method (0-3): ",
            0,
            3
        );

    // ========================================================
    // CANCEL
    // ========================================================

    if (paymentChoice == 0)
    {
        cout << "\nPayment cancelled.\n";

        pauseScreen();

        return false;
    }

    // ========================================================
    // DEBIT / CREDIT CARD
    // ========================================================

    if (paymentChoice == 1 ||
        paymentChoice == 2)
    {
        string cardNumber;
        string ccv;
        string expiryDate;

        cout << "\n----------------------------------------\n";

        if (paymentChoice == 1)
        {
            cout << "          DEBIT CARD PAYMENT\n";
        }
        else
        {
            cout << "          CREDIT CARD PAYMENT\n";
        }

        cout << "----------------------------------------\n";

        // ----------------------------------------------------
        // CARD NUMBER
        // ----------------------------------------------------

        while (true)
        {
            cardNumber = getCardNumber();

            if (validateCardNumber(cardNumber))
            {
                break;
            }

            cout << "\nInvalid card number.\n";
            cout << "Card number must contain exactly 16 digits.\n\n";
        }

        // ----------------------------------------------------
        // CCV
        // ----------------------------------------------------

        while (true)
        {
            cout << "Enter 3-digit CCV: ";
            getline(cin, ccv);

            if (validateCCV(ccv))
            {
                break;
            }

            cout << "Invalid CCV.\n";
            cout << "CCV must contain exactly 3 digits.\n";
        }

        // ----------------------------------------------------
        // EXPIRY DATE
        // ----------------------------------------------------

        while (true)
        {
            cout << "Enter expiry date (MM/YY): ";
            getline(cin, expiryDate);

            if (validateExpiryDate(expiryDate))
            {
                break;
            }

            cout << "Invalid or expired card date.\n";
            cout << "Please enter a valid MM/YY expiry date.\n";
        }

        cout << "\nCard details validated successfully.\n";
        cout << "Processing payment...\n";

        cout << "\n========================================\n";
        cout << "             PAYMENT SUCCESS\n";
        cout << "========================================\n";

        cout << "Payment Method : ";

        if (paymentChoice == 1)
        {
            cout << "Debit Card\n";
        }
        else
        {
            cout << "Credit Card\n";
        }

        cout << "Amount Paid    : RM "
            << fixed
            << setprecision(2)
            << amount
            << "\n";

        cout << "Status         : Successful\n";

        cout << "========================================\n";

        pauseScreen();

        return true;
    }

    // ========================================================
    // TNG EWALLET
    // ========================================================

    if (paymentChoice == 3)
    {
        string phoneNumber;

        cout << "\n----------------------------------------\n";
        cout << "          TNG EWALLET PAYMENT\n";
        cout << "----------------------------------------\n";

        while (true)
        {
            cout << "\nEnter TnG eWallet phone number: ";
            getline(cin, phoneNumber);

            if (validateTngPhone(phoneNumber))
            {
                break;
            }

            cout << "Invalid phone number.\n";
            cout << "Please enter a valid Malaysian mobile number.\n";
        }

        cout << "\nPhone number validated successfully.\n";
        cout << "Processing payment...\n";

        cout << "\n========================================\n";
        cout << "             PAYMENT SUCCESS\n";
        cout << "========================================\n";

        cout << "Payment Method : TnG eWallet\n";

        cout << "Amount Paid    : RM "
            << fixed
            << setprecision(2)
            << amount
            << "\n";

        cout << "Status         : Successful\n";

        cout << "========================================\n";

        pauseScreen();

        return true;
    }

    return false;
}

// ============================================================
// DISPLAY RECEIPT
// ============================================================

void displayReceipt(
    string membershipType,
    double annualFee,
    int years,
    double totalAmount)
{
    cout << "\n========================================\n";
    cout << "          MEMBERSHIP RECEIPT\n";
    cout << "========================================\n";

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
        << membershipType
        << "\n";

    cout << "Annual Fee    : RM "
        << fixed
        << setprecision(2)
        << annualFee
        << "\n";

    cout << "Duration      : "
        << years
        << " Year";

    if (years > 1)
        cout << "s";

    cout << "\n";

    cout << "Start Date    : "
        << membership.startDate
        << "\n";

    cout << "Expiry Date   : "
        << membership.expiryDate
        << "\n";

    cout << "Total Amount  : RM "
        << fixed
        << setprecision(2)
        << totalAmount
        << "\n";

    cout << "========================================\n";
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

    cout << "Annual Fee    : RM "
        << fixed
        << setprecision(2)
        << membership.annualFee
        << "\n";

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
    // CUSTOMER INFORMATION
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
    cout << "----------------------------------------\n";

    cout << "[1] Bronze  (RM50 / Annual)\n";
    cout << "[2] Silver  (RM100 / Annual)\n";
    cout << "[3] Gold    (RM150 / Annual)\n";
    cout << "[4] Skip\n";
    cout << "[0] Back to previous menu\n";

    cout << "----------------------------------------\n";

    int membershipChoice =
        getIntegerInput(
            "Select Membership Type (0-4): ",
            0,
            4
        );

    // ========================================================
    // BACK
    // ========================================================

    if (membershipChoice == 0)
    {
        return;
    }

    // ========================================================
    // SKIP
    // ========================================================

    if (membershipChoice == 4)
    {
        cout << "\nMembership registration skipped.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // MEMBERSHIP DETAILS
    // ========================================================

    string selectedType;
    int selectedDiscount;
    double selectedAnnualFee;

    switch (membershipChoice)
    {
    case 1:

        selectedType = "Bronze";
        selectedDiscount = 2;
        selectedAnnualFee = 50.00;

        break;

    case 2:

        selectedType = "Silver";
        selectedDiscount = 4;
        selectedAnnualFee = 100.00;

        break;

    case 3:

        selectedType = "Gold";
        selectedDiscount = 6;
        selectedAnnualFee = 150.00;

        break;
    }

    // ========================================================
    // ANNUAL MEMBERSHIP
    // ========================================================

    int durationYears = 1;

    string membershipID =
        generateMembershipID();

    string startDate =
        getCurrentDate();

    string expiryDate =
        calculateExpiryDate(
            startDate,
            durationYears
        );

    double totalAmount =
        selectedAnnualFee;

    // ========================================================
    // RECEIPT SUMMARY
    // ========================================================

    clearScreen();

    cout << "========================================\n";
    cout << "        MEMBERSHIP RECEIPT SUMMARY\n";
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

    cout << "Annual Fee    : RM "
        << fixed
        << setprecision(2)
        << selectedAnnualFee
        << "\n";

    cout << "Duration      : 1 Year\n";

    cout << "Start Date    : "
        << startDate
        << "\n";

    cout << "Expiry Date   : "
        << expiryDate
        << "\n";

    cout << "----------------------------------------\n";

    cout << "TOTAL TO PAY  : RM "
        << fixed
        << setprecision(2)
        << totalAmount
        << "\n";

    cout << "========================================\n";

    char confirmation =
        getYesNoInput(
            "\nProceed to payment? (Y/N): "
        );

    if (confirmation == 'N')
    {
        cout << "\nMembership registration cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // PROCESS PAYMENT
    // ========================================================

    bool paymentSuccessful =
        processPayment(totalAmount);

    if (!paymentSuccessful)
    {
        cout << "\nMembership registration was cancelled because payment was not completed.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // SAVE MEMBERSHIP
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

    membership.annualFee =
        selectedAnnualFee;

    membership.durationYears =
        durationYears;

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
    // FINAL RECEIPT
    // ========================================================

    clearScreen();

    cout << "========================================\n";
    cout << "       MEMBERSHIP REGISTRATION\n";
    cout << "========================================\n";

    cout << "\nMembership registered successfully!\n";

    displayReceipt(
        selectedType,
        selectedAnnualFee,
        durationYears,
        totalAmount
    );

    cout << "\nPayment Status : Successful\n";

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
    // UPDATE STATUS
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

    cout << "Annual Fee    : RM "
        << fixed
        << setprecision(2)
        << membership.annualFee
        << "\n";

    cout << "Current Expiry: "
        << membership.expiryDate
        << "\n";

    cout << "Status        : "
        << membership.status
        << "\n";

    // ========================================================
    // RENEWAL CONFIRMATION
    // ========================================================

    cout << "\nRenewal Fee:\n";

    cout << "----------------------------------------\n";

    cout << membership.membershipType
        << " Annual Renewal : RM "
        << fixed
        << setprecision(2)
        << membership.annualFee
        << "\n";

    cout << "----------------------------------------\n";

    char confirmation =
        getYesNoInput(
            "\nDo you want to renew for another year? (Y/N): "
        );

    if (confirmation == 'N')
    {
        cout << "\nMembership renewal cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // PREPARE RENEWAL
    // ========================================================

    string previousExpiryDate =
        membership.expiryDate;

    string newExpiryDate =
        calculateExpiryFromExistingDate(
            previousExpiryDate,
            1
        );

    double renewalFee =
        membership.annualFee;

    // ========================================================
    // RENEWAL RECEIPT
    // ========================================================

    clearScreen();

    cout << "========================================\n";
    cout << "          RENEWAL RECEIPT\n";
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

    cout << "Annual Fee    : RM "
        << fixed
        << setprecision(2)
        << renewalFee
        << "\n";

    cout << "Renewal       : 1 Year\n";

    cout << "Previous Exp. : "
        << previousExpiryDate
        << "\n";

    cout << "New Expiry    : "
        << newExpiryDate
        << "\n";

    cout << "----------------------------------------\n";

    cout << "TOTAL TO PAY  : RM "
        << fixed
        << setprecision(2)
        << renewalFee
        << "\n";

    cout << "========================================\n";

    char paymentConfirmation =
        getYesNoInput(
            "\nProceed to payment? (Y/N): "
        );

    if (paymentConfirmation == 'N')
    {
        cout << "\nMembership renewal cancelled.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // PAYMENT
    // ========================================================

    bool paymentSuccessful =
        processPayment(renewalFee);

    if (!paymentSuccessful)
    {
        cout << "\nMembership renewal cancelled because payment was not completed.\n";

        pauseScreen();

        return;
    }

    // ========================================================
    // SAVE RENEWAL
    // ========================================================

    membership.durationYears = 1;

    membership.expiryDate =
        newExpiryDate;

    membership.status =
        getMembershipStatus(
            membership.expiryDate
        );

    // ========================================================
    // FINAL RENEWAL RECEIPT
    // ========================================================

    clearScreen();

    cout << "========================================\n";
    cout << "        MEMBERSHIP RENEWAL\n";
    cout << "========================================\n";

    cout << "\nMembership renewed successfully!\n\n";

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

    cout << "Renewal Fee   : RM "
        << fixed
        << setprecision(2)
        << renewalFee
        << "\n";

    cout << "Previous Exp. : "
        << previousExpiryDate
        << "\n";

    cout << "New Expiry    : "
        << membership.expiryDate
        << "\n";

    cout << "Status        : "
        << membership.status
        << "\n";

    cout << "\nPayment Status : Successful\n";

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