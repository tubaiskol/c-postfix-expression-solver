# C Postfix Expression Solver

This project implements an algorithm in ANSI C to determine all possible values of unknown characters in a postfix expression.

The unknown characters may represent either:
- An operator (+, -, *, /)
- A positive integer operand

The program analyzes all possible combinations and prints valid solutions that satisfy the given equation.

---

## 📌 Problem Description

Given a postfix expression containing unknown variables (e.g., a, b, c), determine all valid assignments such that:

- The postfix expression is structurally valid
- All intermediate results are integers
- The final result satisfies the equation (e.g., `= 0`)

Example input:

3 1 + 2 3 2 ^ a 2 / 4 3 - b - c * 2 4 1 + ^ 3 4 * 2 2 * - / 6 2 * - + = 0


The program outputs all valid combinations for `(a, b, c)`.

---

## 🧠 Algorithm Overview

The solution follows these steps:

1. **Token Classification**
   - Detect operators, operands, unknown variables, and equality sign.

2. **Expression Simplification**
   - Perform immediate postfix reductions where possible.

3. **Binary Mapping of Unknowns**
   - Each unknown can be either:
     - Operator
     - Operand
   - Total combinations: 2^n

4. **Postfix Validity Check**
   - Simulate stack height to ensure valid postfix structure.

5. **Expression Tree Construction**
   - Build a binary expression tree using stack logic.

6. **Recursive Evaluation**
   - Try all possible values for unknown operands (1–100)
   - Try all possible operators (+, -, *, /)
   - Ensure integer-only operations

7. **Print Valid Solutions**
   - Only combinations that evaluate correctly are printed.

---

## 🏗 Data Structures Used

- Struct-based token representation
- Binary expression tree
- Stack simulation for postfix validation
- Recursive backtracking

---

## ▶ Compilation & Execution

Compile:

```bash
gcc -o solver 150123051prj1.c -lm

