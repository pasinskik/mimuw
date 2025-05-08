# Computer Architecture & Operating Systems Projects

This folder contains three low-level programming projects developed as part of coursework in **Computer Architecture and Operating Systems**. These projects emphasize direct memory management, bitwise operations, assembly-level logic, and system call usage.

---

## Project 1 – Dynamic NAND Gate Library (C)

A dynamically loaded C library that models combinational boolean circuits built entirely from **NAND gates**.

### Features
- Dynamically allocates and links gates in memory
- Supports connecting gates and boolean signals
- Evaluates circuit outputs and **critical path lengths**
- Handles cyclic dependency detection

### Usage
Designed to be compiled as a shared object and loaded from a C test driver. The library exposes functions for gate creation, wiring, evaluation, and teardown.

---

## Project 2 – Signed Multi-Word Division (x86-64 Assembly)

An implementation of **signed multi-word division** in pure x86-64 Assembly. Divides an arbitrary-length (base-2⁶⁴) integer by a 64-bit signed divisor and returns the remainder.

### Features
- Handles two’s complement representation
- Performs quotient sign correction and remainder adjustment
- Checks for overflow cases like `INT64_MIN / -1`
- Follows C-like semantics and register conventions

### Signature
```c
int64_t mdiv(int64_t *x, size_t n, int64_t y);
```
The quotient is written in-place to x, and the signed remainder is returned in RAX.

---

## Project 3 - CRC Calculation with Lookup Table (x86-64 Assembly)

An optimized CRC calculator in assembly that processes a fragmented binary file using a user-defined binary polynomial and a precomputed lookup table.

### Features
- Reads and parses binary files with custom fragmentation format
- Constructs CRC polynomial from binary string input
- Precomputes CRC values for each byte (256-entry lookup table)
- Efficient byte-by-byte processing using bitmasking and system calls
- Final CRC printed as a binary string

### Input Format

The program is invoked with exactly two command-line arguments:

```bash
./crc <filename> <binary_polynomial>
```

### Output

The CRC checksum is printed to stdout in binary format (with a newline at the end).