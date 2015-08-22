/*
 * mat_convert.c
 *
 *  Created on: Aug 22, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <mat.h>
#include <matrix.h>

int main(void)
{
	FILE *raw_data_x, *raw_data_y;
	double *value_y;
	double *value_x;
	int i, j;

	MATFile *datafile;

	value_x = (double *) malloc(5000*400*sizeof(double));

	printf("open mat file\n");
	datafile = matOpen("data/ex3data1.mat", "r");
	if ( datafile == NULL )
	{
		printf("open mat file fail xxxx\n");
		return -1;
	}

	mxArray *mxValue_y, *mxValue_x;
	mxValue_y = matGetVariable(datafile, "y");
	value_y = mxGetPr(mxValue_y);
	printf("get y data\n");

	mxValue_x = matGetVariable(datafile, "X");
	value_x = mxGetPr(mxValue_x);


	raw_data_x = fopen("data/raw_data_x.dat", "w");
	raw_data_y = fopen("data/raw_data_y.dat", "w");


	for ( i = 0; i < 5000; i++ )
	{
		for ( j = 0; j < 400; j++)
			fprintf(raw_data_x, "%lf", value_x[j*5000+i]);
		fprintf(raw_data_x, "\n");
		fprintf(raw_data_y, "%f", value_y[i]);
	}

	fclose(raw_data_x);
	fclose(raw_data_y);
//	Mat mat = Mat( 400, 1, CV_64FC1);
//	for ( i = 0; i < 400; i++ )
//		mat.at<double>(i) = pr[5000*i];

//	imshow("test",mat);
	matClose(datafile);
//	printf("x[134]: %f, y[2000]: %f\n", value_x[3*400+134], value_y[2000]);
//	mxFree(mxValue);
	printf("finished !!!!\n");
	return 0;
}

