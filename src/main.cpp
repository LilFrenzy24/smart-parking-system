#include "../include/parking.h"
#include "../include/vehicle.h"
#include "../include/session.h"
#include "../include/payment.h"
#include "../include/database.h"
#include <iostream>
#include <string>
#include <limits>

using namespace std;

void displayMenu() {
    cout << "\n========================================" << endl;
    cout << "   SMART PARKING MANAGEMENT SYSTEM      " << endl;
    cout << "   (SQLite Database Integrated)         " << endl;
    cout << "========================================" << endl;
    cout << "1. Park Vehicle (Entry)" << endl;
    cout << "2. Checkout Vehicle (Exit & Pay)" << endl;
    cout << "3. View Parking Slots Status (Database)" << endl;
    cout << "4. View Registered Vehicles" << endl;
    cout << "5. View Active Parking Sessions" << endl;
    cout << "6. Exit Program" << endl;
    cout << "========================================" << endl;
    cout << "Enter your choice (1-6): ";
}

int main() {
    // 1. Initialize In-Memory Managers and Pricing
    ParkingManager parkingManager(10);
    VehicleManager vehicleManager;
    SessionManager sessionManager;
    PaymentManager paymentManager(50.0, 100.0); // Base KSh 50, Hourly KSh 100

    // 2. Initialize and Connect Database Manager
    DatabaseManager dbManager("database/parking.db");
    if (!dbManager.connect()) {
        cerr << "Failed to connect to database. Exiting..." << endl;
        return 1;
    }

    int choice = 0;

    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 6) {
            cout << "\nClosing database connection and exiting. Goodbye!\n" << endl;
            dbManager.disconnect();
            break;
        }

        switch (choice) {
            case 1: {
                // Entry Workflow
                string regNo, type;
                cout << "\n--- VEHICLE ENTRY ---" << endl;
                cout << "Enter Vehicle Registration Number (e.g., KDA 123A): ";
                cin.ignore();
                getline(cin, regNo);

                if (sessionManager.isVehicleParked(regNo)) {
                    cout << "Error: Vehicle " << regNo << " is already parked!" << endl;
                    break;
                }

                int slotId = parkingManager.findAvailableSlot();
                if (slotId == -1) {
                    cout << "Sorry, the parking lot is currently FULL!" << endl;
                    break;
                }

                cout << "Enter Vehicle Type (e.g., Saloon, SUV, Lorry): ";
                getline(cin, type);

                // Add to In-Memory System
                vehicleManager.addVehicle(regNo, type);
                if (parkingManager.assignSlot()) {
                    string slotNumber = string("A") + (slotId < 10 ? "0" : "") + to_string(slotId);
                    sessionManager.startSession(regNo, slotId, slotNumber);

                    // Persist to SQLite Database
                    dbManager.addVehicle(regNo, type);
                    dbManager.startSession(regNo, slotNumber);

                    cout << "SUCCESS: Vehicle " << regNo << " parked in Slot " << slotNumber 
                         << " (Saved to Database)" << endl;
                }
                break;
            }

            case 2: {
                // Exit & Checkout Workflow
                string regNo;
                cout << "\n--- VEHICLE CHECKOUT ---" << endl;
                cout << "Enter Vehicle Registration Number: ";
                cin.ignore();
                getline(cin, regNo);

                double durationSec = 0.0;
                int releasedSlotId = -1;

                if (sessionManager.endSession(regNo, durationSec, releasedSlotId)) {
                    parkingManager.releaseSlot(releasedSlotId);

                    // Calculate Fee & Process Payment
                    double totalFee = paymentManager.calculateFee(durationSec);
                    double durationMin = durationSec / 60.0;

                    if (paymentManager.processPayment(regNo, totalFee)) {
                        paymentManager.displayReceipt(regNo, durationSec, totalFee);

                        // Persist Completion to SQLite Database
                        dbManager.endSession(regNo, durationMin, totalFee);
                    }
                }
                break;
            }

            case 3:
                // View Slots directly from SQLite Database
                dbManager.displaySlotsFromDB();
                break;

            case 4:
                vehicleManager.displayVehicles();
                break;

            case 5:
                sessionManager.displayActiveSessions();
                break;

            default:
                cout << "Invalid choice! Please choose an option between 1 and 6." << endl;
                break;
        }
    }

    return 0;
}