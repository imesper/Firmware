/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/
#include <stdio.h>
#include "anfis.h"



#include <stdio.h>
#include <math.h>
#include<stdlib.h>

void gen_config(ANFIS_T *anfis)
{
    int i, j;

    anfis->Rule_n = pow((double)anfis->Mf_n, (double)anfis->In_n);
    anfis->Node_n = anfis->In_n + anfis->In_n*anfis->Mf_n + 3*anfis->Rule_n + 1;

    anfis->config = (int **)create_matrix(anfis->Node_n, anfis->Node_n, sizeof(int));

    for (i = 0; i < anfis->Node_n; i++){
        for (j = 0; j < anfis->Node_n; j++)
             anfis->config[i][j] = connected(i, j, anfis);
        printf("\n");
    }
}

int
connected(int i, int j, ANFIS_T *anfis)
{

    int layer1, layer2;
    int group;
    int position;
    int *rep;

    if (i >= j)
        return(0);
    layer1 = which_layer(i, anfis);
    layer2 = which_layer(j, anfis);
    if ((layer2 - layer1 != 1) && layer1 != 0)
        return(0);
    switch(layer1) {
        case 0:
            if (between(anfis->In_n + i*anfis->Mf_n, j,
                                    anfis->In_n + (i+1)*anfis->Mf_n - 1))
                return(1);
            if (between(anfis->In_n + anfis->In_n*anfis->Mf_n + 2*anfis->Rule_n, j,
                    anfis->In_n + anfis->In_n*anfis->Mf_n + 3*anfis->Rule_n- 1))
                return(1);
            break;
        case 1:
            /*
            group = (int)floor((double)((i - anfis->In_n)/anfis->Mf_n));
            */
            rep = (int *)calloc(anfis->In_n, sizeof(int));
            group = (i - anfis->In_n)/anfis->Mf_n;
            position = (i - anfis->In_n) % anfis->Mf_n;
            digit_rep(rep, j - anfis->In_n - anfis->In_n*anfis->Mf_n, anfis);
            if (rep[group] == position)
                return(1);
            break;
        case 2:
            if (between(anfis->In_n + anfis->In_n*anfis->Mf_n + anfis->Rule_n, j,
                            anfis->In_n + anfis->In_n*anfis->Mf_n + 2*anfis->Rule_n- 1))
                return(1);
            break;
        case 3:
            if (j - i == anfis->Rule_n)
                return(1);
            break;
        case 4:
            if (j == anfis->Node_n - 1)
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
digit_rep(int *rep, int j, ANFIS_T *anfis)
{
    int i;
    for (i = 0; i < anfis->In_n; i++) {
        rep[anfis->In_n - i - 1] = j % anfis->Mf_n;
        /* j = (int)floor((double)(j/anfis->Mf_n)); */
        j = j/anfis->Mf_n;
    }
}

int
which_layer(int i, ANFIS_T *anfis)
{

    if (between(0, i, anfis->In_n - 1))
        return(0);
    if (between(anfis->In_n, i, anfis->In_n + anfis->In_n*anfis->Mf_n - 1))
        return(1);
    if (between(anfis->In_n + anfis->In_n*anfis->Mf_n, i,
                    anfis->In_n + anfis->In_n*anfis->Mf_n + anfis->Rule_n - 1))
        return(2);
    if (between(anfis->In_n + anfis->In_n*anfis->Mf_n + anfis->Rule_n, i,
                    anfis->In_n + anfis->In_n*anfis->Mf_n + 2*anfis->Rule_n- 1))
        return(3);
    if (between(anfis->In_n + anfis->In_n*anfis->Mf_n + 2*anfis->Rule_n, i,
                    anfis->In_n + anfis->In_n*anfis->Mf_n + 3*anfis->Rule_n- 1))
        return(4);
    if ((i = (anfis->In_n + anfis->In_n*anfis->Mf_n + 3*anfis->Rule_n)))
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

void build_anfis(ANFIS_T *anfis)
{

	int i;

    build_layer(0, anfis->In_n, 0, 0, 0, anfis);
    build_layer(1, anfis->In_n*anfis->Mf_n, anfis->In_n, 3, 1, anfis);
    build_layer(2, anfis->Rule_n, anfis->In_n+anfis->In_n*anfis->Mf_n, 0, 2, anfis);
    build_layer(3, anfis->Rule_n, anfis->In_n+anfis->In_n*anfis->Mf_n+anfis->Rule_n, 0, 3, anfis);
    build_layer(4, anfis->Rule_n, anfis->In_n+anfis->In_n*anfis->Mf_n+2*anfis->Rule_n, anfis->In_n + 1, 4, anfis);
    build_layer(5, 1, anfis->In_n+anfis->In_n*anfis->Mf_n+3*anfis->Rule_n, 0, 5, anfis);

    for (i = 0; i < anfis->Node_n; i++) {
        anfis->node_p[i]->fan_in  = build_node_list(0, i, anfis);
        anfis->node_p[i]->fan_out = build_node_list(1, i, anfis);
	}
}

/* Build a node list of layer-th layer, with n node, 
   starting at index index, and each
   node has parameter_n parameters and node function 
   function[function_index]. */
void build_layer(int layer, int n, int index, int parameter_n, int function_index, ANFIS_T *anfis)
{
	int i;
	NODE_T *q;

	if (n == 0)
		printf("Possible error in build_layer!\n");

	for (i = 0; i < n; i++) {
		q = (NODE_T *) malloc(sizeof (NODE_T));
        anfis->node_p[i + index] = q;
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
NODE_LIST_T * build_node_list(int type, int n, ANFIS_T *anfis)
{
	NODE_LIST_T *p, *q, *dummy;
	int i;

	p = (NODE_LIST_T *) malloc(sizeof (NODE_LIST_T));
	dummy = p;
	dummy->next = NULL;
	if (type == 0) 
        for (i = 0; i < anfis->Node_n; i++)
            if (anfis->config[i][n]){
				q = (NODE_LIST_T *) malloc(sizeof (NODE_LIST_T));
                q->content = anfis->node_p[i];
				p->next = q;
				p = q;
			}
	if (type == 1) 
        for (i = 0; i < anfis->Node_n; i++)
            if (anfis->config[n][i]){
				q = (NODE_LIST_T *) malloc(sizeof (NODE_LIST_T));
                q->content = anfis->node_p[i];
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
int set_parameter_mode(ANFIS_T *anfis)
{
	int i, modifiable_p_count = 0;
	PARAMETER_LIST_T *p;

    for (i = 0; i < anfis->Node_n; i++) {
        if (anfis->node_p[i]->parameter == NULL)
			continue;
        for (p = anfis->node_p[i]->parameter; p != NULL; p = p->next) {
			p->fixed = 0;
			modifiable_p_count++;
		}
	}
	printf("Modifiable parameters: %d\n", modifiable_p_count);
	return(modifiable_p_count);
}
