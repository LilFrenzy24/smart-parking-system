import base64
from datetime import datetime
import math
import os
import sqlite3
from flask import Flask, render_template_string, request, jsonify
import requests

app = Flask(__name__)
DB_PATH = "database/parking.db"

# ---------------------------------------------------------
# SAFARICOM DARAJA SANDBOX CREDENTIALS
# ---------------------------------------------------------
CONSUMER_KEY = "r6l96lR0zbvg44xzDuA6Kz2LUMb3QbaAms9Y5s5yg2qLQKTY"
CONSUMER_SECRET = "sOvmFklAelCH3YqIm0EZWReGPnGY7xI8bPVOlXll5h3WE7GoxFwbWctZ1qsAlsDk"
BUSINESS_SHORTCODE = "174379"  # Standard Daraja Sandbox Shortcode
PASSKEY = "bfb279f9aa9bdbcf158e97dd71a467cd2e0c893059b10f78e6b72ada1ed2c919"
CALLBACK_URL = "https://sandbox.safaricom.co.ke/mpesa/"

def get_db_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

def get_mpesa_access_token():
    url = "https://sandbox.safaricom.co.ke/oauth/v1/generate?grant_type=client_credentials"
    try:
        response = requests.get(url, auth=(CONSUMER_KEY, CONSUMER_SECRET))
        if response.status_code == 200:
            return response.json().get("access_token")
    except Exception as e:
        print(f"Error fetching access token: {e}")
    return None

# ---------------------------------------------------------
# API ROUTES
# ---------------------------------------------------------

@app.route("/")
def home():
    return render_template_string(HTML_TEMPLATE)

@app.route("/api/slots", methods=["GET"])
def get_slots():
    conn = get_db_connection()
    slots = conn.execute("SELECT * FROM parking_slots ORDER BY slot_id ASC").fetchall()
    conn.close()
    return jsonify([dict(slot) for slot in slots])

@app.route("/api/active-sessions", methods=["GET"])
def get_active_sessions():
    conn = get_db_connection()
    query = """
        SELECT 
            ps.session_id,
            v.registration_number,
            v.vehicle_type,
            slot.slot_number,
            ps.entry_time
        FROM parking_sessions ps
        JOIN vehicles v ON ps.vehicle_id = v.vehicle_id
        JOIN parking_slots slot ON ps.slot_id = slot.slot_id
        WHERE ps.status = 'ACTIVE'
        ORDER BY slot.slot_id ASC
    """
    sessions = conn.execute(query).fetchall()
    conn.close()
    return jsonify([dict(s) for s in sessions])

@app.route("/api/park", methods=["POST"])
def park_vehicle():
    data = request.json
    reg_no = data.get("registration_number", "").strip().upper()
    v_type = data.get("vehicle_type", "Saloon").strip()

    if not reg_no:
        return jsonify({"success": False, "message": "Registration number is required"}), 400

    conn = get_db_connection()
    cursor = conn.cursor()

    active_session = cursor.execute("""
        SELECT ps.session_id FROM parking_sessions ps
        JOIN vehicles v ON ps.vehicle_id = v.vehicle_id
        WHERE v.registration_number = ? AND ps.status = 'ACTIVE'
    """, (reg_no,)).fetchone()

    if active_session:
        conn.close()
        return jsonify({"success": False, "message": f"Vehicle {reg_no} is ALREADY parked!"}), 400

    slot = cursor.execute("SELECT * FROM parking_slots WHERE status = 'AVAILABLE' ORDER BY slot_id ASC LIMIT 1").fetchone()
    if not slot:
        conn.close()
        return jsonify({"success": False, "message": "Parking Lot is FULL!"}), 400

    cursor.execute("INSERT OR IGNORE INTO vehicles (registration_number, vehicle_type) VALUES (?, ?)", (reg_no, v_type))
    v_id = cursor.execute("SELECT vehicle_id FROM vehicles WHERE registration_number = ?", (reg_no,)).fetchone()["vehicle_id"]

    cursor.execute("INSERT INTO parking_sessions (vehicle_id, slot_id, status) VALUES (?, ?, 'ACTIVE')", (v_id, slot["slot_id"]))
    cursor.execute("UPDATE parking_slots SET status = 'OCCUPIED' WHERE slot_id = ?", (slot["slot_id"],))

    conn.commit()
    conn.close()

    return jsonify({"success": True, "message": f"SUCCESS: Vehicle {reg_no} parked in Slot {slot['slot_number']}"})

