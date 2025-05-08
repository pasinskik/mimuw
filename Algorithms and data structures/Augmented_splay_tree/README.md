# Dynamic Sequence Manager (Augmented Splay Tree)

This project implements an **augmented splay tree** with lazy propagation to handle range queries and updates on a dynamic sequence of integers. It supports two operations efficiently:
- **Range increment** (`N a b k`): Add `k` to all elements from index `a` to `b`
- **Range query** (`C a b`): Return the length of the **longest non-decreasing consecutive subsequence** in range `[a, b]`

---

## Problem Statement

Start with a sequence of `n` ones. Perform `m` operations of two types:

- `N a b k`: Add `k` to all values from `a` to `b`
- `C a b`: Print the length of the longest non-decreasing consecutive subsequence from `a` to `b`

---

## Key Features

- **Self-balancing splay tree** for fast insertion/access
- **Lazy propagation** for efficient range updates
- **Subtree metadata** for prefix/suffix/non-decreasing subsequence tracking

---

## Input Format

```
n m # n = initial sequence length, m = number of operations
<op_1>
<op_2>
...
<op_m>
```

### Example

```
10 3
N 1 10 5
C 1 10
C 5 7
```

This initializes a sequence of 10 ones, adds 5 to all elements, and queries non-decreasing subsequences.

---

## How to Run
```
g++ -std=c++17 garden.cpp -o garden
./garden < example_input.txt
```