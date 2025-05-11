# Peer-to-Peer Clock Synchronization System

This project implements a **distributed clock synchronization protocol** between multiple nodes communicating over UDP. The goal is to maintain a synchronized notion of time across a network of peers without relying on a central server.

---

## Problem Statement

Each node starts independently with its own clock and a list of known peers. Nodes exchange synchronization messages to:
- Detect and connect to other peers
- Elect a leader (level 0)
- Synchronize clocks recursively via pairwise offset measurements
- Propagate synchronized time using a hierarchical level structure

The system is designed to:
- **Broadcast synchronization attempts** periodically
- **Adjust local clock offsets** based on measured delay and timestamps
- **Detect leader failures** and desynchronize as necessary

---

## Message Types

The protocol supports the following messages:
- `HELLO`, `HELLO_REPLY`: Discover and share peer lists
- `CONNECT`, `ACK_CONNECT`: Establish connections between peers
- `SYNC_START`, `DELAY_REQUEST`, `DELAY_RESPONSE`: Perform time offset calculation
- `LEADER`: Leader announcement
- `GET_TIME`, `TIME`: Query and respond with synchronized time

---

## Key Features

- **UDP-based communication** with custom protocol
- **Pairwise offset calculation** using 4-timestamp method
- **Peer list management** and validation of message origin
- **Leader election logic** and desynchronization on timeout
- **Internal monotonic timekeeping** for accurate comparisons

---

## How to Build

This project uses a **Makefile** for building.

### Build the project
```bash
cd code_files
make
```
This will create the executable:
```
peer-time-sync
```

---
## How to Run
```bash
./peer-time-sync -p <port> -b <bind_ip> [-a <peer_ip> -r <peer_port>]
```
### Options
- -p	UDP port to bind the local node
- -b	Local IP to bind to (e.g., 0.0.0.0)
- -a	IP of initial peer to connect to
- -r	Port of initial peer to connect to

Example:
```bash
./peer-time-sync -p 5001 -b 0.0.0.0 -a 127.0.0.1 -r 5000
```