# Smart Parking Management System - Algorithms

## Algorithm 1: Parking Slot Management

### Purpose

Maintain the status of parking slots and determine whether a slot is available or occupied.

### Pseudocode

```text
START

FOR each parking slot
    IF slot is empty
        Mark slot as AVAILABLE
    ELSE
        Mark slot as OCCUPIED
    ENDIF
ENDFOR

Display current slot status

STOP
```

---

## Algorithm 2: Display Available Slots

### Purpose

Show drivers the available parking spaces before entry.

### Pseudocode

```text
START

Count available parking slots

Display:
    Number of available slots
    Number of occupied slots

STOP
```

---

## Algorithm 3: Vehicle Entry

### Purpose

Register arriving vehicles and assign parking slots.

### Pseudocode

```text
START

Receive vehicle registration number

Check for available slots

IF slot available THEN
    Assign slot to vehicle
    Record vehicle details
    Record entry time
    Mark slot as OCCUPIED
    Open entry barrier
ELSE
    Display "Parking Full"
ENDIF

STOP
```

---

## Algorithm 4: Vehicle Session Management

### Purpose

Track vehicles currently parked.

### Pseudocode

```text
START

Store:
    Vehicle registration number
    Assigned slot
    Entry time

Maintain active parking sessions

STOP
```

---

## Algorithm 5: Vehicle Exit

### Purpose

Process departing vehicles.

### Pseudocode

```text
START

Receive vehicle registration number

Search vehicle record

IF vehicle found THEN
    Record exit time
    Proceed to duration calculation
ELSE
    Display "Vehicle Not Found"
ENDIF

STOP
```

---

## Algorithm 6: Parking Duration Calculation

### Purpose

Calculate the time spent in parking.

### Pseudocode

```text
START

Retrieve entry time
Retrieve exit time

Duration = Exit Time - Entry Time

Display duration

STOP
```

---

## Algorithm 7: Fee Calculation

### Purpose

Calculate parking charges.

### Pseudocode

```text
START

Receive parking duration

Fee = Duration × Hourly Rate

Display amount payable

STOP
```

---

## Algorithm 8: Payment Processing

### Purpose

Verify payment.

### Pseudocode

```text
START

Receive payment

IF payment >= required amount THEN
    Payment successful
ELSE
    Payment failed
ENDIF

STOP
```

---

## Algorithm 9: Barrier Control

### Purpose

Allow exit after successful payment.

### Pseudocode

```text
START

Check payment status

IF payment successful THEN
    Open barrier
    Mark slot as AVAILABLE
ELSE
    Keep barrier closed
ENDIF

STOP
```
