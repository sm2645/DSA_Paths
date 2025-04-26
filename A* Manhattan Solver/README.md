# A* Sliding Puzzle Solver

## Overview

This project implements an **A\*** algorithm-based solver for the general **k×k sliding puzzle** problem (also known as the (k²−1)-puzzle).  
The idea originated from an earlier **Breadth-First Search (BFS)** sliding puzzle project and was expanded independently using A\* to optimize performance.

The A* algorithm improves upon BFS by prioritizing search paths, dramatically reducing search time, especially for larger boards.

---

## How It Works

- **A\*** (A-star) search algorithm is used to find the shortest path to the goal.
- Each move is selected based on minimizing the cost function:
  - **f(n) = g(n) + h(n)**  
    - `g(n)`: cost to reach the current board state
    - `h(n)`: estimated cost to reach the goal (heuristic)
- **Manhattan Distance** is used as the heuristic function.
- A custom **min-heap priority queue** manages search states efficiently.
- A **hash table** prevents revisiting duplicate board states.

---

## Features

- Supports arbitrary board sizes (e.g., 3×3, 4×4, etc.)
- Early detection of unsolvable puzzles
- Optimized memory and search performance
- Clear output showing either the sequence of moves or reporting no solution

---

## Input Format

The program expects an input file structured as follows:
- **Line 1**: Comment (ignored)
- **Line 2**: Integer `k` — the board size
- **Line 3**: Comment (ignored)
- **Following lines**: `k × k` integers representing the board tiles (row-major order, 0 = empty space)

### Example Input:
```
# Board size
3
# Initial board
1 2 3
4 0 6
7 5 8
```

---

## Output Format

The program writes the solution to an output file:
- If solvable:  
  - Outputs `#moves` followed by the sequence of tile numbers moved
- If unsolvable:  
  - Outputs `#moves` and `no solution`

### Example Output (Solvable):
```
#moves
6 5 8
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
gcc -o astar_solver main.c
```

### To Run:
```bash
./astar_solver input.txt output.txt
```
- `input.txt` — path to the puzzle input file
- `output.txt` — path to the output file where results will be written

---

## Requirements

- C Compiler (e.g., `gcc`)
- Standard libraries only (no external dependencies)
- Compatible with Linux environments (tested on **Tux** servers)

---


---

