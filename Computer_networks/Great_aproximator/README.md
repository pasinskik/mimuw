# The Great Aproximator

This project implements a **client-server game** in which players submit approximations of a secret linear function. Clients try to guess the best values at specific points, competing for the highest score.

---

## Problem Statement

The server hides a linear function `f(x) = a0 + a1*x + ... + ak*x^k` (coefficients are unknown to clients).

Players connect to the server, receive the list of coefficients, and then submit their approximations of `f(x)` at integer points. Each client can:

- Send multiple `PUT` commands to submit guesses
- Receive server responses indicating state, penalties, or score
- Compete with others in real time

The game ends when a timeout is reached or a scoring condition is met.

---

## Protocol Overview

The communication protocol is text-based and line-delimited using `\r\n`. The following messages are supported:

### From Client to Server

- `HELLO <playerId>` – Initial identification
- `PUT <point> <value>` – Approximation submission

### From Server to Client

- `COEFF <a0> <a1> ... <ak>` – Sent once after HELLO
- `STATE <r0> <r1> ... <rk>` – Periodic broadcast of current best approximations
- `BAD_PUT <point> <value>` – Submitted value is too far off
- `PENALTY <point> <value>` – Server rejects PUT due to previous penalty
- `SCORING <score1> <score2> ...>` – Final scores and game over

---

## Key Features

- **Interactive and automatic modes** for clients
- **Full support for buffering and partial input** (e.g. fragmented messages)
- **Robust readLine implementation** supporting `\r\n` line endings
- **Non-blocking I/O** with `select()` for stdin and sockets
- **Server streaming COEFF values from a file** (lazy loading supported)

---

## How to Build

Use the provided `Makefile`:

```bash
make
```

This will build two executables:

- `approx-server`
- `approx-client`

---

## How to Run

### Server

```bash
./approx-server -p <port> -k <degree> -c <coeffs_file>
```

- `-p` – TCP port to listen on
- `-k` – Degree of the polynomial (0 <= k <= 8)
- `-c` – Path to file with COEFF lines (one line per game)

Example:

```bash
./approx-server -p 4000 -k 3 -c coeffs.txt
```

### Client

```bash
./approx-client -i <playerId> -h <server_ip> -p <port> [-a]
```

- `-i` – Player ID (alphanumeric)
- `-h` – Server IP address
- `-p` – Server port
- `-a` – (Optional) Automatic mode using built-in strategy

Example:

```bash
./approx-client -i alice123 -h 127.0.0.1 -p 4000
```

---

## Manual Mode

In manual mode, players can type lines like:

```
3 42.0
7 -3.14
```

Each line is interpreted as `PUT <point> <value>`.

The client prints server responses, including feedback about guesses and the current state.

---

## Auto Mode

In automatic mode (`-a`), the client uses a greedy strategy to guess points with the highest potential score, updating as new state messages arrive.

---

## Notes

- All messages end with `\r\n` and partial reads are fully supported.
- Multiple clients can connect simultaneously.
- The server rejects malformed messages and logs errors.

---

## License

MIT License. Created as part of the Computer Networks lab assignment (Wielki Aproksymator).

