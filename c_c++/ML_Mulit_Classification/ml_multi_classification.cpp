/*
 * ml_multi_classification.c
 *
 *  Created on: Aug 21, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "fmincg.h"

using namespace cv;

double sigmoid(double z);
void costFunc(COSTDATA *cost_data, double* costVal, double* gradVec);
void beta_func(double beta[], double x[], double y[], double theta[], int featureNum, int size, double label);
void grad_func(double grad[], double beta[], double x[], double theta[], int featureNum, int size, double lambda);
double cost_func(double *x, double *y, double *theta, int size, int featureNum, double lamba, double label);

double sigmoid(double z)
{
	return (1/(1+exp(-z)));
}

void costFunc(COSTDATA *cost_data, double* costVal, double* gradVec)
{
	double beta[cost_data->size];

	*costVal = cost_func(cost_data->xVec,
						 cost_data->yVec,
						 cost_data->theta,
						 cost_data->size,
						 cost_data->featureNum,
						 cost_data->lambda,
						 cost_data->label
						 );

	//printf("cost val : %g\n", *costVal);
//
//	printf("\nbeta: \n");
//	for ( int i = 0; i < cost_data->size; i++ )
//	{
//		printf("%.2f ", beta[i]);
//	}

	beta_func(beta,
			  cost_data->xVec,
			  cost_data->yVec,
			  cost_data->theta,
			  cost_data->featureNum,
			  cost_data->size,
			  cost_data->label
			  );


	grad_func(gradVec,
			  beta,
			  cost_data->xVec,
			  cost_data->theta,
			  cost_data->featureNum,
			  cost_data->size,
			  cost_data->lambda
			 );


}

void beta_func(double beta[], double x[], double y[], double theta[], int featureNum, int size, double label)
{
	int i, j;
	double tmp, tmp_y;

	for ( i = 0; i < size; i++ )
	{
		tmp = 0;
		for ( j = 0; j < featureNum; j++ )
			tmp += x[featureNum*i+j]*theta[j];

		tmp_y = y[i];
		if (tmp_y == label)
		{
			tmp_y = 1.0;
		}
		else
			tmp_y = 0.0;

		beta[i] = sigmoid(tmp) - tmp_y;
	}
}

void grad_func(double grad[], double beta[], double x[], double theta[], int featureNum, int size, double lambda)
{
	int i, j;
	double tmp, reg;

	for ( i = 0; i < featureNum; i++ )
	{
		tmp = 0;
		reg = 0;

		for ( j = 0; j < size; j++ )
			tmp += beta[j]*x[featureNum*j + i];

		if ( i >= 1 )
			reg = theta[i]*lambda/size;

		grad[i] = tmp/size + reg;
	}

}

double cost_func(double *x, double *y, double *theta, int size, int featureNum, double lamba, double label)
{
	double ret, tmp, reg, tmp_y;
	int i, j;

	ret = tmp = reg = 0;

	for ( i = 0; i < size; i++)
	{
		tmp = 0;
		for ( j = 0; j < featureNum; j++ )
			tmp += *(x+i*featureNum+j)*theta[j];

		tmp_y = y[i];
		if (tmp_y == label)
			tmp_y = 1.0;
		else
			tmp_y = 0.0;

		ret += ( -tmp_y*log( sigmoid(tmp) ) - ( 1.0 - tmp_y )*log( 1.0 - sigmoid(tmp) ) );

	}

	for ( i = 1; i < featureNum; i++ )
	{
		reg = reg + theta[i]*theta[i];
	}

	reg = reg*lamba/(2*size);

	return ((ret/size) + reg);
}

int main(void)
{
	FILE *raw_data_x, *raw_data_y, *theta_para;
	COSTDATA cost_data;
	double *value_y;
	double *value_x;
	double *theta;
	double *x;
	double *pred;
	int i, j, k, l, ret;
	int random_arr[100];
	int find, n, cnt, num;
	int label = 10, featureNum = 401, sampleNum = 5000;

	ret = 0;
	value_x = (double *) malloc(sampleNum*400*sizeof(double));
	value_y = (double *) malloc(sampleNum*sizeof(double));
	x = (double *) malloc(sampleNum*featureNum*sizeof(double));
	theta = (double *) malloc(label*featureNum*sizeof(double));
	pred = (double *) malloc(featureNum*sizeof(double));

	raw_data_x = fopen("data/raw_data_x.dat", "r");
	raw_data_y = fopen("data/raw_data_y.dat", "r");
	theta_para = fopen("data/theta_para.dat", "w");

	double tol = 0.0;
	for ( i = 0; i < sampleNum; i++ )
	{
		x[i*featureNum] = 1;
		for ( j = 0; j < 400; j++)
		{
			fscanf(raw_data_x, "%lf", &value_x[i*400 + j]);
			x[i*featureNum + j + 1] = value_x[i*400 + j];
			tol += value_x[i*400 + j];
		}
		fscanf(raw_data_y, "%lf", &value_y[i]);

	}


	fclose(raw_data_x);
	fclose(raw_data_y);

	srand( (unsigned)time(NULL));
	n = 100;
	cnt = 0;

	while(cnt < n)
	{
		num = (int)( rand() /(RAND_MAX+1.0)*sampleNum + 1);

		find = 0;
		for(i=0; i<cnt; ++i){
			if(random_arr[i]==num) {
				find = 1;
				break;
			}
		}

		if(find==0) {
			random_arr[cnt]=num;
			++cnt;
		}
	}

//	for(i=0; i<n; ++i){
//		if(i%10==0) puts("");
//		printf("%d: %d ", i, random_arr[i]);
//	}

	Mat mat = Mat( 200, 200, CV_64FC1);
	for ( k = 0; k < 10; k ++ )
	{
		for (l = 0; l < 10; l++ )
		{
			for ( i = 0; i < 20; i++ )
			{
				for ( j = 0; j < 20; j++ )
//					mat.at<double>(i+k*20, j+l*20) = value_x[400*(k*10+l)+i+20*j];
					mat.at<double>(i+k*20, j+l*20) = value_x[400*random_arr[k*10+l]+i+20*j];
			}
		}
	}


//	imshow("test",mat);
//	char key = (char) waitKey(200);

	cost_data.xVec = x;
	cost_data.yVec = value_y;
	cost_data.size = sampleNum;
	cost_data.featureNum = featureNum;
	cost_data.lambda = 0.1;

	printf("start fmincg ~~~~\n");
	for ( i = 1; i <= label; i++ )
	{
		cost_data.label = i;
		cost_data.theta = (theta+(i-1)*featureNum);

		ret = fmincg(&costFunc, &cost_data, featureNum, 50);

		printf("%d:theat: ", i);
		for ( j = 0; j < featureNum; j++ )
		{
			printf("%g ", cost_data.theta[j]);
			fprintf(theta_para, "%g ", cost_data.theta[j]);
		}
		fprintf(theta_para, "\n");
		printf("\n");

	}
	printf("fmincg finish .... %d \n", ret);


	double tmp, tmp_correct, id;
	for ( i = 0; i < sampleNum; i ++ )
	{
		tmp = tmp_correct = id = 0;
		for ( j = 0; j < label; j++ )
		{
			tmp = 0;
			for ( k = 0; k < featureNum; k++ )
			{
				tmp += x[i*featureNum+k]*theta[j*featureNum+k];
			}
			tmp = sigmoid(tmp);
			if ( tmp > tmp_correct)
			{
				tmp_correct = tmp;
				id = j+1;
			}
		}

		pred[i] = id;
	}

	float correct_cnt = 0;
	for ( i = 0; i < sampleNum; i++ )
	{
		if ( pred[i] == value_y[i])
			correct_cnt++;
	}
	printf("Training Set Accuracy: %f\n", (correct_cnt/sampleNum*100.0));

	free(value_x);
	free(value_y);
	free(x);
	free(theta);
	free(pred);
	fclose(raw_data_x);
	fclose(raw_data_y);
	fclose(theta_para);

	printf("finished !!!!\n");
	return 0;
}

