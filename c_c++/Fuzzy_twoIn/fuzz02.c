/*
 * fuzz02.c
 *
 *  Created on: Mar 31, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define max(a,b) ((a>b) ? a:b)
#define min(a,b) ((a<b) ? a:b)

float Gas_NM[] = { 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float Gas_NS[] = { 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float Gas_Z[]  = { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float Gas_PS[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0 };
float Gas_PM[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0 };

float Gas_output;

void GAS_OUT(int T, int dT)
{
	int i;
	float map_i, v_i, area, moment;
	float t001;
	float ut_nm, ut_z, ut_pm;
	float dt_nm, dt_z, dt_pm;
	float *O_Gas;

	O_Gas = (float *) malloc(11*sizeof(float));
	printf("t001 addr: %x\n", &t001);
	for (i=0; i<11; i++)
		printf("O_Gas[%d]: %f\n", i, O_Gas[i]);

	/* T is NM */
	ut_nm = 0.0;
	if ((T>=54) && (T<=66))
		ut_nm = (float)(66-T)/12;

	/* T is Z */
	ut_z = 0.0;
	if ((T>=60) && (T<=70))
		ut_z = (float)(T-60)/10;
	else if ((T>70) && (T<=80))
		ut_z = (float)(80-T)/10;

	/* T is PM */
	ut_pm = 0.0;
	if ((T>=74) && (T<=86))
		ut_pm = (float)(T-74)/12;

	/* dT is NU */
	dt_nm = 0.0;
	if ((dT>=-10) && (dT<=-2))
		dt_nm = (float)-(dT+2)/8;

	/* dT is Z */
	dt_z = 0.0;
	if ((dT>=-7) && (dT<=0))
		dt_z = (float)(dT+7)/7;
	else if ((dT>0) && (dT<=7))
		dt_z = (float)(7-dT)/7;

	/* dT is PM */
	dt_pm = 0.0;
	if ((dT>=2) && (dT<=10))
		dt_pm = (float)(dT-2)/8;


	/**** Rule 1: T is PM and dT is PM, then Gas is NM ****/
	t001 = min(ut_pm, dt_pm);
	printf("\nRule 1: %f\n", t001);

	if (t001 != 0.0) {
		for (i=0; i<11; i++) {
			//map_i = (float)Gas_NM[i];
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_NM[i]));
		}
	}

	/**** Rule 2: T is PM and dT is Z, then Gas is NS ****/
	t001 = min(ut_pm, dt_z);
	printf("Rule 2: %f\n", t001);

	if (t001 != 0.0) {
		for (i=0; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_NS[i]));
	}

	/**** Rule 3: T is PM and dT is NM, then Gas is Z ****/
	t001 = min(ut_pm, dt_nm);
	printf("Rule 3: %f\n", t001);

	if (t001 != 0.0) {
		for (i=0; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_Z[i]));
	}

	/**** Rule 4: T is Z and dT is PM, then Gas is NS ****/
	t001 = min(ut_z, dt_pm);
	printf("Rule 4: %f\n", t001);

	if (t001 != 0.0) {
		for (i=0; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_NS[i]));
	}

	/**** Rule 5: T is Z and dT is Z, then Gas is Z ****/
	t001 = min(ut_z, dt_z);
	printf("Rule 5: %f\n", t001);

	if (t001 != 0.0) {
		for(i=0; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_Z[i]));
	}

	/**** Rule 6: T is Z and dT is NM, then Gas is PS ****/
	t001 = min(ut_z, dt_nm);
	printf("Rule 6: %f\n", t001);

	if (t001 != 0.0) {
		for(i=1; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_PS[i]));
	}

	/**** Rule 7: T is NM and dT is PM, then Gas is Z ****/
	t001 = min(ut_nm, dt_pm);
	printf("Rule 7: %f\n", t001);

	if (t001 != 0.0) {
		for(i=1; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_Z[i]));
	}

	/**** Rule 8: T is NM and dT is Z, then Gas is PS ****/
	t001 = min(ut_nm, dt_z);
	printf("Rule 8: %f\n", t001);

	if (t001 != 0.0) {
		for(i=1; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_PS[i]));
	}


	/**** Rule 9: T is NM and dT is NM, then Gas is PM ****/
	t001 = min(ut_nm, dt_nm);
	printf("Rule 9: %f\n", t001);

	if (t001 != 0.0) {
		for(i=1; i<11; i++)
			O_Gas[i] = max(O_Gas[i], min(t001, Gas_PM[i]));
	}

	/**** Calculate crisp value for variable Gas ****/
	area = 0.0;
	moment = 0.0;
	for(i=0; i<11; i++) {
		map_i = O_Gas[i];
		printf("i=%2d, map_i=%f\n", i, map_i);
		v_i = -5 + (float)i;
		area += map_i;
		moment += map_i*v_i;
	}

	if (area == 0)
		Gas_output = 5.0;
	else
		Gas_output = (moment/area);

	//free(O_Gas);
}

int main(void) {
	GAS_OUT(64, -5);
	printf("\nT = 64, dT = -5, output = %f\n", Gas_output);
	GAS_OUT(64, 0);
	printf("\nT = 64, dT = 0, output = %f\n", Gas_output);
	GAS_OUT(64, 5);
	printf("\nT = 64, dT = 5, output = %f\n", Gas_output);

	getchar();
	return 0;
}