@app.route("/api/checkout", methods=["POST"])
def checkout_vehicle():
    data = request.json
    reg_no = data.get("registration_number", "").strip().upper()

    if not reg_no:
        return jsonify({"success": False, "message": "Registration number is required"}), 400

    conn = get_db_connection()
    cursor = conn.cursor()

    session = cursor.execute("""
        SELECT ps.session_id, ps.entry_time, ps.slot_id, slot.slot_number 
        FROM parking_sessions ps
        JOIN vehicles v ON ps.vehicle_id = v.vehicle_id
        JOIN parking_slots slot ON ps.slot_id = slot.slot_id
        WHERE v.registration_number = ? AND ps.status = 'ACTIVE'
    """, (reg_no,)).fetchone()

    if not session:
        conn.close()
        return jsonify({"success": False, "message": f"No active parking session found for {reg_no}"}), 404

    # Calculate duration
    entry_time = datetime.strptime(session["entry_time"], "%Y-%m-%d %H:%M:%S")
    now = datetime.now()
    duration_seconds = max(0, (now - entry_time).total_seconds())
    duration_minutes = duration_seconds / 60.0
    duration_hours = duration_seconds / 3600.0

    # Pricing Structure:
    # - Base Fee: KSh 50 (covers up to the first 30 minutes)
    # - KSh 100 per additional hour (pro-rated or ceiling-based after 30 mins)
    base_fee = 50.0
    
    if duration_minutes <= 30:
        total_fee = base_fee
    else:
        # Bill KSh 100 for every additional hour beyond the initial 30 mins
        additional_hours = math.ceil((duration_minutes - 30) / 60.0)
        total_fee = base_fee + (additional_hours * 100.0)

    total_fee = int(total_fee)

    # Complete session and release slot
    cursor.execute("""
        UPDATE parking_sessions 
        SET exit_time = CURRENT_TIMESTAMP, duration_minutes = ?, amount_due = ?, status = 'COMPLETED'
        WHERE session_id = ?
    """, (duration_minutes, total_fee, session["session_id"]))

    cursor.execute("UPDATE parking_slots SET status = 'AVAILABLE' WHERE slot_id = ?", (session["slot_id"],))

    conn.commit()
    conn.close()

    return jsonify({
        "success": True,
        "registration_number": reg_no,
        "amount": total_fee,
        "duration_sec": int(duration_seconds),
        "message": f"Checkout calculated for {reg_no}: Parked for {int(duration_seconds)}s. Total Fee: KSh {total_fee}"
    })

@app.route("/api/stkpush", methods=["POST"])
def trigger_stk_push():
    data = request.json
    raw_phone = data.get("phone_number", "").strip()
    amount = int(data.get("amount", 0))
    reg_no = data.get("registration_number", "").strip().upper()

    if not raw_phone or not reg_no or amount <= 0:
        return jsonify({"success": False, "message": "Phone number, Reg No, and valid amount required"}), 400

    phone = raw_phone
    if phone.startswith("0"):
        phone = "254" + phone[1:]
    elif phone.startswith("+"):
        phone = phone[1:]

    access_token = get_mpesa_access_token()
    if not access_token:
        return jsonify({"success": False, "message": "Failed to authenticate with Safaricom API"}), 500

    timestamp = datetime.now().strftime("%Y%m%d%H%M%S")
    password_str = f"{BUSINESS_SHORTCODE}{PASSKEY}{timestamp}"
    password = base64.b64encode(password_str.encode()).decode("utf-8")

    headers = {
        "Authorization": f"Bearer {access_token}",
        "Content-Type": "application/json"
    }

    payload = {
        "BusinessShortCode": BUSINESS_SHORTCODE,
        "Password": password,
        "Timestamp": timestamp,
        "TransactionType": "CustomerPayBillOnline",
        "Amount": amount,
        "PartyA": phone,
        "PartyB": BUSINESS_SHORTCODE,
        "PhoneNumber": phone,
        "CallBackURL": CALLBACK_URL,
        "AccountReference": reg_no,
        "TransactionDesc": "Parking Fee Payment"
    }

    url = "https://sandbox.safaricom.co.ke/mpesa/stkpush/v1/processrequest"

    try:
        res = requests.post(url, json=payload, headers=headers)
        res_data = res.json()
        print("\n--- SAFARICOM DARAJA RESPONSE ---")
        print(res_data)
        print("---------------------------------\n")

        if res_data.get("ResponseCode") == "0":
            return jsonify({
                "success": True, 
                "message": f"SUCCESS! STK Push sent to {phone}. Enter M-Pesa PIN."
            })
        else:
            err_msg = res_data.get("errorMessage", "Failed to trigger STK Push")
            return jsonify({"success": False, "message": f"Safaricom Error: {err_msg}"}), 400

    except Exception as e:
        print(f"STK Push Exception: {e}")
        return jsonify({"success": False, "message": f"Server Error: {str(e)}"}), 500

