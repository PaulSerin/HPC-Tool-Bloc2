# SpMV: Sparse Matrix-Vector Product

The branch **`dev`** of this repository is dedicated to implementing and benchmarking various methods for Sparse Matrix-Vector (SpMV) products. It is structured into multiple tasks, each focusing on different aspects of SpMV computation. Each task builds upon the previous one, progressively adding more advanced techniques and optimizations.

The repository is organized into folders labeled by task (e.g., Task1, Task2). To run the code for a specific task, navigate to the desired task folder:

```bash
cd Taskx
```

The main `README` file in the root directory provides all necessary instructions on compiling, running, and understanding the benchmarks across tasks. You can refer to this README for guidance on setup and usage for all tasks in the repository.

## TASK 1

### Basic SpMV

#### Purpose

The goal of this GitHub repository is to implement various methods for performing Matrix-Vector products when the matrices are sparse. Initially, the product is calculated using a simple method, followed by a transformation to Compressed Sparse Row (CSR) format to compare the speed of calculations.

#### Branches

To compile the program, please ensure you are on the `dev` branch, where I have developed the code. The `main` branch contains the original code from the GitHub repository.

To switch to the `dev` branch, run the following command in your terminal:

```bash
git checkout dev
```

#### Compilation

To compile the program, simply use the make command:

```bash
make
```

Upon successful compilation and execution, you can expect an output similar to the following:

![MatrixVectorResults](Task1/MatrixVectorResults.png)

## TASK 2

### SpMV Benchmarking

#### Overview

In Task 2, we extended our work from Task 1 by implementing additional methods for performing Sparse Matrix-Vector (SpMV) products. Specifically, we implemented the **COO (Coordinate Format)** and **CSC (Compressed Sparse Column)** versions of SpMV. This allowed us to compare the performance of four different sparse matrix storage formats:

- **Dense**
- **CSR (Compressed Sparse Row)**
- **COO (Coordinate Format)**
- **CSC (Compressed Sparse Column)**

I organized our code by creating separate `.c` files for each method to keep the implementations modular and maintainable.

#### Compilation and Execution Without Optimization

I created a special `Makefile` to work with the two benchmarking scripts (`runGCC.sh` and `runICC.sh`). However, it's still possible to compile and run the `spmv.c` program without any optimization by executing the following commands in the terminal:

```bash
make clean
make
./spmv
```

This will compile the program and execute it, printing the execution times of the six methods we have programmed.

#### Benchmarking with Different Compilers and Optimizations

I created two shell scripts to automate the benchmarking process with different compilers and optimization flags:

- `runGCC.sh` for the **GCC** compiler
- `runICC.sh` for the **ICC** compiler

These scripts compile and run `spmv.c` with various optimization flags and collect the execution times into a table.

Here is an exemple of results I obtained :

![task2testSpmv](Task2/task2testSpmv.png)

##### Running the Benchmark Scripts

To run the benchmark with GCC, use:

```bash
sbatch runGCC.sh
```

To run the benchmark with ICC, use:

```bash
sbatch runICC.sh
```

You will receive the results in two files: `spmv_benchmark_output_gcc.out` and `spmv_benchmark_output_icc.out`. I have kept my results in these files so that you can see what they look like and the results we obtained during a run for each compiler.

#### Results

We ran each script **five times** and compiled a table summarizing the average execution times (in milliseconds) for each method under different optimization levels.

##### GCC Compiler Benchmark Results

| Function | O0 (ms) | O2-novec (ms) | O3-vec (ms) | Ofast (ms) | Ref (ms) |
| -------- | ------- | ------------- | ----------- | ---------- | -------- |
| my_dense | 1097    | 660           | 1008        | 1048       | 1034     |
| my_CSC   | 152     | 69            | 115         | 116        | 113      |
| my_CSR   | 140     | 65            | 112         | 114        | 112      |
| my_COO   | 170     | 106           | 140         | 142        | 144      |

##### ICC Compiler Benchmark Results

| Function | O0 (ms) | O2-novec (ms) | O3-vec (ms) | Ofast (ms) | Ref (ms) |
| -------- | ------- | ------------- | ----------- | ---------- | -------- |
| my_dense | 892     | 398           | 150         | 158        | 176      |
| my_CSC   | 143     | 34            | 36          | 38         | 36       |
| my_CSR   | 120     | 42            | 39          | 45         | 42       |
| my_COO   | 155     | 89            | 86          | 93         | 85       |

