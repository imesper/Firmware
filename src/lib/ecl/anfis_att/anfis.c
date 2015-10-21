/*
   Copyright (c) 1991 Jyh-Shing Roger Jang, Dept of EECS, U.C. Berkeley
   BISC (Berkeley Initiative on Soft Computing) group
   jang@eecs.berkeley.edu
   Permission is granted to modify and re-distribute this code in any manner
   as long as this notice is preserved.  All standard disclaimers apply.
*/

#include "anfis.h"
#include <fcntl.h>

NODE_T **node_p;

int Rule_n, Node_n, In_n, Mf_n;

void get_parameter(char * parameter_file)
{
    int i, j;
    int parameter_n;
    char m_data[20] ;
    int res;
    double tmp;
    int fp;
    PARAMETER_LIST_T *p;

    fp = open(parameter_file,  O_RDONLY);
    for (i = 0; i < Node_n; i++) {
        if (node_p[i]->parameter == NULL)
            continue;

        parameter_n = node_p[i]->parameter_n;
        p = node_p[i]->parameter;
        //printf("Parameter: %d \n", parameter_n);
        for (int k = 0; k < parameter_n; k++){

            j = 0;
            do{
                res = read(fp, &m_data[j++], 1);
                printf("Res: %d \n", res);
            }while(m_data[j - 1] != 0x20 && m_data[j - 1] != ' ' && m_data[j - 1] != '\n');

            m_data[j - 1] = '\0';
            tmp = atof(m_data);

            p->content = tmp;
            p = p->next;

            //printf("Parameter: %d %0.6g \n", k, tmp);
        }

    }
    close(fp);
}

void start_anfis(int in_n, int mf_n, char *parameter_file){

    In_n = in_n;
    Mf_n = mf_n;

    Rule_n = pow((double)Mf_n, (double)In_n);
    Node_n = In_n + In_n*Mf_n + 3*Rule_n + 1;

    /* allocate matrices */
    node_p = (NODE_T **)create_array(Node_n, sizeof(NODE_T *));

    //anfis_output = (double *)calloc(training_data_n, sizeof(double));
    gen_config(In_n, Mf_n);

    build_anfis();

    //parameter_n = set_parameter_mode();

    //parameter_array = (double *)calloc(parameter_n, sizeof(double));

    get_parameter(parameter_file);
}

double run(double *data_vector)
{
    int k;
    //int parameter_n;
    //double *parameter_array;



    //get_data(TRAIN_DATA_FILE, training_data_n, training_data_matrix);
    //get_data(CHECK_DATA_FILE, checking_data_n, checking_data_matrix);

    //if (debug != 0) debug_anfis();

    //put_input_data(j, training_data_matrix);
    for (k = 0; k < In_n; k++)
        node_p[k]->value = data_vector[k];
    /* get node outputs from layer 1 to layer 3 */

    calculate_output(In_n, Node_n - 1);

    return node_p[Node_n - 1]->value;
}

