#include "../include/parking.h"
#include <iostream>

using namespace std;

ParkingManager::ParkingManager(int numberOfSlots) {
    for (int i = 1; i <= numberOfSlots; i++) {
        ParkingSlot slot;

        slot.slotId = i;
        slot.slotNumber = string("A") + (i < 10 ? "0" : "") + to_string(i);
        slot.occupied = false;

        slots.push_back(slot);
    }
}

void ParkingManager::displaySlots() const {
    cout << "\n===== PARKING SLOT STATUS =====\n";

    for (const ParkingSlot& slot : slots) {
        cout << slot.slotNumber << " : ";

        if (slot.occupied) {
            cout << "OCCUPIED";
        } else {
            cout << "AVAILABLE";
        }

        cout << endl;
    }

    cout << "================================\n";
}

int ParkingManager::findAvailableSlot() const {
    for (const ParkingSlot& slot : slots) {
        if (!slot.occupied) {
            return slot.slotId;
        }
    }

    return -1;
}

bool ParkingManager::assignSlot() {
    int slotId = findAvailableSlot();

    if (slotId == -1) {
        return false;
    }

    for (ParkingSlot& slot : slots) {
        if (slot.slotId == slotId) {
            slot.occupied = true;

            cout << "Slot " << slot.slotNumber
                 << " has been assigned.\n";

            return true;
        }
    }

    return false;
}

bool ParkingManager::releaseSlot(int slotId) {
    for (ParkingSlot& slot : slots) {
        if (slot.slotId == slotId) {
            if (!slot.occupied) {
                return false;
            }

            slot.occupied = false;

            cout << "Slot " << slot.slotNumber
                 << " is now available.\n";

            return true;
        }
    }

    return false;
}