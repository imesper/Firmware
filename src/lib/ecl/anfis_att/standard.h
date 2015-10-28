/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/

/* standard header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif	
/* handy macros */
#define ABS(x)   ( x > 0 ? (x): (-(x)) )
#define MAX(x,y) ( x > y ? (x) : (y) )
#define MIN(x,y) ( x < y ? (x) : (y) )
#define SGN(x)   ( x > 0 ? (1) : (-1) )

/* library function declaration */
/* memory allocation */
char *create_array(int array_size, int element_size);
char **create_matrix(int row_n, int col_n, int element_size);
char ***create_cubic(int row, int col,int  height, int element_size);
void free_array(char *array);
void free_matrix(char **matrix, int row_n);
void free_cubic(char ***cubic, int row, int col);

/* print to stdio and write to files */
void print_array(double *array, int size);
void print_matrix(double **matrix, int row_n, int col_n);
void write_array(double *array, int size, char *file_name);
void write_matrix(double **matrix, int row_n, int col_n, char *file_name);

/* file open */
FILE *open_file1(char *file, char *mode);
FILE *open_file2(char *file, char *mode);
FILE *open_file(char *file, char *mode);
/* matrix functions */
void m_plus_m(double **m1, double **m2, int row, int col, double **out);
void m_minus_m(double **m1, double **m2, int row, int col, double **out);
void m_times_m(double **m1, double **m2, int row1, int col1, int col2, double **out);
//void s_mult_m();
void s_times_m(double c, double **m, int row, int col, double **out);
void m_transpose(double **m, int row, int col, double **m_t);
double m_norm(double **m, int row, int col);

/* least square estimate */
//void initialize_kalman();
//double LSE_kalman();

/* random number generator */
//double random1();
//double random2();

/* others */
void exit1(char *s);
char *basename(char *path);
char * get_pwd(void);
#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif
