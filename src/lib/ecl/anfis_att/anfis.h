/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/

#include "standard.h"

/* data structure */

typedef struct node_s {
	int index;		/* global node index within network */
	int layer;		/* which layer */
	int local_index;	/* local node index within layer */
	int parameter_n;	/* parameter no. */
	double value;		/* node value */
	double tmp;		/* for holding temporatory result */
	double de_do;		/* derivative of E to O */
	int function_index;	/* node function index  */
	struct node_list_s *fan_in;	/* list of fan_in nodes */
	struct node_list_s *fan_out;	/* list of fan_out nodes */
	struct parameter_list_s *parameter;/* list of parameters */
} NODE_T;

typedef struct integer_list_s {
	int index;
	struct integer_list_s *next;
} INTEGER_LIST_T; 

typedef struct node_list_s {
	NODE_T *content;
	struct node_list_s *next;
} NODE_LIST_T;

typedef struct parameter_list_s {
	int fixed;
	double content;
	double de_dp;
	struct parameter_list_s *next;
} PARAMETER_LIST_T;

typedef struct anfis_s{
    int In_n; /* number of input variables */
    int Mf_n;   /* number of membership functions along each input */
    int Node_n; /* number of total nodes */
    int Rule_n;    /* number of nodes in the 4-th layer */
    NODE_T **node_p;
    int **config;
}ANFIS_T;


void gen_config(ANFIS_T *anfis);
void build_anfis(ANFIS_T *anfis);
void build_layer(int layer, int n, int index, int parameter_n, int function_index, ANFIS_T *anfis);
PARAMETER_LIST_T * build_parameter_list(int n);
NODE_LIST_T * build_node_list(int type, int n, ANFIS_T *anfis);
int set_parameter_mode(ANFIS_T *anfis);
void calculate_output(int from, int to, ANFIS_T *anfis);
int connected(int i, int j, ANFIS_T *anfis);
void digit_rep(int *rep, int j, ANFIS_T *anfis);
int which_layer(int i, ANFIS_T *anfis);
int between(int l, int x,  int u);
double input(int node_index, ANFIS_T *anfis);
double mf(int node_index, ANFIS_T *anfis);
double multiply(int node_index, ANFIS_T *anfis);
double normalize(int node_index, ANFIS_T *anfis);
double consequent(int node_index, ANFIS_T *anfis);
double sum(int node_index, ANFIS_T *anfis);
void get_parameter(const char *parameter_file, ANFIS_T *anfis);
double run(double *data_vector, ANFIS_T *anfis);
void start_anfis(int in_n, int mf_n, const char *parameter_file, ANFIS_T *anfis);
