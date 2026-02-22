# Project Context: CramTasker (University Syllabus-Aligned)
**Course:** CSE6781 – Algorithms and Data Structures (15-week curriculum)
**Objective:** Build a scheduling engine that integrates the core topics of the semester.

## 1. Technical Scope (Syllabus Constraints)
Claude, your implementation and advice MUST stay within these specific topics found in the course plan:

### Data Structures (DS)
- **Linear (Week 2):** std::vector, std::list, Stack, Queue.
- **Hashing (Week 5):** Hash-tables (unordered_map) for O(1) lookup of subjects/grades.
- **Trees (Week 6-7):** Binary Search Trees (BST) or AVL for categorized tasks; Heaps (priority_queue) for deadline management.
- **Graphs (Week 8-10):** Adjacency lists for representing time-slot dependencies or free-time gaps.

### Algorithms (ALGO)
- **Sorting/Searching (Week 3-4):** MergeSort/QuickSort for timeline ordering; Binary Search for gap finding.
- **Graph Traversal (Week 8-9):** BFS/DFS for availability analysis; Dijkstra for "shortest path" to task completion.
- **MST (Week 10):** Kruskal/Prim logic for connecting related study modules.
- **Dynamic Programming (Week 11):** Weighted Interval Scheduling (to maximize 'Profit' or GPA).
- **Greedy (Week 12):** Activity Selection (maximum tasks in a window), Fractional Knapsack for partial task completion.

## 2. Complexity Requirements
- Every major function must have a **Big-O complexity comment**.
- Aim for optimal solutions

## 3. Project Architecture (Modular Setup)
Refactor my code into 4 files to demonstrate "Professional Student" level:
1. `task.h`: Data models (`struct Subject`, `struct Task`).
2. `scheduler.h`: Interface for the `CramTasker` engine (within `namespace CramCore`).
3. `scheduler.cpp`: Implementation of logic (Focus on Greedy and DP).
4. `main.cpp`: Entry point showing the integration of the above.

## 4. Academic "Profit" Formula
Calculate task priority using the syllabus-implied logic:
`Priority = Subject.Credits * (100 - Subject.CurrentGrade)`

## 5. Implementation Instructions for Claude:
- Do NOT use advanced C++20/23 features or external AI libraries.
- Use **TODO comments** for weeks 6-15 so I can fill them in as the course progresses.
- Ensure the code follows the "Activity Selection" logic from Week 12 of the syllabus for the core scheduling loop.

---
**Current Task:** Refactor my existing code into this 4-file architecture while maintaining the "CramTasker" name and the described syllabus-to-code mapping.