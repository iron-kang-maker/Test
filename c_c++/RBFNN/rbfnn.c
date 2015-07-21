/*
 * rbfnn.c
 *
 *  Created on: Apr 9, 2015
 *      Author: iron
 */
#include "rbfnn.h"
#include <math.h>

#define LIGHT_GREEN "\033[1;32m"

void RBFNN_init(RBFNN *rbfnn, int input_layer_num, int hidden_layer_num, float *bias, float *center, float *weight)
{
	int i;

	rbfnn->in_layer = input_layer_num;
	rbfnn->hid_layer = hidden_layer_num;

	rbfnn->center = (float *) malloc(hidden_layer_num*input_layer_num*sizeof(float));
	rbfnn->center_1 = (float *) malloc(hidden_layer_num*input_layer_num*sizeof(float));
	rbfnn->center_2 = (float *) malloc(hidden_layer_num*input_layer_num*sizeof(float));

	rbfnn->dw = (float *) malloc(rbfnn->hid_layer*sizeof(float));
	rbfnn->db = (float *) malloc(rbfnn->hid_layer*sizeof(float));
	rbfnn->dc = (float *) malloc(rbfnn->in_layer*rbfnn->hid_layer*sizeof(float));

	for(i=0; i<hidden_layer_num*input_layer_num; i++)
	{
		rbfnn->center[i] = center[i];
		rbfnn->center_1[i] = center[i];
		rbfnn->center_2[i] = center[i];
	}

	rbfnn->bias = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn->bias_1 = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn->bias_2 = (float *) malloc(hidden_layer_num*sizeof(float));

	rbfnn->weight = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn->weight_1 = (float *) malloc(hidden_layer_num*sizeof(float));
	rbfnn->weight_2 = (float *) malloc(hidden_layer_num*sizeof(float));

	for(i=0; i<hidden_layer_num; i++)
	{
		rbfnn->bias[i] = bias[i];
		rbfnn->bias_1[i] = bias[i];
		rbfnn->bias_2[i] = bias[i];

		rbfnn->weight[i] = weight[i];
		rbfnn->weight_1[i] = weight[i];
		rbfnn->weight_2[i] = weight[i];
	}

	rbfnn->gaussian = (float *) malloc(hidden_layer_num*sizeof(float));
}

float RBFNN_norm(RBFNN *rbfnn, float *u, int layer)
{
	float norm;
	int i;

	norm = 0;

	for(i=0; i<rbfnn->in_layer; i++)
	{
		norm += pow(u[i]-rbfnn->center[layer*rbfnn->in_layer+i], 2);
	}

	return norm;
}

/* Formula:          ||X(k)-Cj(K)||^2
 * 		h(j) = exp(- ----------------- )
 *                  	2*bj(k)^2
 */
void RBFNN_gaussian(RBFNN *rbfnn, float *u)
{
	int i;
	for(i=0; i<rbfnn->hid_layer; i++)
	{
		rbfnn->gaussian[i] = exp(-RBFNN_norm(rbfnn, u, i)/(2*rbfnn->bias[i]*rbfnn->bias[i]));
	}
}

void RBFNN_exit(RBFNN *rbfnn)
{
	free(rbfnn->bias);
	free(rbfnn->bias_1);
	free(rbfnn->bias_2);
	free(rbfnn->db);

	free(rbfnn->center);
	free(rbfnn->center_1);
	free(rbfnn->center_2);
	free(rbfnn->dc);

	free(rbfnn->weight);
	free(rbfnn->weight_1);
	free(rbfnn->weight_2);
	free(rbfnn->dw);
}


float RBFNN_model(RBFNN *rbfnn, float *u, float out)
{
	float rfb_out, err;
	int i, j, k;

	RBFNN_gaussian(rbfnn, u);

	rfb_out = 0;
	err = 0;
	/* RBFNN output*/
	for(i=0; i<rbfnn->hid_layer; i++)
	{
		rfb_out += rbfnn->weight[i]*rbfnn->gaussian[i];
	}

	err = out - rfb_out;
	/* update weight, bias, center */
	for(i=0; i<rbfnn->hid_layer; i++)
	{
		rbfnn->dw[i] = rbfnn->eta*err*rbfnn->gaussian[i];
		rbfnn->db[i] = rbfnn->eta*err*rbfnn->weight[i]*rbfnn->gaussian[i]*RBFNN_norm(rbfnn, u, i)*pow(rbfnn->bias[i],-3);
		for(j=0; j<rbfnn->in_layer; j++)
			rbfnn->dc[i*rbfnn->in_layer+j] = err*rbfnn->eta*rbfnn->weight[i]*rbfnn->gaussian[i]*(u[j]-rbfnn->center[i*rbfnn->in_layer+j])*pow(rbfnn->bias[i],-2);
	}


	for(i=0; i<rbfnn->hid_layer; i++)
	{
		rbfnn->weight[i] = rbfnn->weight_1[i] + rbfnn->dw[i] + rbfnn->alpha*(rbfnn->weight_1[i]-rbfnn->weight_2[i]);
		rbfnn->bias[i]   = rbfnn->bias_1[i] + rbfnn->db[i] + rbfnn->alpha*(rbfnn->bias_1[i]-rbfnn->bias_2[i]);

		for(j=0; j<rbfnn->in_layer; j++) {
			k = i*rbfnn->in_layer+j;
			rbfnn->center[k] = rbfnn->center_1[k] + rbfnn->dc[k] + rbfnn->alpha*(rbfnn->center_1[k]-rbfnn->center_2[k]);
			rbfnn->center_2[k] = rbfnn->center_1[k];
			rbfnn->center_1[k] = rbfnn->center[k];
		}

		rbfnn->weight_2[i] = rbfnn->weight_1[i];
		rbfnn->weight_1[i] = rbfnn->weight[i];
		rbfnn->bias_2[i] = rbfnn->bias_1[i];
		rbfnn->bias_1[i] = rbfnn->bias[i];

	}


	return rfb_out;
}


