#include <stdlib.h>
#include "spmv.h"

CSCMatrix* dense_to_csc(const unsigned int n, const double mat[]) {
    int nnz = 0;
    // Count the number of non-zero values
    for (unsigned int i = 0; i < n * n; i++) {
        if (mat[i] != 0) {
            nnz++;
        }
    }

    CSCMatrix *csc = (CSCMatrix*) malloc(sizeof(CSCMatrix));
    csc->col_ptr = (int*) malloc((n + 1) * sizeof(int));
    csc->row_indices = (int*) malloc(nnz * sizeof(int));
    csc->values = (double*) malloc(nnz * sizeof(double));

    // Fill the col_ptr array
    int idx = 0;
    for (unsigned int j = 0; j < n; j++) { // For each column
        csc->col_ptr[j] = idx;
        for (unsigned int i = 0; i < n; i++) { // For each row
            double val = mat[i * n + j];
            if (val != 0) {
                csc->row_indices[idx] = i;
                csc->values[idx] = val;
                idx++;
            }
        }
    }
    csc->col_ptr[n] = idx;

    return csc;
}

int my_sparse_CSC(const CSCMatrix *csc, const double vec[], double result[], unsigned int n) {
    // Initialize the result vector to zero
    for (unsigned int i = 0; i < n; i++) {
        result[i] = 0.0;
    }

    // Perform the SpMV product
    for (unsigned int j = 0; j < n; j++) { // For each column
        for (int idx = csc->col_ptr[j]; idx < csc->col_ptr[j + 1]; idx++) {
            int row = csc->row_indices[idx];
            double val = csc->values[idx];
            result[row] += val * vec[j];
        }
    }
    return 0;
}

void free_csc(CSCMatrix *csc) {
    free(csc->col_ptr);
    free(csc->row_indices);
    free(csc->values);
    free(csc);

}
