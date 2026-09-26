#include "../include/session.h"
#include <iostream>

using namespace std;

bool SessionManager::startSession(const string& registrationNumber, int slotId, const string& slotNumber) {
    if (isVehicleParked(registrationNumber)) {
        cout << "Error: Vehicle " << registrationNumber << " already has an active session.\n";
        return false;
    }

    ParkingSession session;
    session.registrationNumber = registrationNumber;
    session.slotId = slotId;
    session.slotNumber = slotNumber;
    session.entryTime = time(nullptr); // Current system timestamp
    session.exitTime = 0;
    session.isActive = true;

    activeSessions[registrationNumber] = session;
    return true;
}

bool SessionManager::endSession(const string& registrationNumber, double &durationInSeconds, int &releasedSlotId) {
    auto it = activeSessions.find(registrationNumber);
    if (it == activeSessions.end() || !it->second.isActive) {
        cout << "Error: No active session found for vehicle " << registrationNumber << ".\n";
        return false;
    }

    time_t exitTime = time(nullptr);
    it->second.exitTime = exitTime;
    it->second.isActive = false;

    // Calculate time spent in seconds
    durationInSeconds = difftime(exitTime, it->second.entryTime);
    releasedSlotId = it->second.slotId;

    // Remove from active tracking map
    activeSessions.erase(it);
    return true;
}

bool SessionManager::isVehicleParked(const string& registrationNumber) const {
    return activeSessions.find(registrationNumber) != activeSessions.end();
}

void SessionManager::displayActiveSessions() const {
    cout << "\n===== ACTIVE PARKING SESSIONS =====\n";
    if (activeSessions.empty()) {
        cout << "No active parking sessions.\n";
    } else {
        for (const auto& pair : activeSessions) {
            cout << "Vehicle: " << pair.second.registrationNumber
                 << " | Slot: " << pair.second.slotNumber
                 << " (ID: " << pair.second.slotId << ")"
                 << " | Entry Time: " << ctime(&pair.second.entryTime);
        }
    }
    cout << "===================================\n";
}