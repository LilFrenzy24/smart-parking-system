-- Smart Parking Management System Schema (SQLite)

-- Enable Foreign Key constraints in SQLite
PRAGMA foreign_keys = ON;

-- 1. Vehicles Table
CREATE TABLE IF NOT EXISTS vehicles (
    vehicle_id INTEGER PRIMARY KEY AUTOINCREMENT,
    registration_number TEXT NOT NULL UNIQUE,
    vehicle_type TEXT NOT NULL
);

-- 2. Parking Slots Table
CREATE TABLE IF NOT EXISTS parking_slots (
    slot_id INTEGER PRIMARY KEY AUTOINCREMENT,
    slot_number TEXT NOT NULL UNIQUE,
    status TEXT NOT NULL CHECK (status IN ('AVAILABLE', 'OCCUPIED')) DEFAULT 'AVAILABLE'
);

-- 3. Parking Sessions Table
CREATE TABLE IF NOT EXISTS parking_sessions (
    session_id INTEGER PRIMARY KEY AUTOINCREMENT,
    vehicle_id INTEGER NOT NULL,
    slot_id INTEGER NOT NULL,
    entry_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    exit_time DATETIME,
    duration_minutes INTEGER,
    amount_due REAL DEFAULT 0.0,
    status TEXT NOT NULL CHECK (status IN ('ACTIVE', 'COMPLETED')) DEFAULT 'ACTIVE',
    FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id) ON DELETE CASCADE,
    FOREIGN KEY (slot_id) REFERENCES parking_slots(slot_id) ON DELETE CASCADE
);

-- 4. Payments Table
CREATE TABLE IF NOT EXISTS payments (
    payment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    session_id INTEGER NOT NULL UNIQUE,
    amount_paid REAL NOT NULL,
    payment_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    payment_status TEXT NOT NULL CHECK (payment_status IN ('SUCCESSFUL', 'FAILED')) DEFAULT 'SUCCESSFUL',
    FOREIGN KEY (session_id) REFERENCES parking_sessions(session_id) ON DELETE CASCADE
);

-- Seed Initial Parking Slots (A01 to A10)
INSERT OR IGNORE INTO parking_slots (slot_number, status) VALUES
('A01', 'AVAILABLE'),
('A02', 'AVAILABLE'),
('A03', 'AVAILABLE'),
('A04', 'AVAILABLE'),
('A05', 'AVAILABLE'),
('A06', 'AVAILABLE'),
('A07', 'AVAILABLE'),
('A08', 'AVAILABLE'),
('A09', 'AVAILABLE'),
('A10', 'AVAILABLE');