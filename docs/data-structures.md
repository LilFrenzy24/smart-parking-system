# Smart Parking Management System - Data Structures

## 1. Introduction

Data structures are used to organize and manage the information required by the Smart Parking Management System.

The selected data structures should allow the system to efficiently store parking slots, identify vehicles, maintain parking sessions, and record payments.

The following data structures will be used in the system.

---

## 2. Structure for Parking Slots

### Data Structure: Vector

A C++ `vector` will be used to store the parking slots.

Each slot will contain information such as:

* Slot ID
* Slot number
* Occupancy status

### Reason for Use

A parking facility has a collection of parking slots that can be stored sequentially. A vector allows the system to maintain a dynamic collection of slots and easily iterate through them when checking for available spaces.

For example:

```cpp
vector<ParkingSlot> slots;
```

The system can search through the vector to find an available slot.

---

## 3. Structure for Vehicle Records

### Data Structure: Unordered Map

A C++ `unordered_map` will be used to store active vehicle records.

The vehicle registration number will be used as the key.

For example:

```cpp
unordered_map<string, Vehicle> vehicles;
```

### Reason for Use

Vehicle registration numbers can be used to uniquely identify vehicles in the parking system.

An unordered map allows the system to quickly locate a vehicle record using its registration number. This is particularly useful when a vehicle exits because the system needs to retrieve its parking information.

---

## 4. Structure for Parking Sessions

### Data Structure: Vector

A `vector` will be used to store parking session records.

A parking session can contain:

* Session ID
* Vehicle registration number
* Parking slot
* Entry time
* Exit time
* Parking duration
* Amount payable

For example:

```cpp
vector<ParkingSession> sessions;
```

### Reason for Use

Parking sessions form a collection of records that can grow as vehicles enter the parking facility. A vector allows the system to dynamically store these records and iterate through them when required.

---

## 5. Structure for Payment Records

### Data Structure: Vector

A `vector` will be used to store payment records.

Each payment record can contain:

* Payment ID
* Parking session ID
* Amount paid
* Payment time
* Payment status

For example:

```cpp
vector<Payment> payments;
```

### Reason for Use

The system can generate multiple payment records over time. A vector provides a simple dynamic structure for storing and accessing these records.

---

## 6. Structure for Individual Records

### Data Structure: Struct / Class

C++ `struct` or `class` types will be used to represent individual entities such as vehicles, parking slots, parking sessions, and payments.

For example:

```cpp
struct ParkingSlot {
    int slotId;
    string slotNumber;
    bool occupied;
};
```

### Reason for Use

A structure groups related information into one logical entity. This makes the system easier to organize and allows each vehicle, slot, session, or payment to be represented as a single object.

---

## 7. Summary of Data Structures

| Information         | Data Structure     | Reason                                                |
| ------------------- | ------------------ | ----------------------------------------------------- |
| Parking slots       | `vector`           | Stores a dynamic collection of parking slots          |
| Active vehicles     | `unordered_map`    | Allows quick lookup using vehicle registration number |
| Parking sessions    | `vector`           | Stores multiple parking session records               |
| Payments            | `vector`           | Stores payment records dynamically                    |
| Individual entities | `struct` / `class` | Groups related attributes into one object             |

## 8. Relationship Between Data Structures and Algorithms

The data structures support the algorithms developed for the system.

For example:

1. The **Parking Slot Management Algorithm** searches the vector of parking slots to find an available slot.
2. The **Vehicle Entry Algorithm** stores vehicle information in the vehicle map and associates the vehicle with an available slot.
3. The **Vehicle Exit Algorithm** uses the vehicle registration number to locate the vehicle record.
4. The **Duration Calculation Algorithm** uses the entry and exit times stored in the parking session.
5. The **Fee Calculation Algorithm** uses the calculated parking duration to determine the amount payable.
6. The **Payment Algorithm** creates and stores a payment record.
7. The **Barrier Control Algorithm** checks the payment status before authorizing the vehicle to exit.
