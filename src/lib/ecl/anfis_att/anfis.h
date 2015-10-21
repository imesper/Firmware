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

/* global variables */
extern int In_n;  /* number of input variables */
extern int Mf_n;   /* number of membership functions along each input */
extern int Node_n; /* number of total nodes */
extern int Rule_n;    /* number of nodes in the 4-th layer */
extern NODE_T **node_p;

void gen_config(int in_n, int mf_n);
void build_anfis(void);
void build_layer(int layer, int n, int index, int parameter_n, int function_index);
PARAMETER_LIST_T * build_parameter_list(int n);
NODE_LIST_T * build_node_list(int type, int n);
int set_parameter_mode(void);
void calculate_output(int from, int to);
int connected(int i, int j);
void digit_rep(int *rep, int j);
int which_layer(int i);
int between(int l, int x,  int u);
double input(int node_index);
double mf(int node_index);
double multiply(int node_index);
double normalize(int node_index);
double consequent(int node_index);
double sum(int node_index);
void get_parameter(char * parameter_file);
double run(double *data_vector);
void start_anfis(int in_n, int mf_n, char *parameter_file);
