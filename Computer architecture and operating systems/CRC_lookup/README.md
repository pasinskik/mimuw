# CRC Calculation Using Lookup Table (x86-64 Assembly)

This project implements a **CRC (Cyclic Redundancy Check)** calculation tool in x86-64 Assembly. It reads binary data from a file and computes the CRC using a user-specified binary polynomial. The algorithm is optimized using a **lookup table**, enabling byte-wise processing instead of bit-wise.

---

## Project summary

- **Input:** A binary file and a binary representation of a polynomial (e.g. `"100000111"`)
- **Output:** A CRC checksum printed to stdout as a binary string (`'0'`/`'1'` format)
- **Language:** x86-64 NASM Assembly (Linux syscalls only, no C runtime)
- **Optimization:** Uses a 256-entry lookup table for fast CRC evaluation

---

## Input Format

The program is invoked with exactly two command-line arguments:

```bash
./crc <filename> <binary_polynomial>
```
Example:
```bash
./crc example.data 100000
```
---

## Output

The CRC checksum is printed to stdout in binary format (with a newline at the end), e.g.:

```bash
1011100011001011011000100011011011001000100000110101110101110101
```
Length of the output matches the degree of the input polynomial.