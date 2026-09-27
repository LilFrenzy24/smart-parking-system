# 🚗 Smart Parking Management System

A full-stack Smart Parking System built with C++ for low-level backend operations, SQLite3 for persistent data management, and a Python Flask web dashboard integrated with the Safaricom Daraja M-Pesa API for automated checkout and STK Push mobile payments.

---

## 🛠️ Tech Stack & Features

- **Core Engine:** C++ (Object-Oriented Architecture, SQLite C API integration).
- **Web Server & REST API:** Python 3, Flask, `requests`.
- **Database:** SQLite3 (`database/parking.db`).
- **Payment Gateway:** Safaricom Daraja M-Pesa API (STK Push / Lipa Na M-Pesa Online).
- **Frontend:** Responsive HTML5, CSS Grid/Flexbox, and asynchronous JavaScript (`fetch` API).

---

## ⚡ Prerequisites & Setup

### 1. Install System Dependencies & Libraries
```bash
sudo apt update
sudo apt install -y g++ sqlite3 libsqlite3-dev python3 python3-pip
pip install flask requests

### 2. Initialize the SQLite Database
```bash
sqlite3 database/parking.db < database/schema.sql

## How to Use
Option 1: Web Dashboard With M-Pesa Payment Gateway (Recommended)

1. Launch the Flask Server:
```bash
python3 app.py

2. Access the Dashboard: Open your browser and go to: http://127.0.0.1:5000

### Note:  You are not paying anything, press cancel when prompted.

Option 2: C++ Core Terminal Interface (CLI Engine)

1. Compile the Executable: 
```bash 
g++ src/main.cpp src/parking.cpp src/vehicle.cpp src/session.cpp src/payment.cpp src/database.cpp -lsqlite3 -o parking_system

2. Run the Binary:
 ```bash
./parking_system

3. CLI Options:

1: Park a Vehicle

2: View Parking Slot Status

3: Checkout Vehicle & Calculate Fees

4: View Active & Past Parking Records

5: Exit Program