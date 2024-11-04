#include <stdlib.h>
#include "spmv.h"

// Function to perform the SpMV for a CSR matrix
int my_sparse(const CSRMatrix *csr, const double vec[], double result[], unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        result[i] = 0.0;
        for (int j = csr->row_ptr[i]; j < csr->row_ptr[i + 1]; j++) {
            result[i] += csr->values[j] * vec[csr->col_indices[j]];
        }
    }
    return 0;
}

// Function to convert dense matrix to CSR format
CSRMatrix* dense_to_csr(const unsigned int n, const double mat[]) {
    CSRMatrix *csr = (CSRMatrix*) malloc(sizeof(CSRMatrix));
    csr->row_ptr = (int*) malloc((n + 1) * sizeof(int));
    int nnz = 0; // Count of non-zero elements

    // Count non-zero elements and fill the row pointer
    for (unsigned int i = 0; i < n; i++) {
        csr->row_ptr[i] = nnz; // Start of the row
        for (unsigned int j = 0; j < n; j++) {
            if (mat[i * n + j] != 0) {
                nnz++;
            }
        }
    }
    csr->row_ptr[n] = nnz; // End of the last row

    csr->values = (double*) malloc(nnz * sizeof(double));
    csr->col_indices = (int*) malloc(nnz * sizeof(int));

    nnz = 0; // Reset non-zero count for filling values and column indices
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n; j++) {
            if (mat[i * n + j] != 0) {
                csr->values[nnz] = mat[i * n + j];
                csr->col_indices[nnz] = j;
                nnz++;
            }
        }
    }

    return csr;
}

void free_csr(CSRMatrix *csr) {
    free(csr->values);
    free(csr->row_ptr);
    free(csr->col_indices);
    free(csr);
}
