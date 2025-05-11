# Pętlik Language Interpreter & Optimizing Compiler

This project implements an interpreter and optimizing compiler for a minimalist custom programming language called **Pętlik**, developed as a university assignment. The language operates on 26 named variables (`a` to `z`) and supports two instructions: increment and loops.

## Language Overview

**Grammar:**

```
InstructionSequence
InstructionSequence → ε | InstructionSequence Instruction
Instruction → Increment | Repeat
Increment → Variable
Repeat → '(' Variable InstructionSequence ')'
Variable → 'a' | ... | 'z'
```

### Semantics

- `a` through `z` represent non-negative integer variables (initially `0`).
- `a` is equivalent to `++a;`
- `(a...)` is equivalent to:
  ```c
  while (a > 0) {
      --a;
      ...
  }
  ```

### Optimization

The compiler generates optimized VM instructions for non-nested Repeat blocks where the loop variable is not reused inside the block.

Example optimized output for (a bc):

```
ADD b a  
ADD c a  
CLR a
```

### Virtual Machine Instructions

- `INC x` – Increment variable `x`

- `ADD x y` – Add value of `y` to `x`

- `CLR x` – Clear variable `x` (set to `0`)

- `DJZ x addr` – If `x == 0`, jump to `addr`; else decrement `x`

- `JMP addr` – Jump unconditionally

- `HLT` – Halt execution

---

## Features

- Full parsing and interpretation of the Pętlik language

- Optimized code generation for simple repeat blocks

- Manual handling of arbitrary-length integers (as character arrays)

- Preserves variable state across multiple program executions

- Supports value inspection through custom commands

---

## Input Format
Each line represents one command:

- `=x` - Print the current value of variable `x`
- Any other valid Pętlik code - interpreted and executed immediately

### Example Input
```
=a
aaaaa
=a
(a(a(ab)))(ba)
=a
=b
(abb)(bcc)
=a
=b
=c
(c)
=c
```

---

## Output Format

Only values printed using `=x` commands appear in `stdout`, each on its own line.

---

## How to Compile & Run
```bash
gcc -std=c11 petlik.c -o petlik
./petlik < input.txt
```