#### Conclusion

From the benchmark results, we can draw several conclusions about the performance of different SpMV implementations and the impact of compiler optimizations:

1. **Compiler Impact**: The **ICC compiler** generally produces faster executables compared to **GCC**, especially at higher optimization levels. For instance, `my_dense` at `O3-vec` with ICC is significantly faster than with GCC.

2. **Optimization Levels**:

   - **No Optimization (`O0`)**: As expected, all methods run slower without any optimization. This effect is more pronounced in the `my_dense` method due to the lack of compiler optimizations.

   - **Disable Vectorization (`O2-novec`)**: Disabling vectorization leads to slower execution times compared to when vectorization is enabled (`O3-vec`), highlighting the importance of vectorization in numerical computations.

   - **Enable Vectorization (`O3-vec`, `Ofast`)**: Enabling vectorization and aggressive optimizations improves performance, particularly for the dense computations. The sparse methods also benefit, but to a lesser extent.

3. **Method Comparison**:

   - **Dense vs. Sparse**: The sparse implementations (`my_CSC`, `my_CSR`, `my_COO`) are significantly faster than the dense implementation (`my_dense`), demonstrating the efficiency of using sparse storage formats for matrices with a high proportion of zero elements.

   - **Sparse Formats**: Among the sparse methods, `my_CSC` and `my_CSR` generally perform better than `my_COO`. This could be due to the overhead associated with the COO format and the efficiency gains from the compressed formats.

4. **Effect of ICC Compiler**: The ICC compiler shows more significant improvements with higher optimization levels, particularly for the dense method. This suggests that ICC's optimization techniques are highly effective for computationally intensive code.

#### Summary

By implementing and benchmarking different sparse matrix storage formats and compiling with various optimization levels using both GCC and ICC compilers, we have demonstrated the importance of choosing the appropriate data structures and compiler optimizations for high-performance numerical computations.

Our results show that:

- **Using sparse matrix formats** can greatly reduce computation time for matrices with many zero elements.
- **Compiler optimizations**, especially vectorization, have a significant impact on performance.
- The **choice of compiler** can affect the execution time, with ICC providing better performance in our tests.

# Task 3: VTune Analysis and Performance Characterization

## Introduction

In this final task, we use Intel VTune Profiler to analyze and characterize the performance of our Sparse Matrix-Vector Multiplication (SpMV) routines. Previously, we implemented multiple sparse formats (CSR, CSC, COO) and assessed their performance under different compiler optimizations and vectorization flags. Now, by leveraging a dedicated profiling tool, we can gain deeper insights into vectorization efficiency, memory and cache behavior, and potential bottlenecks.

The main objectives for this analysis are:

1. **Vectorization Analysis**:  
   Determine if the critical sections of our SpMV routines are being autovectorized by the compiler. If not, understand the reasons and explore ways to aid the compiler in achieving vectorization.

2. **Memory and Cache Behavior**:  
   Evaluate how efficiently our code manages heap usage, and assess the access patterns to understand their impact on cache performance and memory bandwidth. This includes checking for memory leaks, proper allocation sizes, and analyzing both spatial and temporal locality.

Below, we present our findings based on VTune Profiler’s output and discuss the possible optimizations.

---

## Hotspot Analysis

### Key Hotspots

The **Top Hotspots** summary from VTune identifies the most CPU-intensive functions:

<img src="Task3/Images%20Task3/HP_TopHotSpots.png" alt="Top Hotspots" width="50%">

- **`dense_to_csc`** consumes **20.5%** of the CPU time, making it the most time-consuming function.
- **`dense_to_coo`** accounts for **7.8%**, and **`populate_sparse_matrix`** adds another **10.2%** of CPU time.  
  These hotspots indicate that the sparse matrix conversion routines are major bottlenecks due to irregular access patterns and conditional branches.

---

### Dense-to-COO Hotspot

<img src="Task3/Images%20Task3/HP_dense_to_coo.png" alt="Dense to COO Hotspot" width="50%">

The dense-to-COO conversion shows that the loop counting non-zero elements is a hotspot, with ~1.8% CPU time consumed. The irregular memory access pattern and conditional checks prevent vectorization and cause cache inefficiencies.

---

### Dense-to-CSR Hotspot

<img src="Task3/Images%20Task3/HP_dense_to_csr.png" alt="Dense to CSR Hotspot" width="50%">

