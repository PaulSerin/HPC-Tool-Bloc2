#!/bin/bash
#SBATCH --nodes=1               # Number of nodes
#SBATCH --mem=16G               # Total memory per node
#SBATCH --time=01:00:00         # Total run time limit (HH:MM:SS)
#SBATCH --job-name=spmv_benchmark_gcc
#SBATCH --output=spmv_benchmark_output_gcc.out

# Load the GCC module
module purge
module load gcc

# List of optimization levels and corresponding CFLAGS for GCC
optimization_levels=("O0" "O2-novec" "O3-vec" "Ofast" "Ref")
cflags_list=("-O0" "-O2 -fno-tree-vectorize" "-O3 -ftree-vectorize" "-Ofast" "-O2")

# Functions to benchmark (as they appear in the output)
functions=("your dense" "your CSC" "your CSR" "your COO")

# Map function names to identifiers for the table
declare -A func_names
func_names["your dense"]="my_dense"
func_names["your CSC"]="my_CSC"
func_names["your CSR"]="my_CSR"
func_names["your COO"]="my_COO"

# Declare associative array to store times
declare -A times

# Matrix size and density for the benchmark
# MATRIX_SIZE=4096  # Adjust as needed
# DENSITY=10        # Density in percentage

# Loop over optimization levels
for i in "${!optimization_levels[@]}"; do
    opt_level="${optimization_levels[$i]}"
    cflags="${cflags_list[$i]}"

    echo "==========================================="
    echo "Compiling and running with GCC optimization: $opt_level"
    echo "CFLAGS: $cflags"
    echo "==========================================="

    # Clean previous builds
    make clean

    # Compile with the given compiler and optimization flags
    make CC=gcc CFLAGS="-Wall -Wextra $cflags"

    if [ $? -ne 0 ]; then
        echo "Compilation failed with GCC CFLAGS: $cflags"
        exit 1
    fi

    # Run the program and capture the output
    output=$(./spmv $MATRIX_SIZE $DENSITY 2>&1)

    # Check if the program ran successfully
    if [ $? -ne 0 ]; then
        echo "Execution failed for GCC optimization level $opt_level"
        echo "$output"
        continue
    fi

    # Display the output for debugging purposes
    echo "$output"

    # Extract execution times from the output
    for func in "${functions[@]}"; do
        pattern="Execution time ($func):"
        # Extract the execution time for the current function
        time_line=$(echo "$output" | grep "$pattern")
        time_ms=$(echo "$time_line" | awk '{print $(NF-1)}')

        # Store the time in the associative array
        times["$opt_level,${func_names[$func]}"]=$time_ms
    done
done

# Print the results in a table
echo
echo "Benchmark Results for GCC"
echo "========================="
printf "%-10s" "Function"
for opt_level in "${optimization_levels[@]}"; do
    printf "%15s" "$opt_level"
done
echo

for func_key in "${functions[@]}"; do
    func="${func_names[$func_key]}"
    printf "%-10s" "$func"
    for opt_level in "${optimization_levels[@]}"; do
        time="${times["$opt_level,$func"]}"
        if [ -z "$time" ]; then
            time="N/A"
        fi
        printf "%15s" "$time"
    done
    echo
done
