#include <iostream>
#include <string>
#include <limits>
using namespace std;

// Enum for membership types
enum MemberType {
    BRONZE,
    SILVER,
    GOLD,
    NONE
};

// Class for Member
class Member {
private:
    MemberType type;
    double price;

public:
    // Constructor
    Member() : type(NONE), price(0.0) {}

    // Parameterized Constructor
    Member(MemberType t) {
        type = t;
        setPrice(t);
    }

    // Set price based on membership type
    void setPrice(MemberType t) {
        switch (t) {
        case BRONZE:
            price = 50.0;
            break;
        case SILVER:
            price = 100.0;
            break;
        case GOLD:
            price = 150.0;
            break;
        case NONE:
            price = 0.0;
            break;
        }
    }

    // Set membership type
    void setType(MemberType t) {
        type = t;
        setPrice(t);
    }

    // Getter methods
    MemberType getType() const { return type; }
    double getPrice() const { return price; }

    // Get membership type as string
    string getTypeString() const {
        switch (type) {
        case BRONZE: return "Bronze";
        case SILVER: return "Silver";
        case GOLD: return "Gold";
        case NONE: return "None";
        default: return "Unknown";
        }
    }
};

// Function to get membership type from user
MemberType getMemberTypeFromUser() {
    int choice;
    cout << "\nSelect Membership Type:" << endl;
    cout << "1. Bronze (RM50)" << endl;
    cout << "2. Silver (RM100)" << endl;
    cout << "3. Gold (RM150)" << endl;
    cout << "4. Skip" << endl;
    cout << "0. Back to previous menu" << endl;
    cout << "Enter choice (0-4): ";
    cin >> choice;

    // Input validation
    while (cin.fail() || choice < 0 || choice > 4) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "ERROR: Invalid choice! Please enter 0, 1, 2, 3, or 4: ";
        cin >> choice;
    }

    switch (choice) {
    case 1: return BRONZE;
    case 2: return SILVER;
    case 3: return GOLD;
    case 4: return NONE;  // Skip
    case 0: return NONE;  // Back to previous menu
    default: return NONE;
    }
}

int main() {
    Member member;
    bool isMember = false;

    cout << "\n========================================" << endl;
    cout << "   BECOME A MEMBER ? (OPTIONAL)" << endl;
    cout << "========================================" << endl;

    MemberType type = getMemberTypeFromUser();

    // Check if user chose to skip or go back
    if (type == NONE) {
        cout << "\nYou have chosen to skip membership." << endl;
        cout << "Returning to previous page..." << endl;
        isMember = false;
    }
    else {
        member = Member(type);
        isMember = true;
        cout << "\nSUCCESS: You are a " << member.getTypeString() << " member now!" << endl;
    }

    return 0;
}