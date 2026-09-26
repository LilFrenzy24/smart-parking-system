#ifndef PARKING_H
#define PARKING_H

#include <string>
#include <vector>

using namespace std;

struct ParkingSlot {
    int slotId;
    string slotNumber;
    bool occupied;
};

class ParkingManager {
private:
    vector<ParkingSlot> slots;

public:
    ParkingManager(int numberOfSlots);

    void displaySlots() const;
    int findAvailableSlot() const;
    bool assignSlot();
    bool releaseSlot(int slotId);
};

#endif