# L-System Interpreter

This project implements a **deterministic context-free L-System interpreter** in **C**, developed during my first programming course. It reads a formal grammar (L-System) from input, performs a number of rewriting steps, and then prints an interpreted output based on user-defined interpretation rules.

---

## What is an L-System?

An **L-System** (Lindenmayer System) is a formal grammar used to model the growth of plants and generate complex fractals. A D0L-system is a triple **(S, A, R)** where:

- **S** is the alphabet of symbols  
- **A** is the **axiom** (initial word)  
- **R** is a set of **replacement rules** (each symbol has at most one rule)

Each derivation step replaces every symbol in the word simultaneously using the corresponding rule (or leaves it unchanged if no rule exists). The result after *n* derivation steps is then interpreted symbol by symbol using interpretation rules.

---

## Program Features

- Parses an L-System and performs `n` rewriting steps
- Supports rule-based replacement and symbol interpretation
- Reads **prolog**, **interpretation rules**, and **epilog**
- Symbols without interpretation rules are skipped
- Uses only standard C features and manual memory management

---

## Input Format

Input must follow this structure:

1. A non-negative integer - the number of derivation steps  
2. A line containing the **axiom**  
3. Zero or more **replacement rules** in the format:  
   ```
   <symbol><replacement>
   ```
4. A **blank line**  
5. **Prolog** - any number of lines  
6. A **blank line**  
7. **Interpretation rules**, one per line:  
   ```
   <symbol><text>
   ```
8. A **blank line**  
9. **Epilog** - any number of lines  

---

### Sample Input

```txt
0
xbyw
aacbz
c
xxy
bay

first line of prolog
second line of prolog

xiks
yletter y
b
aletter a

first line of epilog
second line of epilog
third line of epilog
```

---

## How to Compile & Run

```bash
gcc -std=c99 -Wall -o lsystem lsystem.c
./lsystem < input.txt
```

---

## Output

The program will print the following:
- All **prolog** lines
- The interpreted version of the final word (after rewriting)
- All **epilog** lines

Symbols without interpretation rules are skipped.