/*
 * fuzzy01.c
 *
 *  Created on: Mar 31, 2015
 *      Author: iron
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float Gas_NM[] = { 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float Gas_Z[]  = { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float Gas_PM[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0 };

#define max(a,b) ((a>b) ? a:b)
#define min(a,b) ((a<b) ? a:b)

int Gas_output;

void GAS_OUT(int T)
{
	int i;
	float map_i, v_i, area, moment;
	float u_T;
	float *u_Gas;

	u_Gas = (float *) malloc(11*sizeof(float));

	/************* Rule 1 ***********************/
	/* If T is PM */
	u_T = 0.0;
	if ((T>=74) && (T<=86))
		u_T = (float)(T-74)/12;

	printf("Rule 1: %f\n", u_T);

	/* Then Gas is NM */
	if (u_T != 0.0) {
		for(i=0; i<11; i++) {
			u_Gas[i] = max(u_Gas[i], min(u_T, Gas_NM[i]));
		}
	}

	/*************** Rule 2 **********************/
	/* If T is Z */
	u_T = 0.0;
	if ((T>=60) && (T<=70))
		u_T = (float)(T-60)/10;
	else if ((T>70) && (T<=80))
		u_T = (float)(80-T)/10;

	printf("Rule 2: %f\n", u_T);

	/* Then Gas is Z */
	if (u_T != 0.0) {
		for(i=0; i<11; i++) {
			u_Gas[i] = max(u_Gas[i], min(u_T, Gas_Z[i]));
		}
	}


	/**************** Rule 3 ************************/
	/* If T is NM */
	u_T = 0.0;
	if ((T>=54) && (T<=66))
		u_T = (float)(66-T)/12;

	printf("Rule 3: %f\n", u_T);

	/* Then Gas is PM*/
	if (u_T != 0) {
		for(i=0; i<11; i++)
			u_Gas[i] = max(u_Gas[i], min(u_T, Gas_PM[i]));
	}

	/**************** DeFuzzy ************************/
	area = 0.0;
	moment = 0.0;
	for(i=0; i<11; i++) {
		map_i = u_Gas[i];
		printf("i=%2d, map_i=%f\n", i, map_i);
		v_i = -5 + i;
		area += map_i;
		moment += map_i*v_i;
	}

	printf("\narea = %f moment = %f\n", area, moment);
	if (area == 0)
		Gas_output = 5;
	else
		Gas_output = (int)(moment/area);
}

int main(void) {
	int T;
	T = 64;
	GAS_OUT(T);
	printf("\nT = %d Gas_output = %d\n", T, Gas_output);
	getchar();
	return 0;
}
