# Maximum Domino Tiling (Bitmask DP)

This project implements a recursive bitmask dynamic programming algorithm to solve the **Maximum Domino Tiling** problem on a grid with integer values. The goal is to cover the grid using 2×1 dominoes in such a way that each cell is covered at most once, and the total sum of covered cell values is maximized.

---

## Problem Statement

Given an `k × n` grid (with `k` rows and `n` columns), each cell contains an integer value (positive, negative, or zero). You can place **2×1 dominos**, either horizontally or vertically, to cover the grid. A cell’s value contributes to the total sum **only if it is covered by a domino**.

---

## How It Works

- The algorithm uses **bitmask DP** to track tile placements column by column.
- Recursive exploration of valid tilings with pruning based on optimality.
- Memoization ensures efficient reuse of overlapping subproblems.

---

## Input Format

```c
n k # n = number of columns, k = number of rows  
v11 v12 ... v1n # Row 1 values  
v21 v22 ... v2n # Row 2 values  
...  
vk1 vk2 ... vkn # Row k values  
```
### Example

```c
3 2
1 2 3
4 5 6
```

This represents a 2x3 grid:

1 2 3  
4 5 6

---

## How to Run

```bash
g++ -std=c++17 domino.cpp -o domino  
./domino < example_input.txt
```