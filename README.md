# Maze Solver with Dijkstra's Algorithm using B-Tree

## Overview

This C++ program demonstrates a maze-solving algorithm using Dijkstra's algorithm implemented with a B-tree for efficient pathfinding. The program takes a maze as input, finds the shortest path from the specified source to the goal, and prints both the input and solved mazes.

## Features

- **Dijkstra's Algorithm:** Utilizes Dijkstra's algorithm for finding the shortest path in the maze.
- **B-Tree Implementation:** Efficiently manages the priority queue for Dijkstra's algorithm using a B-tree.
- **Input from File:** Reads maze data from a user-specified file.
- **Colorful Output:** Displays the input and solved mazes with color-coded paths.

## Getting Started

1. **Compile the Code:**
   - Save the C++ code in a file, e.g., `pathfinder.cpp`.
   - Compile using a C++ compiler:
     ```bash
     g++ pathfinder.cpp -o pathfinder
     ```

2. **Prepare Maze File:**
   - Create a text file containing the maze. See the provided format in the example.

3. **Run the Executable:**
   - Execute the compiled program:
     ```bash
     ./pathfinder
     ```

4. **Explore the Output:**
   - Observe the input maze, the solving process, and the final solved maze.

## Example Maze File Format

```plaintext
5 5
1 0 1 1 1
1 0 0 0 1
1 1 1 1 1
0 0 0 0 1
1 1 1 1 1
```

## Dependencies

- C++ compiler

## Additional Notes

- Replace "maze.txt" in the code with the correct path to your maze file.
- Adjust the compilation command based on your system and compiler.

Enjoy exploring mazes and discovering the shortest paths! 🌐🚀