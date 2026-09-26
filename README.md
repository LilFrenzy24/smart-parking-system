# Smart Parking Management System (C++ & SQLite)

A modular, object-oriented C++ application integrated with an SQLite relational database to manage parking space allocations, vehicle registrations, active parking sessions, duration tracking, and automated fee processing.

---

## Key Features

* **Vehicle Management:** Track vehicle registration numbers and types using high-performance $O(1)$ lookup hash maps (`std::unordered_map`).
* **Parking Slot Allocation:** Assign physical parking spaces (`A01`–`A10`) and update statuses (`AVAILABLE` / `OCCUPIED`) in real-time.
* **Session & Fee Management:** Log exact entry and exit timestamps using C++ `<ctime>`, calculate elapsed durations, and generate itemized billing receipts.
* **Database Persistence:** Save all vehicles, active/completed sessions, slot updates, and payment logs permanently to an SQLite database (`parking.db`).
* **Interactive CLI Interface:** Menu-driven operator interface for smooth parking entry, checkout, and live database queries.

---

## Project Structure

```text
smart-parking-system/
├── database/
│   ├── schema.sql         # Relational database schema & initial seed data
│   └── parking.db         # Persistent SQLite database file
├── include/
│   ├── parking.h          # Physical slot manager header
│   ├── vehicle.h          # Vehicle tracking manager header
│   ├── session.h          # Parking session manager header
│   ├── payment.h          # Fee calculation & payment manager header
│   └── database.h         # SQLite DatabaseManager wrapper header
├── src/
│   ├── parking.cpp        # Slot allocation implementation
│   ├── vehicle.cpp        # Vehicle registration implementation
│   ├── session.cpp        # Session duration tracking implementation
│   ├── payment.cpp        # Payment processing implementation
│   ├── database.cpp       # sqlite3 C API interface implementation
│   └── main.cpp           # Interactive CLI menu & workflow controller
├── README.md              # Project documentation
└── parking_system         # Compiled binary executable

## How to use: Compilation and Usage

Compile all C++ modules linking the sqlite3 C library:

run: g++ src/main.cpp src/parking.cpp src/vehicle.cpp src/session.cpp src/payment.cpp src/database.cpp -lsqlite3 -o parking_system

Launch the application:

run: ./parking_system