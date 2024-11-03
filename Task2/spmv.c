#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_cblas.h>      // CBLAS in GSL (the GNU Scientific Library)
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_spblas.h>
#include "timer.h"
#include "spmv.h"

#define DEFAULT_SIZE 16384
#define DEFAULT_DENSITY 0.10

unsigned int populate_sparse_matrix(double mat[], unsigned int n, double density, unsigned int seed)
{
    unsigned int nnz = 0;

    srand(seed);

    for (unsigned int i = 0; i < n * n; i++) {
        if ((rand() % 100) / 100.0 < density) {
            // Get a pseudorandom value between -9.99 and 9.99
            mat[i] = ((double)(rand() % 10) + (double)rand() / RAND_MAX) * ((rand() % 2 == 0) ? 1 : -1);
            nnz++;
        } else {
            mat[i] = 0;
        }
    }

    return nnz;
}

unsigned int populate_vector(double vec[], unsigned int size, unsigned int seed)
{
    srand(seed);

    for (unsigned int i = 0; i < size; i++) {
        vec[i] = ((double)(rand() % 10) + (double)rand() / RAND_MAX) * ((rand() % 2 == 0) ? 1 : -1);
    }

    return size;
}

int is_nearly_equal(double x, double y)
{
    const double epsilon = 1e-5; /* some small number */
    return fabs(x - y) <= epsilon * fabs(x);
    // see Knuth section 4.2.2 pages 217-218
}

