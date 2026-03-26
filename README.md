

# 🗳️ Secure Remote Voting System (Nepal Edition)
**A 1st Semester Project | Computer Engineering | Pulchowk Campus, IOE**

This system was developed as part of our first-semester project to demonstrate 
foundational C programming, file-based data structures, and security implementation.

![C Programming](https://img.shields.io/badge/Language-C-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Security](https://img.shields.io/badge/Security-Hashed%20Auth-red.svg)

A robust, console-based Election Management System designed for the unique constituency-based electoral structure of Nepal. This project focuses on data integrity, administrative security, and user-centric voting flows.

---

## 📖 Project Overview
This system digitizes the manual voting process. It handles voter verification, prevents double-voting, manages regional candidate lists, and generates real-time election analytics. By using file-based persistence, all data survives system restarts.

### 🛠️ Core Technology Stack
- **Language:** C (C99 Standard)
- **Security:** DJB2 Hashing Algorithm for Admin Authentication.
- **Data Storage:** Flat-file database system (`.txt` for records, `.dat` for security hashes).
- **Environment:** Windows Console (utilizes `conio.h` and `windows.h` for UX).

---

## 🚀 Key Features

### 1. Advanced Security
- **Master Key Hashing:** Unlike basic projects, the admin password is never stored as plain text. It is hashed and stored in a binary `.dat` file.
- **Masked Input:** PIN entry uses character masking (`****`) to prevent shoulder-surfing.
- **Audit Logging:** Every login attempt and successful vote is timestamped and recorded in `log.txt`.

### 2. Intelligent Voting Logic
- **Constituency Filtering:** Voters only see candidates running in their specific district (e.g., a voter from *Dang-2* cannot vote for a candidate in *Kaski-2*).
- **One-Vote Constraint:** A strict boolean flag (`voteflag`) prevents any Voter ID from being used twice.

### 3. Comprehensive Analytics
- **Live Voter Turnout:** Calculates the percentage of the registered population that has participated.
- **Automated Winner Declaration:** Scans all constituencies to identify winners or detect **Ties**, where re-election is required.

---

## 📂 File Architecture

| File | Description |
| :--- | :--- |
| `main.c` | The primary application containing the login and voting engine. |
| `key.c` | A standalone security utility to generate the hashed `admin.dat`. |
| `reset.c` | A maintenance script to clear all votes and flags for a new session. |
| `voters.txt` | The citizen database (ID, PIN, Name, DOB, Area, Status). |
| `candidates.txt` | The candidate registry (Name, Constituency, Vote Count). |
| `log.txt` | System audit trail for security monitoring. |

---

## ⚙️ Installation & Usage

### Prerequisites
- A C compiler (GCC/MinGW recommended).
- Windows OS (for `conio.h` support).

### Setup Instructions
1. **Initialize Security:**
   Compile and run `key.c`. This creates the encrypted `admin.dat` file.
   ```bash
   gcc key.c -o key
   ./key
