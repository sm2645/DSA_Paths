# Sliding Puzzle Solver (BFS Approach)

## Overview

This project implements a **Breadth-First Search (BFS)** based solver for the general **k×k sliding puzzle** (also known as the (k²−1)-puzzle).  
The idea was originally developed for an academic project focused on Breadth-First Search, and was later expanded independently to improve runtime efficiency and deepen understanding of graph traversal techniques.

The solver finds the minimum sequence of moves needed to rearrange a shuffled board into the goal configuration.

---

## How It Works

- **Breadth-First Search (BFS)** is used to explore possible board configurations level-by-level.
- Each board state keeps track of:
  - The current layout of the board.
  - A reference to the previous board state.
  - The tile number that was moved to reach the new state.
- A custom **linked list queue** is used to implement the BFS traversal.
- A **hash table** efficiently tracks previously visited board states to prevent cycles and redundant processing.
- The goal state is defined as the sorted order with the empty space (0) at the bottom right corner.

---

## Features

- Supports arbitrary board sizes (e.g., 3×3, 4×4, up to 10×10).
- Early detection and reporting of unsolvable puzzles.
- Efficient memory management using a lightweight graph representation.
- Outputs the sequence of moves necessary to reach the goal, or reports "no solution" if unsolvable.

---

## Input Format

The program expects an input file structured as follows:
- **Line 1**: Comment (ignored)
- **Line 2**: Integer `k` — the board size
- **Line 3**: Comment (ignored)
- **Following lines**: `k × k` integers representing the board tiles in row-major order (0 represents the empty space)

### Example Input:
```
#k
4
#initial state
1 2 3 4 5 6 7 8 9 10 15 11 13 14 0 12
```

---

## Output Format

The program writes the solution to an output file:
- If solvable:
  - Outputs `#moves` followed by the sequence of tile numbers moved.
- If unsolvable:
  - Outputs `#moves` followed by `no solution`.

### Example Output (Solvable):
```
#moves
15 11 12
```

### Example Output (Unsolvable):
```
#moves
no solution
```

---

## Compilation & Running

### To Compile:
```bash
gcc -o solve main.c
```

### To Run:
```bash
./solve input.txt output.txt
```
- `input.txt` — path to the puzzle input file
- `output.txt` — path to the file where results will be written

---

## Requirements

- C Compiler (e.g., `gcc`)
- Standard C libraries only
- Compatible with Linux environments

---

## Future Improvements

- Implement memory optimizations for extremely large boards.
- Explore alternative search algorithms such as Iterative Deepening BFS or Bidirectional BFS.
- Compare BFS solver performance to heuristic-based solvers like A*.