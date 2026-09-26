# Smart Parking Management System - Database Design

## 1. Introduction

The Smart Parking Management System requires a dynamic database to store and update information about vehicles, parking slots, parking sessions, and payments.

The database will use SQLite as the database management system. SQLite provides a lightweight relational database that can be used by the C++ application without requiring a separate database server.

---

## 2. Database Requirements

The database should be able to:

1. Store vehicle information.
2. Store parking slot information.
3. Record when a vehicle enters.
4. Record the parking slot assigned to a vehicle.
5. Record when a vehicle exits.
6. Store parking duration.
7. Store the amount payable.
8. Store payment information.
9. Track whether a parking slot is available or occupied.
10. Update records when a vehicle leaves.

---

## 3. Database Tables

The database will contain four main tables:

```text
VEHICLES
    |
    | 1
    |
    | M
PARKING_SESSIONS
    |
    | 1
    |
    | 1
PAYMENTS

PARKING_SLOTS
    |
    | 1
    |
    | M
PARKING_SESSIONS
```

### 3.1 Vehicles Table

The `vehicles` table stores information about vehicles using the parking facility.

| Field               | Data Type | Description                 |
| ------------------- | --------- | --------------------------- |
| vehicle_id          | INTEGER   | Unique vehicle identifier   |
| registration_number | TEXT      | Vehicle registration number |
| vehicle_type        | TEXT      | Type of vehicle             |

Primary Key:

```text
vehicle_id
```

The registration number should also be unique because it identifies the vehicle.

---

### 3.2 Parking Slots Table

The `parking_slots` table stores information about individual parking spaces.

| Field       | Data Type | Description                 |
| ----------- | --------- | --------------------------- |
| slot_id     | INTEGER   | Unique slot identifier      |
| slot_number | TEXT      | Visible parking slot number |
| status      | TEXT      | AVAILABLE or OCCUPIED       |

Primary Key:

```text
slot_id
```

The status changes dynamically when vehicles enter and leave.

For example:

```text
Before vehicle enters:

Slot A01 → AVAILABLE

After vehicle is assigned:

Slot A01 → OCCUPIED

After vehicle exits:

Slot A01 → AVAILABLE
```

---

### 3.3 Parking Sessions Table

The `parking_sessions` table records each parking visit.

| Field            | Data Type | Description                       |
| ---------------- | --------- | --------------------------------- |
| session_id       | INTEGER   | Unique parking session identifier |
| vehicle_id       | INTEGER   | Vehicle using the parking slot    |
| slot_id          | INTEGER   | Assigned parking slot             |
| entry_time       | DATETIME  | Time vehicle entered              |
| exit_time        | DATETIME  | Time vehicle exited               |
| duration_minutes | INTEGER   | Total parking duration            |
| amount_due       | REAL      | Amount the vehicle must pay       |
| status           | TEXT      | ACTIVE or COMPLETED               |

Primary Key:

```text
session_id
```

Foreign Keys:

```text
vehicle_id → vehicles.vehicle_id
slot_id → parking_slots.slot_id
```

---

### 3.4 Payments Table

The `payments` table records payments made for parking sessions.

| Field          | Data Type | Description                    |
| -------------- | --------- | ------------------------------ |
| payment_id     | INTEGER   | Unique payment identifier      |
| session_id     | INTEGER   | Parking session being paid for |
| amount_paid    | REAL      | Amount paid                    |
| payment_time   | DATETIME  | Time payment was made          |
| payment_status | TEXT      | SUCCESSFUL or FAILED           |

Primary Key:

```text
payment_id
```

Foreign Key:

```text
session_id → parking_sessions.session_id
```

---

## 4. Entity Relationships

### Vehicle → Parking Session

One vehicle can have many parking sessions over time.

For example:

```text
KDA 123A
   |
   ├── Session 1
   ├── Session 2
   └── Session 3
```

Therefore:

```text
VEHICLES 1 ─────── M PARKING_SESSIONS
```

---

### Parking Slot → Parking Session

A parking slot can be used by many vehicles over time, although it can only be occupied by one active parking session at a time.

Therefore:

```text
PARKING_SLOTS 1 ─────── M PARKING_SESSIONS
```

---

### Parking Session → Payment

A parking session is associated with its payment record.

Therefore:

```text
PARKING_SESSIONS 1 ─────── 1 PAYMENTS
```

---

## 5. Dynamic Database Operation

The database changes as vehicles use the parking facility.

### Vehicle Entry

When a vehicle enters:

1. Check for an available parking slot.
2. Create the vehicle record if necessary.
3. Assign the available slot.
4. Create a new parking session.
5. Record the entry time.
6. Change the slot status from `AVAILABLE` to `OCCUPIED`.

Example:

```text
Slot A01
AVAILABLE
     ↓
Vehicle KDA 123A enters
     ↓
Slot A01
OCCUPIED
```

---

### Vehicle Exit

When a vehicle exits:

1. Find the active parking session.
2. Record the exit time.
3. Calculate the parking duration.
4. Calculate the amount due.
5. Record the payment.
6. Verify that payment was successful.
7. Mark the parking session as `COMPLETED`.
8. Change the parking slot status to `AVAILABLE`.
9. Authorize the exit barrier.

Example:

```text
Slot A01
OCCUPIED
     ↓
Vehicle exits
     ↓
Payment successful
     ↓
Slot A01
AVAILABLE
```

---

## 6. Database Schema

The database tables will be implemented using SQL.

The database will contain:

```text
vehicles
    |
    └── parking_sessions
            |
            └── payments

parking_slots
    |
    └── parking_sessions
```

Foreign keys will be used to maintain relationships between the tables and help maintain data integrity.

## 7. Database and C++ System

The C++ application will interact with the SQLite database.

The general relationship will be:

```text
        C++ APPLICATION
               |
               v
       DATABASE MODULE
               |
               v
          SQLite DB
               |
       ┌───────┼────────┐
       ↓       ↓        ↓
   Vehicles  Slots   Sessions
                         |
                         ↓
                      Payments
```

The database module will be responsible for operations such as inserting, retrieving, updating, and deleting records where required.
