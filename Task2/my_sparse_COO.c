#include <stdlib.h>
#include "spmv.h"

COOMatrix* dense_to_coo(const unsigned int n, const double mat[]) {
    int nnz = 0;
    // Count the number of non-zero values
    for (unsigned int i = 0; i < n * n; i++) {
        if (mat[i] != 0) {
            nnz++;
        }
    }

    COOMatrix *coo = (COOMatrix*) malloc(sizeof(COOMatrix));
    coo->nnz = nnz;
    coo->row_indices = (int*) malloc(nnz * sizeof(int));
    coo->col_indices = (int*) malloc(nnz * sizeof(int));
    coo->values = (double*) malloc(nnz * sizeof(double));

    // Fill the COO arrays
    int idx = 0;
    for (unsigned int i = 0; i < n; i++) { // Row index
        for (unsigned int j = 0; j < n; j++) { // Column index
            double val = mat[i * n + j];
            if (val != 0) {
                coo->row_indices[idx] = i;
                coo->col_indices[idx] = j;
                coo->values[idx] = val;
                idx++;
            }
        }
    }

    return coo;
}

int my_sparse_COO(const COOMatrix *coo, const double vec[], double result[], unsigned int n) {
    // Initialize the result vector to zero
    for (unsigned int i = 0; i < n; i++) {
        result[i] = 0.0;
    }

    // Perform the SpMV product
    for (int i = 0; i < coo->nnz; i++) {
        int row = coo->row_indices[i];
        int col = coo->col_indices[i];
        double val = coo->values[i];
        result[row] += val * vec[col];
    }

    return 0;
}

void free_coo(COOMatrix *coo) {
    free(coo->row_indices);
    free(coo->col_indices);
    free(coo->values);
    free(coo);
}
