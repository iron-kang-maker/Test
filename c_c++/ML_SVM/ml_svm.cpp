/*
 * ml_svm.cpp
 *
 *  Created on: Sep 22, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"
#include "svm.h"

using namespace arma;

#define SAMPLE_NUM 51

void dataset3Params(mat X, mat y, mat Xval, mat yval, double &C, double &sigma)
{
	MODEL model;
	int i, j, k;
	double c_val[8] = { 0.01, 0.03, 0.1, 0.3, 1, 3, 10, 30};
	double sigma_val[8] = { 0.01, 0.03, 0.1, 0.3, 1, 3, 10, 30};
	double c_tmp = 1, sigma_tmp = 0.3, max_val = 1, sum = 0, pred;

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			model = svmTrain(X, y, c_val[i], gaussian, 0, 0, sigma_val[j]);
			sum = 0;
			for (k = 0; k < (int)Xval.n_rows; k++)
			{
				pred = svmPredict(model, Xval.row(k), gaussian, sigma_val[j]);
				if (pred >= 0)
					pred = 1;
				else
					pred = 0;

				if (pred != yval(k, 0))
					sum++;
			}

			sum = sum/(int)Xval.n_rows;

			if (sum < max_val)
			{
				c_tmp = c_val[i];
				sigma_tmp = sigma_val[j];
				max_val = sum;
			}
		}
	}
	C = c_tmp;
	sigma = sigma_tmp;
}

void visualizeBoundary(mat X, mat y, MODEL model, const char *dataPath)
{
	FILE *gaussian_kernel_predict;
	double min_val_1, max_val_1, step_1, min_val_2, max_val_2, step_2;
	double x1, x2;
	mat x1_max, x1_min, x2_max, x2_min, tmp(1, 2);

	gaussian_kernel_predict = fopen(dataPath, "w");

	x1_max = max(X.col(0), 0);
	x1_min = min(X.col(0), 0);
	x2_max = max(X.col(1), 0);
	x2_min = min(X.col(1), 0);

	max_val_1 = x1_max(0, 0);
	min_val_1 = x1_min(0, 0);
	max_val_2 = x2_max(0, 0);
	min_val_2 = x2_min(0, 0);

	step_1 = (max_val_1 - min_val_1)/100;
	step_2 = (max_val_2 - min_val_2)/100;

	for (x1 = min_val_1; x1 <= max_val_1; x1 += step_1)
	{
		for (x2 = min_val_2; x2 <= max_val_2; x2 += step_2)
		{
			tmp(0, 0) = x1;
			tmp(0, 1) = x2;
			if (fabs(svmPredict(model, tmp, gaussian, 0.1)) < 0.09)
				fprintf(gaussian_kernel_predict, "%lf %lf\n", x1, x2);
		}
	}

	fclose(gaussian_kernel_predict);

}

void visualizeBoundaryLinear(mat X, mat y, MODEL model)
{
	FILE *linear_kernel_predict;
	mat w = model.w, tmp1, tmp2;
	double b = model.b;
	double min_val, max_val, step, xp, yp;

	tmp1 = min(X.col(0), 0);
	tmp2 = max(X.col(0), 0);
	min_val = tmp1(0, 0);
	max_val = tmp2(0, 0);

	step = (max_val - min_val)/100;
	printf("min: %f, max: %f, step: %f\n", min_val,max_val, step);

	linear_kernel_predict = fopen("data/linear_kernel_predict", "w");

	for (xp = min_val; xp <= max_val; xp += step)
	{
		yp = -(w(0, 0)*xp + b)/w(1, 0);
		fprintf(linear_kernel_predict, "%f %f\n", xp, yp);
	}

	fclose(linear_kernel_predict);
}

int main(void)
{
	int i, data_num;
	double C, sigma;
	FILE *f_X, *f_y, *f_combine;
	FILE *plot_sample, *plot_linear_predict, *plot_sample_2;
	FILE *plot_gaussian_predict;
	mat X(SAMPLE_NUM, 2), y(SAMPLE_NUM, 1);
	MODEL model;

	f_X = fopen("data/X.dat", "r");

	plot_sample 	= popen ("gnuplot -persistent", "w");
	plot_linear_predict = popen ("gnuplot -persistent", "w");

	for (i = 0; i < SAMPLE_NUM; i++)
	{
		fscanf(f_X, "%lf %lf %lf", &X(i, 0), &X(i, 1), &y(i, 0));
	}

	fclose(f_X);

	fprintf(plot_sample, "set title \"SVM Sample data\"\n");
	fprintf(plot_sample, "set xr [0.0:5.0] \n");
	fprintf(plot_sample, "set yr [1.5:5.0] \n");
	fprintf(plot_sample, "plot \"< awk '{if($3 == 1) print}' data/X.dat\" u 1:2 notitle with points pointtype 2,");
	fprintf(plot_sample, "     \"< awk '{if($3 == 0) print}' data/X.dat\" u 1:2 notitle with points pointtype 64\n");

	/************** Training Linear Kernel SVM ****************/
	C = 1;
	model = svmTrain(X, y, C, linear, 0.001, 20, 0);
	visualizeBoundaryLinear(X, y, model);

	fprintf(plot_linear_predict, "set title \"SVM linear kernel\"\n");
	fprintf(plot_linear_predict, "set xr [0.0:5.0] \n");
	fprintf(plot_linear_predict, "set yr [1.5:5.0] \n");
	fprintf(plot_linear_predict, "plot \"< awk '{if($3 == 1) print}' data/X.dat\" u 1:2 notitle with points pointtype 2,");
	fprintf(plot_linear_predict, "     \"< awk '{if($3 == 0) print}' data/X.dat\" u 1:2 notitle with points pointtype 64,");
	fprintf(plot_linear_predict, "     'data/linear_kernel_predict' u 1:2 notitle with lines\n");

	/************* Implementing Gaussian Kernel ***************/
	mat X1(3, 1), X2(3, 1);
	X1(0, 0) = 1; X1(1, 0) = 2; X1(2, 0) = 1;
	X2(0, 0) = 0; X2(1, 0) = 4; X2(2, 0) = -1;
	sigma = 2;

	printf("Gaussian Kernel between x1 = [1; 2; 1], x2 = [0; 4; -1], sigma = 0.5 :"
	        "\n\t%f\n(this value should be about 0.324652)\n", gaussianKernel(X1, X2, sigma));


	/************* Dataset 2 ********************/
	f_combine = fopen("data/f_combine", "w");
	f_X = fopen("data/X2.dat", "r");
	f_y = fopen("data/y2.dat", "r");
	data_num = 863;
	X = mat(data_num, 2);
	y = mat(data_num, 1);

	for (i = 0; i < data_num; i++)
	{
		fscanf(f_X, "%lf %lf\n", &X(i, 0), &X(i, 1));
		fscanf(f_y, "%lf", &y(i, 0));

		fprintf(f_combine, "%lf %lf %d\n", X(i, 0), X(i, 1), (int)y(i, 0));
	}

	fclose(f_combine);
	fclose(f_X);
	fclose(f_y);

	plot_sample_2 	= popen ("gnuplot -persistent", "w");
	fprintf(plot_sample_2, "set title \"SVM Sample 2 data\"\n");
	fprintf(plot_sample_2, "set xr [0.0:1.0] \n");
	fprintf(plot_sample_2, "set yr [0.4:1.0] \n");
	fprintf(plot_sample_2, "plot \"< awk '{if($3 == 1) print}' data/f_combine\" u 1:2 notitle with points pointtype 2,");
	fprintf(plot_sample_2, "     \"< awk '{if($3 == 0) print}' data/f_combine\" u 1:2 notitle with points pointtype 64\n");

	/************** Training Gaussian kernel SVM ****************/
	C = 1;
	sigma = 0.1;
	model = svmTrain(X, y, C, gaussian, 0, 0, sigma);
	visualizeBoundary(X, y, model, "data/gaussian_kernel_predict");
	plot_gaussian_predict = popen ("gnuplot -persistent", "w");

	fprintf(plot_gaussian_predict, "set title \"SVM Gaussian kernel\"\n");
	fprintf(plot_gaussian_predict, "set xr [0.0:1.0] \n");
	fprintf(plot_gaussian_predict, "set yr [0.4:1.0] \n");
	fprintf(plot_gaussian_predict, "plot \"< awk '{if($3 == 1) print}' data/f_combine\" u 1:2 notitle with points pointtype 2,");
	fprintf(plot_gaussian_predict, "     \"< awk '{if($3 == 0) print}' data/f_combine\" u 1:2 notitle with points pointtype 64,");
	fprintf(plot_gaussian_predict, "     'data/gaussian_kernel_predict' u 1:2 notitle with points pointtype 3\n");


	/************* Dataset 3 ********************/
	FILE *plot_sample_3;
	mat Xval, yval;

	f_combine = fopen("data/f_combine3", "w");
	f_X = fopen("data/X3.dat", "r");
	f_y = fopen("data/y3.dat", "r");
	data_num = 211;
	X = mat(data_num, 2);
	y = mat(data_num, 1);

	for (i = 0; i < data_num; i++)
	{
		fscanf(f_X, "%lf %lf\n", &X(i, 0), &X(i, 1));
		fscanf(f_y, "%lf", &y(i, 0));

		fprintf(f_combine, "%lf %lf %d\n", X(i, 0), X(i, 1), (int)y(i, 0));
		printf("%f, %f, %f\n", X(i, 0), X(i, 1), y(i, 0));
	}

	fclose(f_combine);
	fclose(f_X);
	fclose(f_y);

