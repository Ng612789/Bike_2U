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
    MemberType getType() const {
        return type;
    }

    double getPrice() const {
        return price;
    }

    // Get membership type as string
    string getTypeString() const {
        switch (type) {
        case BRONZE:
            return "Bronze";

        case SILVER:
            return "Silver";

        case GOLD:
            return "Gold";

        case NONE:
            return "None";

        default:
            return "Unknown";
        }
    }
};

// Function to get membership type from user
MemberType getMemberTypeFromUser() {

    int choice;

    cout << "\n";
    cout << "========================================" << endl;
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

    cin >> choice;

    // Input validation
    while (cin.fail() || choice < 0 || choice > 4) {

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "\n";
        cout << "Invalid choice." << endl;
        cout << "Please enter a number from 0 to 4." << endl;
        cout << "Enter choice (0-4): ";

        cin >> choice;
    }

    // Return selected membership type
    switch (choice) {

    case 1:
        return BRONZE;

    case 2:
        return SILVER;

    case 3:
        return GOLD;

    case 4:
        return NONE;  // Skip

    case 0:
        return NONE;  // Back to previous menu

    default:
        return NONE;
    }
}

// Main function
int main() {

    Member member;
    bool isMember = false;

    MemberType type = getMemberTypeFromUser();

    // Check if user chose to skip or go back
    if (type == NONE) {

        cout << "\n";
        cout << "You have chosen to skip membership." << endl;
        cout << "Returning to previous page..." << endl;

        isMember = false;
    }
    else {

        member = Member(type);
        isMember = true;

        cout << "\n";
        cout << "SUCCESS: You are a "
            << member.getTypeString()
            << " member now!" << endl;
    }

    return 0;
}