# ---------------------------------------------------------
# FRONTEND UI
# ---------------------------------------------------------
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Parking Management System</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f172a; color: #f8fafc; margin: 0; padding: 20px; }
        h1 { text-align: center; color: #38bdf8; margin-bottom: 25px; }
        .container { max-width: 1100px; margin: 0 auto; display: grid; grid-template-columns: 1fr 1fr; gap: 20px; }
        .card { background: #1e293b; padding: 22px; border-radius: 12px; box-shadow: 0 4px 6px -1px rgba(0,0,0,0.5); }
        .grid-slots { display: grid; grid-template-columns: repeat(5, 1fr); gap: 10px; margin-top: 15px; }
        .slot-btn { padding: 15px; border-radius: 8px; text-align: center; font-weight: bold; border: none; font-size: 0.95rem; }
        .AVAILABLE { background: #16a34a; color: #fff; }
        .OCCUPIED { background: #dc2626; color: #fff; }
        input { width: 100%; padding: 11px; margin: 8px 0; border-radius: 6px; border: 1px solid #334155; background: #0f172a; color: #fff; box-sizing: border-box; }
        input:read-only { background: #1e293b; color: #94a3b8; cursor: not-allowed; }
        button.btn-primary { width: 100%; padding: 11px; margin-top: 6px; border-radius: 6px; background: #0284c7; color: #fff; font-weight: bold; border: none; cursor: pointer; }
        button.btn-primary:hover { background: #0369a1; }
        button.btn-checkout { background: #d97706; }
        button.btn-checkout:hover { background: #b45309; }
        button.btn-mpesa { background: #16a34a; }
        button.btn-mpesa:hover { background: #15803d; }
        .log-box { background: #090d16; padding: 12px; border-radius: 6px; font-family: monospace; color: #4ade80; min-height: 60px; word-wrap: break-word; }
        .checkout-section { display: none; background: #0f172a; padding: 15px; border-radius: 8px; border: 1px solid #334155; margin-top: 15px; }
        
        /* Sessions Table Styling */
        table { width: 100%; border-collapse: collapse; margin-top: 15px; font-size: 0.9rem; }
        th, td { text-align: left; padding: 10px; border-bottom: 1px solid #334155; }
        th { background-color: #0f172a; color: #38bdf8; }
        tr:hover { background-color: #334155; }
        .badge { padding: 4px 8px; border-radius: 4px; font-weight: bold; background: #0284c7; color: #fff; font-size: 0.8rem; }
    </style>
</head>
<body>
    <h1>🚗 Smart Parking Real-Time Dashboard</h1>
    <div class="container">
        <!-- Live Slots Grid & Active Vehicles Table -->
        <div class="card">
            <h2>Live Parking Slots Status</h2>
            <p style="color: #94a3b8; font-size: 0.9rem;">Real-time slot updates from SQLite database:</p>
            <div id="slotsGrid" class="grid-slots"></div>

            <hr style="border-color: #334155; margin: 25px 0;">

            <h2>Currently Parked Vehicles</h2>
            <div style="overflow-x: auto;">
                <table>
                    <thead>
                        <tr>
                            <th>Slot</th>
                            <th>Reg No</th>
                            <th>Type</th>
                            <th>Entry Time</th>
                        </tr>
                    </thead>
                    <tbody id="sessionsTableBody">
                        <tr><td colspan="4" style="text-align: center; color: #94a3b8;">No vehicles currently parked</td></tr>
                    </tbody>
                </table>
            </div>
        </div>

        <!-- Controls -->
        <div class="card">
            <h2>1. Vehicle Entry</h2>
            <input type="text" id="regNo" placeholder="Vehicle Reg No (e.g., KDA 123A)">
            <input type="text" id="vType" placeholder="Vehicle Type (e.g., Saloon)">
            <button class="btn-primary" onclick="parkVehicle()">Park Vehicle</button>

            <hr style="border-color: #334155; margin: 20px 0;">

            <h2>2. Vehicle Checkout & Fee Calculation</h2>
            <input type="text" id="outRegNo" placeholder="Enter Vehicle Reg No to Checkout">
            <button class="btn-primary btn-checkout" onclick="checkoutVehicle()">Calculate Fee & Checkout</button>

            <!-- Payment Prompt -->
            <div id="checkoutBox" class="checkout-section">
                <h3 style="margin-top: 0; color: #38bdf8;">3. Pay via M-Pesa</h3>
                <label style="font-size: 0.85rem; color: #94a3b8;">Automated Parking Fee (KSh):</label>
                <input type="number" id="payAmount" readonly>
                <label style="font-size: 0.85rem; color: #94a3b8;">Customer Phone Number:</label>
                <input type="text" id="phoneNo" placeholder="M-Pesa Phone (2547XXXXXXXX)">
                <button class="btn-primary btn-mpesa" onclick="triggerMpesa()">Send M-Pesa Prompt</button>
            </div>

            <h3 style="margin-top: 20px;">System Activity Log</h3>
            <div id="statusLog" class="log-box">System initialized and ready...</div>
        </div>
    </div>

    <script>
        async function fetchSlots() {
            const res = await fetch('/api/slots');
            const slots = await res.json();
            const grid = document.getElementById('slotsGrid');
            grid.innerHTML = '';
            slots.forEach(slot => {
                const div = document.createElement('div');
                div.className = `slot-btn ${slot.status}`;
                div.innerHTML = `${slot.slot_number}<br><small>${slot.status}</small>`;
                grid.appendChild(div);
            });
        }

        async function fetchActiveSessions() {
            const res = await fetch('/api/active-sessions');
            const sessions = await res.json();
            const tbody = document.getElementById('sessionsTableBody');
            tbody.innerHTML = '';

            if (sessions.length === 0) {
                tbody.innerHTML = '<tr><td colspan="4" style="text-align: center; color: #94a3b8;">No vehicles currently parked</td></tr>';
                return;
            }

            sessions.forEach(session => {
                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td><span class="badge">${session.slot_number}</span></td>
                    <td><strong>${session.registration_number}</strong></td>
                    <td>${session.vehicle_type}</td>
                    <td><small>${session.entry_time}</small></td>
                `;
                tbody.appendChild(tr);
            });
        }

        async function parkVehicle() {
            const regNoInput = document.getElementById('regNo');
            const vTypeInput = document.getElementById('vType');
            
            const res = await fetch('/api/park', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ registration_number: regNoInput.value, vehicle_type: vTypeInput.value })
            });
            const data = await res.json();
            document.getElementById('statusLog').innerText = data.message;
            
            if (data.success) {
                regNoInput.value = '';
                vTypeInput.value = '';
                refreshDashboard();
            }
        }

        async function checkoutVehicle() {
            const outRegNo = document.getElementById('outRegNo').value;
            const res = await fetch('/api/checkout', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ registration_number: outRegNo })
            });
            const data = await res.json();
            document.getElementById('statusLog').innerText = data.message;

            if (data.success) {
                document.getElementById('payAmount').value = data.amount;
                document.getElementById('checkoutBox').style.display = 'block';
                refreshDashboard();
            }
        }

        async function triggerMpesa() {
            const phone = document.getElementById('phoneNo').value;
            const regNo = document.getElementById('outRegNo').value;
            const amount = document.getElementById('payAmount').value;

            const res = await fetch('/api/stkpush', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ phone_number: phone, registration_number: regNo, amount: amount })
            });
            const data = await res.json();
            document.getElementById('statusLog').innerText = data.message;
        }

        function refreshDashboard() {
            fetchSlots();
            fetchActiveSessions();
        }

        refreshDashboard();
        setInterval(refreshDashboard, 3000);
    </script>
</body>
</html>
"""

if __name__ == '__main__':
    app.run(debug=True, port=5000)