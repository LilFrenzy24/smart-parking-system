#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>

using namespace std;

class DatabaseManager {
private:
    sqlite3* db;
    string dbPath;

public:
    // Constructor accepts the database path (default: "database/parking.db")
    DatabaseManager(const string& path = "database/parking.db");
    ~DatabaseManager();

    // Core Database Control
    bool connect();
    void disconnect();

    // Vehicle Operations
    bool addVehicle(const string& regNo, const string& type);
    int getVehicleId(const string& regNo);

    // Slot Operations
    bool updateSlotStatus(const string& slotNumber, const string& status);
    void displaySlotsFromDB();

    // Session Operations
    bool startSession(const string& regNo, const string& slotNumber);
    bool endSession(const string& regNo, double durationMinutes, double amountDue);

    // Payment Operations
    bool recordPayment(int sessionId, double amountPaid, const string& status);
};

#endif