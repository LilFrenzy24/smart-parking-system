# Smart Parking Management System

## 1. Introduction

The Smart Parking Management System is proposed to automate parking operations for the client. The system will manage parking slot availability, vehicle entry and exit, parking duration, payment calculation, and exit barrier control.

The system is designed around the requirements provided by the client in the Data Structures and Algorithms task.

## 2. Problem Statement

The client requires an automated parking system that allows drivers to see available parking slots before entering the parking area. The system must record vehicles when they arrive and, when they leave, automatically determine the total time spent in the parking area and the amount to be paid.

After the required parking fee has been paid, the system should allow the vehicle to exit by opening the exit barrier.

## 3. System Objectives

The system should:

1. Display the availability of parking slots before a vehicle enters.
2. Record vehicles arriving at the parking facility.
3. Assign available parking slots to incoming vehicles.
4. Record the vehicle's parking entry time.
5. Track vehicles while they are parked.
6. Record the vehicle's exit time.
7. Calculate the total duration that the vehicle was parked.
8. Calculate the amount payable based on the parking duration.
9. Record and process the parking payment.
10. Open the exit barrier after successful payment.
11. Update the parking slot to available after the vehicle leaves.

## 4. Proposed System Modules

The system will be divided into the following modules:

### 4.1 Parking Slot Management Module

Responsible for maintaining the status of parking slots and determining which slots are available or occupied.

### 4.2 Slot Availability Display Module

Responsible for displaying the current availability of parking slots to drivers before entry.

### 4.3 Vehicle Entry Module

Responsible for recording arriving vehicles, assigning available parking slots, and recording their entry time.

### 4.4 Vehicle and Parking Session Module

Responsible for maintaining information about vehicles currently parked and their associated parking sessions.

### 4.5 Vehicle Exit Module

Responsible for processing vehicles leaving the parking facility and retrieving their parking information.

### 4.6 Parking Duration Module

Responsible for calculating the total amount of time a vehicle spent in the parking facility.

### 4.7 Fee Calculation Module

Responsible for calculating the amount that the vehicle owner needs to pay based on the parking duration.

### 4.8 Payment and Barrier Module

Responsible for recording payment and allowing the exit barrier to open after the required parking fee has been paid.

### 4.9 Database Management Module

Responsible for storing and updating vehicle, parking slot, parking session, and payment information.

## 5. Basic System Flow

The overall operation of the proposed system is:

1. Display available parking slots.
2. A vehicle arrives at the parking facility.
3. Check whether a parking slot is available.
4. If a slot is available, assign it to the vehicle.
5. Record the vehicle and entry time.
6. The vehicle remains parked.
7. When the vehicle exits, retrieve its parking information.
8. Record the exit time.
9. Calculate the total parking duration.
10. Calculate the parking fee.
11. Process the payment.
12. If payment is successful, open the exit barrier.
13. Mark the parking slot as available again.

## 6. Main System Inputs

The system will require information such as:

* Vehicle registration number
* Vehicle type, where applicable
* Parking slot information
* Entry time
* Exit time
* Parking duration
* Parking fee
* Payment information

## 7. Main System Outputs

The system should provide:

* Current parking slot availability
* Assigned parking slot
* Vehicle parking information
* Total parking duration
* Amount payable
* Payment status
* Exit authorization/barrier status

## 8. Expected System Behaviour

If parking slots are available, the system should allow an arriving vehicle to be assigned a slot and record its parking information.

If no parking slot is available, the system should indicate that parking is full rather than assigning an occupied slot.

When a parked vehicle requests to exit, the system should calculate the parking duration and amount payable. The exit barrier should only be opened after the required parking fee has been paid.

After the vehicle exits, its parking slot should be updated as available for another vehicle.
