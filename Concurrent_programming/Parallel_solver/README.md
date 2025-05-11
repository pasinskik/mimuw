# Concurrent Boolean Expression Tree Solver

This project implements a **parallel solver** for boolean expressions represented as trees. It was developed for the **Concurrent Programming course (2024/2025)**.

Each node in the tree is a logical operation (e.g., `AND`, `OR`, `IF`, `GT`, `LT`, `NOT`) or a `LEAF` node containing a boolean value. The tree is evaluated **concurrently** using an `ExecutorService`, with intelligent early termination to minimize computation time.

---

## Project Summary

> **Note**  
> This project was developed against a university-provided framework or test suite that is not included here.  
> Only the **implementation code** is provided to showcase coding style, algorithmic design, and concurrent programming techniques.  
> The goal of this repository is not to provide a runnable application, but rather to demonstrate problem-solving and clean, structured code.

- Language: **Java**
- Main components:
  - `ParallelCircuitSolver`: manages thread pool and lifecycle
  - `ParallelCircuitValue`: performs the actual evaluation in parallel

---

## Key Concepts

- **Recursive tree evaluation** with concurrency
- **ExecutorCompletionService** for prioritizing fast tasks
- **Early termination** of branches if the final result can be deduced
- **AtomicBoolean** for cooperative cancellation
- **Threshold logic** (GT, LT) with `AtomicInteger` counters

---

## Class Overview

### `ParallelCircuitSolver implements CircuitSolver`
- Entrypoint for solving a given circuit.
- Creates a `ParallelCircuitValue` for each new task.
- Manages shared `ExecutorService` and cancellation flag.

### `ParallelCircuitValue implements CircuitValue`
- Submits the root node to the executor.
- Each node is processed recursively, potentially in parallel.
- Logic nodes like `AND`, `OR`, `IF`, `GT`, `LT` use optimized concurrent logic.