The `dense_to_csr` function also highlights performance bottlenecks, where the loop counting non-zero elements consumes **1.9% of CPU time**. The memory access pattern here further exacerbates the inefficiencies.

---

### Dense-to-CSC Hotspot

<img src="Task3/Images%20Task3/HP_dense_to_csc.png" alt="Dense to CSC Hotspot" width="50%">

In the dense-to-CSC conversion, **15.5% of CPU time** is spent in a single conditional check. This indicates that irregular access patterns to the input matrix and the conditionals contribute significantly to the performance issues.

---

### Dense SpMV Hotspot

<img src="Task3/Images%20Task3/HP_my_dense.png" alt="Dense SpMV Hotspot" width="50%">

The dense matrix-vector product also shows hotspots, with a significant portion of the CPU time spent in the computation loop. While the memory access here is regular, the computational cost remains high for large matrices.

---

## Summary of Findings

### Key Observations from Hotspot Analysis:

1. **Conversion Functions:**

   - **`dense_to_csc`** and **`dense_to_csr`** are the most CPU-intensive routines, with conditionals and irregular memory accesses causing significant performance bottlenecks.
   - These functions consume **20.5%** and **7.8%** of the total CPU time, respectively.

2. **Conditional Checks:**  
   The presence of conditionals (e.g., `if (val != 0)`) in the sparse matrix conversion routines prevents vectorization and causes inefficient use of CPU resources.

3. **Cache and Memory Access Patterns:**  
   Sparse matrix formats suffer from irregular memory accesses, which reduce cache efficiency and prevent the compiler from applying stride-based optimizations.

4. **Dense vs. Sparse Performance:**
   - Dense SpMV benefits from contiguous memory access patterns but incurs high computational costs for large matrices.
   - Sparse formats optimize computation but are limited by their poor memory and cache utilization.

---

## Recommendations to Optimize Hotspots

1. **Remove Conditional Branches:**  
   Pre-filter the input matrix to identify non-zero elements before performing sparse matrix conversion. This eliminates condition checks in the inner loops.

2. **Reorder Data for Better Cache Utilization:**  
   Sorting non-zero elements or rows/columns in advance can improve spatial locality and enable better cache line usage.

3. **Use Compiler Directives:**  
   Add compiler hints such as `#pragma ivdep` or `#pragma simd` to encourage autovectorization in critical loops.

4. **Parallelize Conversion Routines:**  
   Use OpenMP or another parallel framework to split the sparse matrix conversion workload across multiple threads.

5. **Profile NUMA Behavior:**  
   Ensure that memory allocations are aligned with the NUMA domain of the thread performing the computation. This can reduce remote memory access latencies.

---

## Conclusion

The VTune analysis identifies key performance bottlenecks in our SpMV code:

1. **Conversion routines (`dense_to_csc`, `dense_to_csr`)** dominate CPU usage due to conditionals and irregular memory access patterns.
2. **Cache inefficiencies** and **lack of vectorization** limit the performance of both dense and sparse routines.
3. Optimizations such as **conditional removal**, **data reordering**, and **parallelization** are recommended to address these bottlenecks.

By implementing these improvements, we can significantly reduce CPU time and enhance the overall efficiency of the SpMV kernels.

## Vectorization Analysis

### Autovectorization Status

![Performance Analysis](Task3/Images%20Task3/PERF_1.png)

**Observation:**  
From the VTune analysis, the percentage of packed floating-point operations vectorized is relatively low (~13.3%). Critical loops within our SpMV kernel do not show significant vectorization gains. For instance, the main computation loop of the COO-based SpMV:

```c
// Example snippet for COO SpMV kernel
for (int i = 0; i < coo->nnz; i++) {
    int row = coo->row_indices[i];
    int col = coo->col_indices[i];
    double val = coo->values[i];
    result[row] += val * vec[col];
}
```

According to the profiler, these loops are not heavily vectorized. The presence of indirect indexing (`coo->row_indices[i]`, `coo->col_indices[i]`) and the sparse data layout complicates the compiler’s ability to generate SIMD instructions. Additionally, irregular access patterns and condition checks (e.g., `if (val != 0)` in the CSC or CSR creation routines) impede autovectorization.

![Performance Analysis2](Task3/Images%20Task3/PERF_2.png)

### Why Autovectorization is Limited:

