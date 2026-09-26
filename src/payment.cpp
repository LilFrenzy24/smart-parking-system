#include "../include/payment.h"
#include <iostream>
#include <cmath>

using namespace std;

PaymentManager::PaymentManager(double base, double hourly) 
    : baseRate(base), hourlyRate(hourly) {}

double PaymentManager::calculateFee(double durationInSeconds) const {
    if (durationInSeconds <= 0) return 0.0;

    // For testing/simulation, 1 second = 1 hour (or actual hours using 3600s)
    // Minimum 1 hour charge
    int hoursParked = ceil(durationInSeconds / 3600.0); 
    if (hoursParked < 1) hoursParked = 1;

    return baseRate + (hoursParked * hourlyRate);
}

bool PaymentManager::processPayment(const string& registrationNumber, double amount) {
    if (amount <= 0) {
        cout << "Error: Invalid payment amount.\n";
        return false;
    }

    cout << "\n[Payment System] Processing payment of KSh " << amount 
         << " for vehicle " << registrationNumber << "...\n";
    cout << "[Payment System] Payment successful! Gate barrier opening...\n";
    return true;
}

void PaymentManager::displayReceipt(const string& registrationNumber, double durationInSeconds, double amountPaid) const {
    cout << "\n====================================\n";
    cout << "          PARKING RECEIPT           \n";
    cout << "====================================\n";
    cout << "Vehicle Reg:    " << registrationNumber << "\n";
    cout << "Duration Spent: " << durationInSeconds << " seconds\n";
    cout << "Base Charge:    KSh " << baseRate << "\n";
    cout << "Total Paid:     KSh " << amountPaid << "\n";
    cout << "Status:         PAID\n";
    cout << "====================================\n";
}