//	plot_sample_3 	= popen ("gnuplot -persistent", "w");
//	fprintf(plot_sample_3, "set title \"SVM Sample 3 data\"\n");
//	fprintf(plot_sample_3, "set xr [-0.6:0.3] \n");
//	fprintf(plot_sample_3, "set yr [-0.8:0.6] \n");
//	fprintf(plot_sample_3, "plot \"< awk '{if($3 == 1) print}' data/f_combine3\" u 1:2 notitle with points pointtype 2,");
//	fprintf(plot_sample_3, "     \"< awk '{if($3 == 0) print}' data/f_combine3\" u 1:2 notitle with points pointtype 64\n");

	/*************** Training SVM with RBF Kernel *****************/
	f_X = fopen("data/Xval3.dat", "r");
	f_y = fopen("data/yval3.dat", "r");
	data_num = 200;
	Xval = mat(data_num, 2);
	yval = mat(data_num, 1);

	for (i = 0; i < data_num; i++)
	{
		fscanf(f_X, "%lf %lf\n", &Xval(i, 0), &Xval(i, 1));
		fscanf(f_y, "%lf", &yval(i, 0));
	}

	dataset3Params(X, y, Xval, yval, C, sigma);
	model = svmTrain(X, y, C, gaussian, 0, 0, sigma);
	visualizeBoundary(X, y, model, "data/gaussian_kernel_predict_3");

	plot_sample_3 	= popen ("gnuplot -persistent", "w");
	fprintf(plot_sample_3, "set title \"SVM Gaussian kernel using dataset 3\"\n");
	fprintf(plot_sample_3, "set xr [-0.6:0.3] \n");
	fprintf(plot_sample_3, "set yr [-0.8:0.6] \n");
	fprintf(plot_sample_3, "plot \"< awk '{if($3 == 1) print}' data/f_combine3\" u 1:2 notitle with points pointtype 2,");
	fprintf(plot_sample_3, "     \"< awk '{if($3 == 0) print}' data/f_combine3\" u 1:2 notitle with points pointtype 64,");
	fprintf(plot_sample_3, "     'data/gaussian_kernel_predict_3' u 1:2 notitle with points pointtype 3\n");


	return 0;
}

