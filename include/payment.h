#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>

using namespace std;

class PaymentManager {
private:
    double baseRate;     // Base entry charge (e.g., KSh 50)
    double hourlyRate;   // Rate per hour (e.g., KSh 100)

public:
    PaymentManager(double base = 50.0, double hourly = 100.0);

    // Calculates total fee based on duration in seconds
    double calculateFee(double durationInSeconds) const;

    // Simulates processing a payment
    bool processPayment(const string& registrationNumber, double amount);

    // Displays the formal receipt
    void displayReceipt(const string& registrationNumber, double durationInSeconds, double amountPaid) const;
};

#endif