/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/
#include <stdio.h>
#include "anfis.h"

static int **config;

#include <stdio.h>
#include <math.h>
#include<stdlib.h>
/*
#define VAR_N 3
#define MF_N 2
#define NODE_N (VAR_N + VAR_N*MF_N + 3*MF_N*MF_N + 1)
*/
int VAR_N, MF_N, NODE_N, RULE_N;

void gen_config(int in_n, int mf_n)
{
    int i, j;

    VAR_N = in_n;
    MF_N = mf_n;
    RULE_N = pow((double)MF_N, (double)VAR_N);
    NODE_N = VAR_N + VAR_N*MF_N + 3*RULE_N + 1;

    config = (int **)create_matrix(NODE_N, NODE_N, sizeof(int));

    for (i = 0; i < NODE_N; i++){
        for (j = 0; j < NODE_N; j++)
             config[i][j] = connected(i, j);
        printf("\n");
    }
}

int
connected(int i, int j)
{

    int layer1, layer2;
    int group;
    int position;
    int *rep;

    if (i >= j)
        return(0);
    layer1 = which_layer(i);
    layer2 = which_layer(j);
    if ((layer2 - layer1 != 1) && layer1 != 0)
        return(0);
    switch(layer1) {
        case 0:
            if (between(VAR_N + i*MF_N, j,
                                    VAR_N + (i+1)*MF_N - 1))
                return(1);
            if (between(VAR_N + VAR_N*MF_N + 2*RULE_N, j,
                    VAR_N + VAR_N*MF_N + 3*RULE_N- 1))
                return(1);
            break;
        case 1:
            /*
            group = (int)floor((double)((i - VAR_N)/MF_N));
            */
            rep = (int *)calloc(VAR_N, sizeof(int));
            group = (i - VAR_N)/MF_N;
            position = (i - VAR_N) % MF_N;
            digit_rep(rep, j - VAR_N - VAR_N*MF_N);
            if (rep[group] == position)
                return(1);
            break;
        case 2:
            if (between(VAR_N + VAR_N*MF_N + RULE_N, j,
                            VAR_N + VAR_N*MF_N + 2*RULE_N- 1))
                return(1);
            break;
        case 3:
            if (j - i == RULE_N)
                return(1);
            break;
        case 4:
            if (j == NODE_N - 1)
                return(1);
            break;
        case 5:
            return(0);
            break;
        default:
            printf("Error in layer!\n");
    }
    return(0);
}

void
digit_rep(int *rep, int j)
{
    int i;
    for (i = 0; i < VAR_N; i++) {
        rep[VAR_N - i - 1] = j % MF_N;
        /* j = (int)floor((double)(j/MF_N)); */
        j = j/MF_N;
    }
}

int
which_layer(int i)
{

    if (between(0, i, VAR_N - 1))
        return(0);
    if (between(VAR_N, i, VAR_N + VAR_N*MF_N - 1))
        return(1);
    if (between(VAR_N + VAR_N*MF_N, i,
                    VAR_N + VAR_N*MF_N + RULE_N- 1))
        return(2);
    if (between(VAR_N + VAR_N*MF_N + RULE_N, i,
                    VAR_N + VAR_N*MF_N + 2*RULE_N- 1))
        return(3);
    if (between(VAR_N + VAR_N*MF_N + 2*RULE_N, i,
                    VAR_N + VAR_N*MF_N + 3*RULE_N- 1))
        return(4);
    if ((i = (VAR_N + VAR_N*MF_N + 3*RULE_N)))
        return(5);
    printf("Error in which_layer!\n");
    exit(1);
}

int between(int l, int x,  int u)
{
    if ((l <= x) && (x <= u))
        return(1);
    else
        return(0);
}

void build_anfis()
{

	int i;

	build_layer(0, In_n, 0, 0, 0); 
	build_layer(1, In_n*Mf_n, In_n, 3, 1); 
	build_layer(2, Rule_n, In_n+In_n*Mf_n, 0, 2); 
	build_layer(3, Rule_n, In_n+In_n*Mf_n+Rule_n, 0, 3); 
	build_layer(4, Rule_n, In_n+In_n*Mf_n+2*Rule_n, In_n + 1, 4); 
	build_layer(5, 1, In_n+In_n*Mf_n+3*Rule_n, 0, 5); 

	for (i = 0; i < Node_n; i++) {
		node_p[i]->fan_in  = build_node_list(0, i);
		node_p[i]->fan_out = build_node_list(1, i);
	}
}

/* Build a node list of layer-th layer, with n node, 
   starting at index index, and each
   node has parameter_n parameters and node function 
   function[function_index]. */
void build_layer(int layer, int n, int index, int parameter_n, int function_index)
{
	int i;
	NODE_T *q;

	if (n == 0)
		printf("Possible error in build_layer!\n");

	for (i = 0; i < n; i++) {
		q = (NODE_T *) malloc(sizeof (NODE_T));
		node_p[i + index] = q;
		q->index = i + index;
		q->layer = layer;
		q->local_index = i;
		q->parameter_n = parameter_n;
		q->function_index = function_index;
		q->parameter = build_parameter_list(parameter_n);
	}
}

/* Build a parameter list with length n. */
PARAMETER_LIST_T * build_parameter_list(int n)
{
	PARAMETER_LIST_T *head, *p, *q;
	int i;

	if (n < 0)
		exit1("Error in build_parameter_list()!");

	if (n == 0)
		return(NULL);

	head = (PARAMETER_LIST_T *) malloc(sizeof (PARAMETER_LIST_T));
	p = head;
	for (i = 1; i < n; i++){
		q = (PARAMETER_LIST_T *) malloc(sizeof (PARAMETER_LIST_T));
		p->next = q;
		p = q;
	}
	p->next = NULL;
	return(head);
}

/* type == 0 --> build node list along column n of matrix config.
   type == 1 --> build node list along row n of matrix config.     */
NODE_LIST_T * build_node_list(int type, int n)
{
	NODE_LIST_T *p, *q, *dummy;
	int i;

	p = (NODE_LIST_T *) malloc(sizeof (NODE_LIST_T));
	dummy = p;
	dummy->next = NULL;
	if (type == 0) 
		for (i = 0; i < Node_n; i++)
			if (config[i][n]){
				q = (NODE_LIST_T *) malloc(sizeof (NODE_LIST_T));
				q->content = node_p[i];
				p->next = q;
				p = q;
			}
	if (type == 1) 
		for (i = 0; i < Node_n; i++)
			if (config[n][i]){
				q = (NODE_LIST_T *) malloc(sizeof (NODE_LIST_T));
				q->content = node_p[i];
				p->next = q;
				p = q;
			}
	p->next = NULL;
	q = dummy;
	dummy = dummy->next;
	free(q);
	return(dummy);
}

/* set parameter mode: fixed = 1 --> fixed parameter;
		       fixed = 0 --> modifiable parameter */
int set_parameter_mode()
{
	int i, modifiable_p_count = 0;
	PARAMETER_LIST_T *p;

	for (i = 0; i < Node_n; i++) {
		if (node_p[i]->parameter == NULL)
			continue;
		for (p = node_p[i]->parameter; p != NULL; p = p->next) {
			p->fixed = 0;
			modifiable_p_count++;
		}
	}
	printf("Modifiable parameters: %d\n", modifiable_p_count);
	return(modifiable_p_count);
}