- **Irregular Memory Access:**  
  Sparse formats rely on indirect indexing to find non-zero elements. This disrupts the compiler’s ability to generate predictable, stride-based vector loads and stores.

- **Conditionals in the Hot Loop:**  
  Checks such as `if (val != 0)` within the innermost loops break the linearity and prevent effective vectorization.

- **Mixed Memory Layouts:**  
  Non-contiguous memory access patterns reduce the compiler’s confidence in safe vector transformations.

### Potential Approaches to Improve Vectorization

**Data Reordering and Padding:**  
Reordering sparse data to improve contiguity (e.g., sorting indices and grouping non-zeros by rows or columns more tightly) can help the compiler vectorize load/store operations.

**Removing Conditional Checks Inside Inner Loops:**  
Pre-filtering non-zero elements before entering the main computational loop could eliminate conditional branches. For example, constructing the sparse data structure so that only non-zero elements are accessed at runtime.

**Explicit Vector Intrinsics or Compiler Directives:**  
Using compiler hints such as `#pragma ivdep` or `#pragma simd` may encourage autovectorization. For more control, manually implementing vector instructions using intrinsics could yield better performance, though it reduces portability.

---

## Memory and Cache Behavior

### Heap Usage and Allocation

![Performance MEM](Task3/Images%20Task3/MEM_summary.png)

**Observation:**  
VTune’s memory analysis did not indicate memory leaks or excessive allocation overhead. The code allocates arrays for indices and values only once per run. Heap management appears reasonable, with proper allocation and deallocation at program termination.

![Performance MEM](Task3/Images%20Task3/MEM_detail.png)

### Access Patterns and Cache Efficiency

**Spatial and Temporal Locality:**

- **Sparse Formats (CSC, CSR, COO):**  
  By definition, sparse formats often suffer from poor spatial locality because the data for non-zero elements is distributed irregularly. For instance, in CSC format, data is stored column-by-column, but the vector is accessed in a way that might not align perfectly with cache lines. This can lead to suboptimal use of caches.

- **Dense vs. Sparse Access:**  
  The dense version, although simpler, benefits from more predictable and contiguous memory accesses, resulting in better spatial locality. However, because the matrix is large and sparse, performing computations on zero values wastes compute cycles and bandwidth.

**Memory Bound and NUMA Effects:**  
The VTune summary indicated a moderate memory bound scenario (~16.4% of pipeline slots) and a high percentage of remote NUMA accesses (over 90%). This suggests that data placement might not be NUMA-aware, causing higher latency when accessing memory not local to the executing core. Ensuring memory allocations are closer to the NUMA domain where the thread is running, or using first-touch memory policies, could reduce memory access latencies.

### Implications for Cache Performance

- **Low Effective Cache Reuse:**  
  Since sparse matrices are read mostly once per SpMV operation, temporal locality is limited. Each non-zero entry is typically accessed once, multiplied, and then not reused immediately. This inherently reduces the potential benefits of caching.

- **Instruction vs. Data Bound:**  
  While the computations are fairly simple (just multiplications and additions), the main bottleneck often becomes memory bandwidth. The time spent fetching scattered elements from memory dwarfs the time spent executing the floating-point instructions. Improving data layout to be more cache-friendly can mitigate this issue.

---

## Summary of Findings

- **Vectorization:**  
  The relevant parts of our SpMV code are not fully autovectorized due to irregular memory access patterns and conditional checks inside loops. To improve vectorization, consider reordering data, removing conditionals in tight loops, and possibly using compiler directives or intrinsics.

- **Memory and Cache Behavior:**  
  While memory management is correct (no leaks observed), the code is somewhat memory-bound, with irregular accesses impacting performance. The sparse formats provide faster execution than dense representations for large sparse matrices, but they inherently limit the compiler’s ability to optimize for cache locality and vectorization.

- **NUMA and Cache Optimizations:**  
  The high percentage of remote NUMA accesses suggests an opportunity to optimize data placement. This could involve ensuring that the thread performing the computation touches the data first or using NUMA-aware allocators, potentially reducing latency.

---

## Conclusion

VTune profiling reveals that while our sparse kernels benefit from reduced computations on zero elements, their performance is largely constrained by memory access patterns and limited vectorization opportunities. To achieve further performance gains, we need to:

- Improve data layouts for better memory locality and vectorization.
- Consider compiler hints or intrinsics to encourage SIMD usage.
- Explore NUMA optimizations and memory allocation strategies to reduce remote memory access penalties.
