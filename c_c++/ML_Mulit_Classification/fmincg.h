/*
 * fmincg.h
 *
 *  Created on: Aug 24, 2015
 *      Author: iron
 */

#ifndef FMINCG_H_
#define FMINCG_H_

#include <stdio.h>
#include <math.h>
#include <float.h>

#define COST_FUNC_DATATYPE double
#define COST_FUNC_DATATYPE_MIN 2.2251e-308

#define RHO 0.01f
#define SIG 0.5f
#define INT 0.1f
#define EXT 3.0f
#define MAX 20
#define RATIO 100.0f

typedef struct cost_data {
	double *xVec;
	double *yVec;
	double *theta;
	double label;
	int size;
	int featureNum;
	float lambda;
} COSTDATA;

int fmincg(void (*costFunc)(COSTDATA *cost_data, COST_FUNC_DATATYPE *cost, COST_FUNC_DATATYPE *gradVec), COSTDATA* cost_data, int nDim, int maxCostCalls);


#endif /* FMINCG_H_ */
