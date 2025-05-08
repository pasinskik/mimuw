# NAND Gate Simulator (Dynamic Library)

This project implements a **dynamically loaded C library** for modeling boolean circuits made of **NAND gates**. It was developed as part of coursework in Computer Architecture and Operating Systems.

The library allows building a network of NAND gates and evaluating outputs, including computing the **critical path** length of a circuit.

---

## Features

- Dynamically allocated NAND gates
- Support for boolean signals and gate-to-gate connections
- Functions for:
  - Connecting and disconnecting inputs/outputs
  - Propagating signals through gates
  - Detecting cycles in the circuit
  - Calculating **output signals** and **critical paths**
- Defensive error handling using `errno`

---

## Example Usage

> **Note**  
> This project was developed against a university-provided framework or test suite that is not included here.  
> Only the **implementation code** is provided to showcase coding style, algorithmic design, and concurrent programming techniques.  
> The goal of this repository is not to provide a runnable application, but rather to demonstrate problem-solving and clean, structured code.

This is a C library intended to be compiled into a `.so` file and dynamically loaded by other C programs.