#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <ctime>
#include <unordered_map>

using namespace std;

struct ParkingSession {
    string registrationNumber;
    int slotId;
    string slotNumber;
    time_t entryTime;
    time_t exitTime;
    bool isActive;
};

class SessionManager {
private:
    // Maps vehicle registration number -> active parking session
    unordered_map<string, ParkingSession> activeSessions;

public:
    bool startSession(const string& registrationNumber, int slotId, const string& slotNumber);
    bool endSession(const string& registrationNumber, double &durationInSeconds, int &releasedSlotId);
    bool isVehicleParked(const string& registrationNumber) const;
    void displayActiveSessions() const;
};

#endif