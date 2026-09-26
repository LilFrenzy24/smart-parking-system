#include "../include/vehicle.h"
#include <iostream>

using namespace std;

bool VehicleManager::addVehicle(string registrationNumber, string vehicleType) {
    if (vehicleExists(registrationNumber)) {
        return false;
    }

    Vehicle vehicle;
    vehicle.registrationNumber = registrationNumber;
    vehicle.vehicleType = vehicleType;

    vehicles[registrationNumber] = vehicle;
    return true;
}

bool VehicleManager::vehicleExists(string registrationNumber) const {
    return vehicles.find(registrationNumber) != vehicles.end();
}

Vehicle* VehicleManager::findVehicle(string registrationNumber) {
    auto it = vehicles.find(registrationNumber);
    if (it != vehicles.end()) {
        return &it->second;
    }
    return nullptr;
}

void VehicleManager::displayVehicles() const {
    cout << "\n===== REGISTERED VEHICLES =====\n";

    if (vehicles.empty()) {
        cout << "No vehicles registered.\n";
    }

    for (const auto& pair : vehicles) {
        cout << "Registration: "
             << pair.second.registrationNumber
             << " | Type: "
             << pair.second.vehicleType
             << endl;
    }

    cout << "================================\n";
}