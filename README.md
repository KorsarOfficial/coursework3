# Coursework 3 — Sorting Algorithms Comparison

C++ implementation comparing classic sorting algorithms with measured comparison counts and wall-clock benchmarks.

## Algorithms Implemented

| Algorithm | Average | Worst | Space | Stable |
|---|---|---|---|---|
| Quicksort (Lomuto) | O(n log n) | O(n^2) | O(log n) | No |
| Mergesort | O(n log n) | O(n log n) | O(n) | Yes |
| Counting Sort | O(n + k) | O(n + k) | O(k) | Yes |

## Build & Run

```bash
g++ -std=c++17 -O2 -o coursework3 main.cpp
./coursework3
```

## Description

- Sorts an array of 25 random integers using quicksort, mergesort, and counting sort
- Counts exact number of comparisons for comparison-based sorts
- Benchmarks O(n log n) vs O(n + k) on 1,000,000 elements
- Demonstrates when linear-time sorting outperforms comparison sorts
