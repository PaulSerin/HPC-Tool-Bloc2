#ifndef SPMV_H
#define SPMV_H

int my_dense(const unsigned int n, const double mat[], double vec[], double result[]);

// Definition for the CSR format
typedef struct {
    int *row_ptr;     // Pointers to the start of each row
    int *col_indices; // Column indices of the non-zero values
    double *values;   // Non-zero values of the matrix
} CSRMatrix;

CSRMatrix* dense_to_csr(const unsigned int n, const double mat[]);
int my_sparse_CSR(const CSRMatrix *csr, const double vec[], double result[], unsigned int n);
void free_csr(CSRMatrix *csr);

// Definition for the COO format
typedef struct {
    int nnz;          // Number of non-zero values
    int *row_indices; // Row indices of the non-zero values
    int *col_indices; // Column indices of the non-zero values
    double *values;   // Non-zero values of the matrix
} COOMatrix;

COOMatrix* dense_to_coo(const unsigned int n, const double mat[]);
int my_sparse_COO(const COOMatrix *coo, const double vec[], double result[], unsigned int n);
void free_coo(COOMatrix *coo);

// Definition for the CSC format
typedef struct {
    int *col_ptr;     // Pointers to the start of each column
    int *row_indices; // Row indices of the non-zero values
    double *values;   // Non-zero values of the matrix
} CSCMatrix;

CSCMatrix* dense_to_csc(const unsigned int n, const double mat[]);
int my_sparse_CSC(const CSCMatrix *csc, const double vec[], double result[], unsigned int n);
void free_csc(CSCMatrix *csc);

#endif
