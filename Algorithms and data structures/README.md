# Algorithms and Data Structures

This folder contains two algorithmic projects written in **C++**, developed as part of my university coursework. Each project showcases advanced problem-solving, optimization, and data structure implementation skills.

---

## Projects

### 1. **Maximum Domino Tiling (Bitmask Dynamic Programming)**

**Description:**  
Given a grid with integer values in each cell, this algorithm finds the maximum possible sum achievable by placing 2x1 dominos so that each tile is covered exactly once. Only the values of covered cells are counted toward the sum.

- **Key Idea:** Recursive dynamic programming with bitmasking and memoization.  
- **Complexity:** `O(n × k × 2^k)`  
- **Language:** C++

**Features:**
- Bitmasking for tile placement tracking
- Recursive tiling state exploration
- Efficient memoized DP implementation

> Try it with a custom matrix input to see the optimal tiling value in action.

---

### 2. **Augmented Splay Tree (Dynamic Sequence Management)**

**Description:**  
Implements an augmented **Splay Tree** to store a sequence of integers. Supports efficient range updates and queries on non-decreasing subsequences. Designed for an interactive "garden" simulation problem.

- **Key Idea:** Splay tree with lazy propagation and subtree metadata.  
- **Complexity:** `O(m × log n)`  
- **Language:** C++

**Supports:**
- Range increment operations (lazy propagation)
- Finding max non-decreasing consecutive subsequence in a given range
- Online splaying and subtree attribute maintenance

> Handles large sequences efficiently with smart tree rotations and augmentations.

---

## How to Run

Both projects read from `stdin`, so you can compile and run them as follows:

```bash
g++ -std=c++17 domino.cpp -o domino
./domino < input.txt

g++ -std=c++17 garden.cpp -o garden
./garden < input.txt
