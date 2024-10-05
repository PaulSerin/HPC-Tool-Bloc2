#ifndef SPMV_H
#define SPMV_H

void my_dense(int size, const double mat[], const double vec[], double result[]);
int my_sparse(int size, const double mat[], const double vec[], double result[]);

#endif
