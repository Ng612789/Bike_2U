//#include <iostream>
//#include <vector>
//#include <string>
//#include <iomanip>
//#include <fstream>
//#include <sstream>
//
//using namespace std;
//
//// ---------------------------- Constants ----------------------------
//const double HOURLY_RATE = 5.50;
//
//// ---------------------------- Structure ----------------------------
//struct Bicycle {
//    int bikeID;
//    string type;
//    string brand;
//    double rentalRatePerHour;
//    bool isAvailable;
//};
//
//// ---------------------------- Function Declarations ----------------------------
//void DisplayInventoryMenu(vector<Bicycle>& inventory, int& nextBikeID);
//void AddBicycle(vector<Bicycle>& inventory, int& nextBikeID);
//void UpdateBicycle(vector<Bicycle>& inventory);
//void RemoveBicycle(vector<Bicycle>& inventory);
//int SearchBicycle(const vector<Bicycle>& inventory, int bikeID);
//void ViewBicycle(const vector<Bicycle>& inventory);
//void SaveInventoryToFile(const vector<Bicycle>& inventory);
//void LoadInventoryFromFile(vector<Bicycle>& inventory, int& nextBikeID);
//
//
//int main() {
//    vector<Bicycle> bicycles;   
//    int nextBikeID = 0;         
//
//    LoadInventoryFromFile(bicycles, nextBikeID);
//    DisplayInventoryMenu(bicycles, nextBikeID);
//    SaveInventoryToFile(bicycles);
//
//    return 0;
//}
//
//// ---------------------------- Bicycle Inventory Menu ----------------------------
//void DisplayInventoryMenu(vector<Bicycle>& inventory, int& nextBikeID) {
//    int option;
//    do {
//        cout << "\n-----------------------------------\n";
//        cout << "      Bicycle Inventory Menu       \n";
//        cout << "-----------------------------------\n";
//        cout << "1. Add a new bicycle\n";
//        cout << "2. Update a bicycle\n";
//        cout << "3. Remove a bicycle\n";
//        cout << "4. Search for a bicycle\n";
//        cout << "5. View all bicycles\n";
//        cout << "6. Return to main menu\n";
//        cout << "Please select an option (1-6): ";
//        cin >> option;
//
//        switch (option) {
//        case 1: AddBicycle(inventory, nextBikeID); break;
//        case 2: UpdateBicycle(inventory); break;
//        case 3: RemoveBicycle(inventory); break;
//        case 4: {
//            int id;
//            cout << "Enter bicycle ID to search: ";
//            cin >> id;
//            int idx = SearchBicycle(inventory, id);
//            if (idx != -1) {
//                cout << "Found: ID=" << inventory[idx].bikeID
//                    << ", Type=" << inventory[idx].type
//                    << ", Brand=" << inventory[idx].brand
//                    << ", Rate=" << fixed << setprecision(2) << inventory[idx].rentalRatePerHour
//                    << ", Available=" << (inventory[idx].isAvailable ? "Yes" : "No") << endl;
//            }
//            else {
//                cout << "Bicycle not found.\n";
//            }
//            break;
//        }
//        case 5: ViewBicycle(inventory); break;
//        case 6: cout << "Returning to main menu...\n"; return;
//        default: cout << "Invalid option. Please select between 1 and 6.\n";
//        }
//    } while (true);
//}
//
//// ---------------------------- Add Bicycle ----------------------------
//void AddBicycle(vector<Bicycle>& inventory, int& nextBikeID) {
//    Bicycle bike;
//    bike.bikeID = ++nextBikeID;   // uses the reference
//
//    cin.ignore();
//    cout << "Enter Bicycle Type (e.g., Mountain, Road, Electric): ";
//    getline(cin, bike.type);
//    cout << "Enter Bicycle Brand (e.g., Giant, Trek): ";
//    getline(cin, bike.brand);
//
//    double rate;
//    do {
//        cout << "Enter Rental Rate Per Hour (RM/h): ";
//        cin >> rate;
//        if (rate <= 0)
//            cout << "Rate must be positive. Please try again.\n";
//    } while (rate <= 0);
//    bike.rentalRatePerHour = rate;
//    bike.isAvailable = true;
//
//    inventory.push_back(bike);
//    cout << "Bicycle added successfully with ID: " << bike.bikeID << endl;
//}
//
//// ---------------------------- Update Bicycle ----------------------------
//void UpdateBicycle(vector<Bicycle>& inventory) {
//    int id;
//    cout << "Enter the Bicycle ID to update: ";
//    cin >> id;
//
//    int idx = SearchBicycle(inventory, id);
//    if (idx == -1) {
//        cout << "Bicycle ID not found.\n";
//        return;
//    }
//
//    int subOption;
//    do {
//        cout << "\n--- Update Menu for ID " << inventory[idx].bikeID << " ---\n";
//        cout << "1. Type      (current: " << inventory[idx].type << ")\n";
//        cout << "2. Brand     (current: " << inventory[idx].brand << ")\n";
//        cout << "3. Rate      (current: RM" << fixed << setprecision(2) << inventory[idx].rentalRatePerHour << ")\n";
//        cout << "4. Availability (current: " << (inventory[idx].isAvailable ? "Available" : "Not Available") << ")\n";
//        cout << "5. Return to Inventory Menu\n";
//        cout << "Enter choice (1-5): ";
//        cin >> subOption;
//
//        switch (subOption) {
//        case 1:
//            cin.ignore();
//            cout << "Enter new type: ";
//            getline(cin, inventory[idx].type);
//            cout << "Type updated.\n";
//            break;
//        case 2:
//            cin.ignore();
//            cout << "Enter new brand: ";
//            getline(cin, inventory[idx].brand);
//            cout << "Brand updated.\n";
//            break;
//        case 3: {
//            double newRate;
//            do {
//                cout << "Enter new rate (positive): ";
//                cin >> newRate;
//                if (newRate <= 0)
//                    cout << "Rate must be positive.\n";
//            } while (newRate <= 0);
//            inventory[idx].rentalRatePerHour = newRate;
//            cout << "Rate updated.\n";
//            break;
//        }
//        case 4: {
//            int status;
//            cout << "Enter 1 for Available, 0 for Not Available: ";
//            cin >> status;
//            inventory[idx].isAvailable = (status == 1);
//            cout << "Availability updated.\n";
//            break;
//        }
//        case 5:
//            cout << "Returning to inventory menu.\n";
//            return;
//        default:
//            cout << "Invalid option. Please select 1-5.\n";
//        }
//    } while (true);
//}
//
//// ---------------------------- Remove Bicycle ----------------------------
//void RemoveBicycle(vector<Bicycle>& inventory) {
//    int id;
//    cout << "Enter bicycle ID to remove: ";
//    cin >> id;
//
//    int idx = SearchBicycle(inventory, id);
//    if (idx == -1) {
//        cout << "Bicycle not found.\n";
//        return;
//    }
//
//    if (!inventory[idx].isAvailable) {
//        cout << "Cannot remove a bicycle that is currently rented.\n";
//        return;
//    }
//
//    inventory.erase(inventory.begin() + idx);
//    cout << "Bicycle removed successfully.\n";
//}
//
//// ---------------------------- Search Bicycle ----------------------------
//int SearchBicycle(const vector<Bicycle>& inventory, int bikeID) {
//    for (size_t i = 0; i < inventory.size(); ++i) {
//        if (inventory[i].bikeID == bikeID)
//            return static_cast<int>(i);
//    }
//    return -1;
//}
//
//// ---------------------------- View Bicycle ----------------------------
//void ViewBicycle(const vector<Bicycle>& inventory) {
//    if (inventory.empty()) {
//        cout << "\nNo bicycles in inventory.\n";
//        return;
//    }
//
//    cout << "\n----- All Bicycles -----\n";
//    cout << left << setw(6) << "ID"
//        << setw(15) << "Type"
//        << setw(15) << "Brand"
//        << setw(15) << "Rate (RM/h)"
//        << setw(12) << "Available" << "\n";
//    cout << string(63, '-') << "\n";
//
//    for (size_t i = 0; i < inventory.size(); ++i) {
//        cout << left << setw(6) << inventory[i].bikeID
//            << setw(15) << inventory[i].type
//            << setw(15) << inventory[i].brand
//            << setw(15) << fixed << setprecision(2) << inventory[i].rentalRatePerHour
//            << setw(12) << (inventory[i].isAvailable ? "Yes" : "No") << "\n";
//    }
//}
//
//// ---------------------------- Save to File ----------------------------
//void SaveInventoryToFile(const vector<Bicycle>& inventory) {
//    ofstream outFile("inventory.csv");
//    if (!outFile) {
//        cerr << "Error: Cannot open inventory.csv.\n";
//        return;
//    }
//
//    for (const auto& bike : inventory) {
//        outFile << bike.bikeID << ","
//            << bike.type << ","
//            << bike.brand << ","
//            << bike.rentalRatePerHour << ","
//            << (bike.isAvailable ? 1 : 0) << "\n";
//    }
//
//    outFile.close();
//}
//
//// ---------------------------- Load from File ----------------------------
//void LoadInventoryFromFile(vector<Bicycle>& inventory, int& nextBikeID) {
//    ifstream inFile("inventory.csv");
//    if (!inFile) {
//        return;
//    }
//
//    string line;
//    int maxID = 0;
//    while (getline(inFile, line)) {
//        if (line.empty()) continue;
//
//        stringstream ss(line);
//        string token;
//        Bicycle bike;
//
//        getline(ss, token, ',');
//        bike.bikeID = stoi(token);
//
//        getline(ss, token, ',');
//        bike.type = token;
//
//        getline(ss, token, ',');
//        bike.brand = token;
//
//        getline(ss, token, ',');
//        bike.rentalRatePerHour = stod(token);
//
//        getline(ss, token, ',');
//        bike.isAvailable = (stoi(token) == 1);
//
//        inventory.push_back(bike);
//
//        if (bike.bikeID > maxID)
//            maxID = bike.bikeID;
//    }
//
//    inFile.close();
//
//    if (!inventory.empty()) {
//        nextBikeID = maxID + 1; 
//    }
//}