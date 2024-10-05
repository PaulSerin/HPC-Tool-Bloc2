#ifndef SPMV_H
#define SPMV_H

int my_dense(const unsigned int n, const double mat[], double vec[], double result[]);

typedef struct {
    int *row_ptr;     // Pointer to the start of each row
    int *col_indices; // Column indices of the non-zero values
    double *values;   // Non-zero values of the matrix
} CSRMatrix;

CSRMatrix* dense_to_csr(const unsigned int n, const double mat[]);

int my_sparse(const CSRMatrix *csr, const double vec[], double result[], unsigned int n);

void free_csr(CSRMatrix *csr);

#endif
