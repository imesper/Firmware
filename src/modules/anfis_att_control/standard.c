
/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/

#include "standard.h"


/* Usage of create_cubic, create_matrix and create_array:
   1. Declaration in the calling program:
    char ***create_cubic();
    char **create_matrix();
    char *create_array();

   2. Examples of invocations:
    my_cubic = (float ***)create_cubic(row, col, height, sizeof(float));
    my_matrix = (int **)create_matrix(row, col, sizeof(int));
    my_array = (double *)create_array(row+col, sizeof(double));
*/

char ***create_cubic(int row, int col,int  height, int element_size)
{
    char ***cubic;
    int i, j;

    cubic = (char ***)malloc(sizeof(char **)*row);
    if (cubic == NULL) {
        printf("Error in create_cubic!\n");
        exit(1);
    }
    for (i = 0; i < row; i++) {
        cubic[i] = (char **)malloc(sizeof(char *)*col);
        if (cubic[i] == NULL) {
            printf("Error in create_cubic!\n");
            exit(1);
        }
        for (j = 0; j < col; j++) {
            cubic[i][j] = (char *)malloc(element_size*height);
            if (cubic[i][j] == NULL) {
                printf("Error in create_cubic!\n");
                exit(1);
            }
        }
    }
    return(cubic);
}

char **create_matrix(int row_n, int col_n, int element_size)
{
    char **matrix;
    int i;

    matrix = (char **) malloc(sizeof(char *)*row_n);
    if (matrix == NULL) {
        printf("Error in create_matrix!\n");
        exit(1);
    }
    for (i = 0; i < row_n; i++) {
        matrix[i] = (char *) malloc(element_size*col_n);
        if (matrix[i] == NULL) {
            printf("Error in create_matrix!\n");
            exit(1);
        }
    }
    return(matrix);
}

char *create_array(int array_size, int element_size)
{
    char *array;

    array = (char *)malloc(array_size*element_size);
    if (array == NULL) {
        printf("Error in create_array!\n");
        exit(1);
    }
    return(array);
}

/* an friendly interface to fopen() */
FILE *open_file(char *file, char *mode)
{
    FILE *fp;

    if ((fp = fopen(file, mode)) == NULL){
        printf("Cannot open '%s'.\n", file);
        exit(1);
    }
    return(fp);
}

void free_cubic(char ***cubic, int row, int col)
{
    int i, j;

    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++)
            if (cubic[i][j] == NULL) {
                printf("free_cubic: already free!\n");
                exit(1);
            } else {
                free(cubic[i][j]);
                cubic[i][j] = NULL;
            }

        if (cubic[i] == NULL) {
            printf("free_cubic: already free!\n");
            exit(1);
        } else {
            free(cubic[i]);
            cubic[i] = NULL;
        }
    }
    if (cubic == NULL) {
        printf("free_cubic: already free!\n");
        exit(1);
    } else {
        free(cubic);
        cubic = NULL;
    }
}

void free_matrix(char **matrix, int row_n)
{
    int i;

    for (i = 0; i < row_n; i++)
        if (matrix[i] == NULL) {
            printf("free_matrix: row %d is already free!\n", i);
            exit(1);
        } else {
            free(matrix[i]);
            matrix[i] = NULL;
        }

    if (matrix == NULL) {
        printf("free_matrix: given matrix is already free!\n");
        exit(1);
    } else {
        free(matrix);
        matrix = NULL;
    }
}

void free_array(char *array)
{
    if (array == NULL) {
        printf("free_array: already free!\n");
        exit(1);
    } else {
        free(array);
        array = NULL;
    }
}

void print_matrix(double **matrix, int row_n, int col_n)
{
    int i, j;
    for (i = 0; i < row_n; i++) {
        for (j = 0; j < col_n; j++)
            printf("%lf ", matrix[i][j]);
        printf("\n");
    }
}

void print_array(double *array, int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%lf ", array[i]);
    printf("\n");
}

void write_matrix(double **matrix, int row_n, int col_n, char *file_name)
{
    FILE *fp;
    int i, j;

    fp = open_file(file_name, "w");
    for (i = 0; i < row_n; i++) {
        for (j = 0; j < col_n; j++)
            fprintf(fp, "%lf ", matrix[i][j]);
        fprintf(fp, "\n");
    }
}

/* write array to file 'file_name', using matlab format */
/* the matlab variable name is the baseanme of 'file_name' */
void write_array(double *array, int size, char *file_name)
{
    FILE *fp;
    int i;

    fp = open_file(file_name, "w");
    for (i = 0; i < size; i++)
        fprintf(fp, "%d %lf\n", i+1, array[i]);
    fclose(fp);
}

void
exit1(char *s)
{
    printf("%s\n", s);
    exit(1);
}

/* matrix operations */

/* matrix plus matrix */
void
m_plus_m(double **m1, double **m2, int row, int col, double **out)
{
    int i, j;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            out[i][j] = m1[i][j] + m2[i][j];
}

/* matrix minus matrix */
void
m_minus_m(double **m1, double **m2, int row, int col, double **out)
{
    int i, j;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            out[i][j] = m1[i][j] - m2[i][j];
}

/* matrix times matrix */
void
m_times_m(double **m1, double **m2, int row1, int col1, int col2, double **out)
{
    int i, j, k;
    for (i = 0; i < row1; i++)
        for (j = 0; j < col2; j++) {
            out[i][j] = 0;
            for (k = 0; k < col1; k++)
                out[i][j] += m1[i][k]* m2[k][j];
        }
}

/* scalar times matrix */
void
s_times_m(double c, double **m, int row, int col, double **out)
{
    int i, j;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            out[i][j] = c*m[i][j];
}

/* matrix transpose */
void
m_transpose(double **m, int row, int col, double **m_t)
{
    int i, j;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            m_t[j][i] = m[i][j];
}

/* matrix L-2 norm */
double
m_norm(double **m, int row, int col)
{
    int i, j;
    double total = 0;

    for (i = 0; i < col; i++)
        for (j = 0; j < row; j++)
            total += m[i][j]*m[i][j];
    return(sqrt(total));
}
