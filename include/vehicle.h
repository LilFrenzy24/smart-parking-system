#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <unordered_map>

using namespace std;

struct Vehicle {
    string registrationNumber;
    string vehicleType;
};

class VehicleManager {
private:
    unordered_map<string, Vehicle> vehicles;

public:
    bool addVehicle(string registrationNumber, string vehicleType);
    bool vehicleExists(string registrationNumber) const;
    Vehicle* findVehicle(string registrationNumber);
    void displayVehicles() const;
};

#endif