#include "../include/database.h"
#include <iostream>

using namespace std;

DatabaseManager::DatabaseManager(const string& path) : db(nullptr), dbPath(path) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        cerr << "[Database Error] Cannot open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    
    // Enable foreign keys
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    return true;
}

void DatabaseManager::disconnect() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

// ---------------------------------------------------------
// VEHICLE OPERATIONS
// ---------------------------------------------------------
bool DatabaseManager::addVehicle(const string& regNo, const string& type) {
    string sql = "INSERT OR IGNORE INTO vehicles (registration_number, vehicle_type) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, regNo.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, type.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

int DatabaseManager::getVehicleId(const string& regNo) {
    string sql = "SELECT vehicle_id FROM vehicles WHERE registration_number = ?;";
    sqlite3_stmt* stmt;
    int vehicleId = -1;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, regNo.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            vehicleId = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return vehicleId;
}

// ---------------------------------------------------------
// SLOT OPERATIONS
// ---------------------------------------------------------
bool DatabaseManager::updateSlotStatus(const string& slotNumber, const string& status) {
    string sql = "UPDATE parking_slots SET status = ? WHERE slot_number = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, slotNumber.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

void DatabaseManager::displaySlotsFromDB() {
    string sql = "SELECT slot_number, status FROM parking_slots ORDER BY slot_id ASC;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        cout << "\n--- PARKING SLOTS STATUS (DATABASE) ---\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string slotNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            cout << "Slot " << slotNo << ": [" << status << "]\n";
        }
        cout << "--------------------------------------\n";
    }
    sqlite3_finalize(stmt);
}

// ---------------------------------------------------------
// SESSION OPERATIONS
// ---------------------------------------------------------
bool DatabaseManager::startSession(const string& regNo, const string& slotNumber) {
    int vId = getVehicleId(regNo);
    if (vId == -1) return false;

    // Get slot_id
    string getSlotSql = "SELECT slot_id FROM parking_slots WHERE slot_number = ?;";
    sqlite3_stmt* stmt;
    int sId = -1;

    if (sqlite3_prepare_v2(db, getSlotSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, slotNumber.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sId = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);

    if (sId == -1) return false;

    // Insert active session
    string insertSql = "INSERT INTO parking_sessions (vehicle_id, slot_id, status) VALUES (?, ?, 'ACTIVE');";
    if (sqlite3_prepare_v2(db, insertSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, vId);
    sqlite3_bind_int(stmt, 2, sId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (success) {
        updateSlotStatus(slotNumber, "OCCUPIED");
    }

    return success;
}

bool DatabaseManager::endSession(const string& regNo, double durationMinutes, double amountDue) {
    int vId = getVehicleId(regNo);
    if (vId == -1) return false;

    // Find active session for vehicle
    string findSql = "SELECT session_id, slot_id FROM parking_sessions WHERE vehicle_id = ? AND status = 'ACTIVE';";
    sqlite3_stmt* stmt;
    int sessionId = -1;
    int slotId = -1;

    if (sqlite3_prepare_v2(db, findSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, vId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sessionId = sqlite3_column_int(stmt, 0);
            slotId = sqlite3_column_int(stmt, 1);
        }
    }
    sqlite3_finalize(stmt);

    if (sessionId == -1) return false;

    // Update session record
    string updateSql = "UPDATE parking_sessions SET exit_time = CURRENT_TIMESTAMP, duration_minutes = ?, amount_due = ?, status = 'COMPLETED' WHERE session_id = ?;";
    if (sqlite3_prepare_v2(db, updateSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_double(stmt, 1, durationMinutes);
    sqlite3_bind_double(stmt, 2, amountDue);
    sqlite3_bind_int(stmt, 3, sessionId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (success) {
        // Record payment
        recordPayment(sessionId, amountDue, "SUCCESSFUL");

        // Free up slot
        string freeSlotSql = "UPDATE parking_slots SET status = 'AVAILABLE' WHERE slot_id = ?;";
        if (sqlite3_prepare_v2(db, freeSlotSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, slotId);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }

    return success;
}

// ---------------------------------------------------------
// PAYMENT OPERATIONS
// ---------------------------------------------------------
bool DatabaseManager::recordPayment(int sessionId, double amountPaid, const string& status) {
    string sql = "INSERT INTO payments (session_id, amount_paid, payment_status) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, sessionId);
    sqlite3_bind_double(stmt, 2, amountPaid);
    sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}