unsigned int check_result(double ref[], double result[], unsigned int size)
{
    for(unsigned int i = 0; i < size; i++) {
        if (!is_nearly_equal(ref[i], result[i]))
            return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    unsigned int size;        // number of rows and cols (size x size matrix)
    double density;           // approx. ratio of non-zero values

    if (argc < 2) {
        size = DEFAULT_SIZE;
        density = DEFAULT_DENSITY;
    } else if (argc < 3) {
        size = atoi(argv[1]);
        density = DEFAULT_DENSITY;
    } else {
        size = atoi(argv[1]);
        density = (double) atoi(argv[2]) / 100.0;
    }

    double *mat, *vec, *refsol, *mysol;

    mat = (double *) malloc(size * size * sizeof(double));
    vec = (double *) malloc(size * sizeof(double));
    refsol = (double *) malloc(size * sizeof(double));
    mysol = (double *) malloc(size * sizeof(double));

    unsigned int nnz = populate_sparse_matrix(mat, size, density, 1);
    populate_vector(vec, size, 2);

    printf("Matrix size: %d x %d (%d elements)\n", size, size, size*size);
    printf("%d non-zero elements (%.2lf%% density)\n\n", nnz, (double) nnz / (size*size) * 100.0);

    timeinfo start, now;

    /********** Method 1: Dense computation with CBLAS **********/
    printf("==================================================\n");
    printf("Method 1: Dense computation with CBLAS\n");
    printf("--------------------------------------------------\n");

    timestamp(&start);
    cblas_dgemv(CblasRowMajor, CblasNoTrans, size, size, 1.0, mat, size, vec, 1, 0.0, refsol, 1);
    timestamp(&now);
    printf("Execution time (CBLAS dense): %ld ms\n", diff_milli(&start, &now));
    printf("==================================================\n\n");

    /********** Method 2: Your own dense implementation **********/
    printf("==================================================\n");
    printf("Method 2: My dense implementation\n");
    printf("--------------------------------------------------\n");

    timestamp(&start);
    my_dense(size, mat, vec, mysol);
    timestamp(&now);
    printf("Execution time (your dense): %ld ms\n", diff_milli(&start, &now));

    if (check_result(refsol, mysol, size))
        printf("Result is correct.\n");
    else
        printf("Result is incorrect!\n");
    printf("==================================================\n\n");

    /********** Method 3: Sparse computation with GSL (CSR) **********/
    printf("==================================================\n");
    printf("Method 3: Sparse computation with GSL (CSR)\n");
    printf("--------------------------------------------------\n");

    // Convert the dense matrix 'mat' to a sparse format (CSR)
    gsl_spmatrix *gsl_csr = gsl_spmatrix_alloc(size, size);

    // Fill GSL CSR matrix with non-zero values from the dense matrix
    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            double val = mat[i * size + j];
            if (val != 0) {
                gsl_spmatrix_set(gsl_csr, i, j, val);
            }
        }
    }

    gsl_vector *gsl_vec = gsl_vector_alloc(size);
    gsl_vector *gsl_result = gsl_vector_alloc(size);

    // Initialize GSL vector with values from 'vec'
    for (unsigned int i = 0; i < size; i++) {
        gsl_vector_set(gsl_vec, i, vec[i]);
    }

    // Measure time for GSL SpMV operation
    timestamp(&start);
    gsl_spblas_dgemv(CblasNoTrans, 1.0, gsl_csr, gsl_vec, 0.0, gsl_result);
    timestamp(&now);
    printf("Execution time (GSL sparse): %ld ms\n", diff_milli(&start, &now));

    // Copy result from GSL vector to 'mysol' for comparison
    for (unsigned int i = 0; i < size; i++) {
        mysol[i] = gsl_vector_get(gsl_result, i);
    }

    // Compare GSL result with the reference solution
    if (check_result(refsol, mysol, size)) {
        printf("Result is correct.\n");
    } else {
        printf("Result is incorrect!\n");
    }
    printf("==================================================\n\n");

  
    /********** Method 4: Your own sparse implementation (CSC) **********/
    printf("==================================================\n");
    printf("Method 4: My sparse implementation (CSC)\n");
    printf("--------------------------------------------------\n");

    // Convert the dense matrix 'mat' to CSC format
    CSCMatrix *csc = dense_to_csc(size, mat);

    // Perform SpMV with custom implementation
    timestamp(&start);
    my_sparse_CSC(csc, vec, mysol, size);
    timestamp(&now);
    printf("Execution time (your CSC): %ld ms\n", diff_milli(&start, &now));

    // Compare custom SpMV result with the reference solution
    if (check_result(refsol, mysol, size)) {
        printf("Result is correct.\n");
    } else {
        printf("Result is incorrect!\n");
    }
    printf("==================================================\n\n");

        /********** Method 5: Your own sparse implementation (CSR) **********/
    printf("==================================================\n");
    printf("Method 5: My sparse implementation (CSR)\n");
    printf("--------------------------------------------------\n");

    // Convert the dense matrix 'mat' to CSR format
    CSRMatrix *csr = dense_to_csr(size, mat);

    // Perform SpMV with custom implementation
    timestamp(&start);
    my_sparse_CSR(csr, vec, mysol, size);
    timestamp(&now);
    printf("Execution time (your CSR): %ld ms\n", diff_milli(&start, &now));

    // Compare custom SpMV result with the reference solution
    if (check_result(refsol, mysol, size)) {
        printf("Result is correct.\n");
    } else {
        printf("Result is incorrect!\n");
    }
    printf("==================================================\n\n");

    /********** Method 6: Your own sparse implementation (COO) **********/
    printf("==================================================\n");
    printf("Method 6: My sparse implementation (COO)\n");
    printf("--------------------------------------------------\n");

    // Convert the dense matrix 'mat' to COO format
    COOMatrix *coo = dense_to_coo(size, mat);

    // Perform SpMV with custom implementation
    timestamp(&start);
    my_sparse_COO(coo, vec, mysol, size);
    timestamp(&now);
    printf("Execution time (your COO): %ld ms\n", diff_milli(&start, &now));

    // Compare custom SpMV result with the reference solution
    if (check_result(refsol, mysol, size)) {
        printf("Result is correct.\n");
    } else {
        printf("Result is incorrect!\n");
    }
    printf("==================================================\n\n");

    /********** Free resources **********/
    free(mat);
    free(vec);
    free(refsol);
    free(mysol);

    free_csr(csr);          // Free custom CSR matrix
    free_coo(coo);          // Free custom COO matrix
    free_csc(csc);          // Free custom CSC matrix

    gsl_spmatrix_free(gsl_csr);    // Free GSL CSR matrix
    gsl_vector_free(gsl_vec);      // Free GSL vector
    gsl_vector_free(gsl_result);   // Free GSL result vector

    return 0;
}
