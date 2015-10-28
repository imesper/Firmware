/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/

#include "anfis.h"

/* calculate node outputs from node 'from' to node 'to' */
void calculate_output(int from, int to, ANFIS_T *anfis)

{

    static double (*function[6])(int, ANFIS_T *) = {input, mf, multiply, normalize, consequent, sum};
	int i;
	int function_index;

	for (i = from; i <= to; i++) {
        function_index= anfis->node_p[i]->function_index;
        printf("Function: %d \n", function_index);
        anfis->node_p[i]->value = (*function[function_index])(i, anfis);
	}
}

double input(int node_index, ANFIS_T *anfis)
{
	printf("This shouldn't have been called!\n");
	return(0);
}

/* membership function = 1/(1+pow((x - c)/a, 2*b)) */
double mf(int node_index, ANFIS_T *anfis)
{
    NODE_LIST_T *arg_p = anfis->node_p[node_index]->fan_in;
    PARAMETER_LIST_T *para_p = anfis->node_p[node_index]->parameter;
	double c, a, b, x;
	double tmp1, tmp2;

	x = arg_p->content->value;
	a = para_p->content;
	b = para_p->next->content;
	c = para_p->next->next->content;
    //printf("A:  %.6f \n", a);
    if ( a < 0.0000000001 && a > -0.000000001)
		exit1("Error in mf!");
	tmp1 = (x - c)/a;
    tmp2 = (tmp1 < 0.000000001 && tmp1 > -0.0000000001) ? 0 : pow(pow(tmp1, 2.0), b);
	return(1/(1+ tmp2)); 
}

double multiply(int node_index, ANFIS_T *anfis)
{
	double product = 1.0;
	NODE_LIST_T *p;

    NODE_LIST_T *arg_p = anfis->node_p[node_index]->fan_in;
    PARAMETER_LIST_T *para_p = anfis->node_p[node_index]->parameter;
	if (para_p != NULL)
		exit1("Error in multiply!");
	for (p = arg_p; p != NULL; p = p->next)
		product *= p->content->value;
	return(product); 
}
 
double normalize(int node_index, ANFIS_T *anfis)
{
    NODE_LIST_T *arg_p = anfis->node_p[node_index]->fan_in;
    PARAMETER_LIST_T *para_p = anfis->node_p[node_index]->parameter;
	int i;
	double denom = 0;
	NODE_LIST_T *p;

	if (para_p != NULL)
		exit1("Error in normalize!");

	for (p = arg_p; p != NULL; p = p->next)
		denom += p->content->value;

	p = arg_p;
    for (i = 0; i < anfis->node_p[node_index]->local_index; i++)
		p = p->next; 

    if (denom > -0.00000001 && denom < 0.000000001)
		exit1("Error in normalize!");

	return(p->content->value/denom);
}

double consequent(int node_index, ANFIS_T *anfis)
{
    NODE_LIST_T *arg_p = anfis->node_p[node_index]->fan_in;
    PARAMETER_LIST_T *para_p = anfis->node_p[node_index]->parameter;
	int i;
    double x = 0, a = 0, total = 0;
    for (i = 0; i < anfis->In_n + 1; i++) {
		x = arg_p->content->value;
		a = para_p->content;
        if (i == anfis->In_n)
			break;
		total += x*a;
		arg_p = arg_p->next;
		para_p = para_p->next;
	}
	return(x*(total + a));
	/*
	double a, b, c, wn, x1, x2;
	x1 = arg_p->content->value;
	x2 = arg_p->next->content->value;
	wn = arg_p->next->next->content->value;
	a = para_p->content;
	b = para_p->next->content;
	c = para_p->next->next->content;
	return((a*x1 + b*x2 + c)*wn);
	*/
}

double sum(int node_index, ANFIS_T *anfis)
{
    NODE_LIST_T *arg_p = anfis->node_p[node_index]->fan_in;
	NODE_LIST_T *t;
	double total = 0;

	if (arg_p == NULL)
		exit1("Error! Given pointer is NIL!");

	for (t = arg_p; t != NULL; t = t->next)
		total += t->content->value;
	return(total);
}
