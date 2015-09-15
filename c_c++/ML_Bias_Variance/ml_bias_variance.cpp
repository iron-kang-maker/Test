/*
 * ml_bias_variance.cpp
 *
 *  Created on: Sep 12, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"
#include "fmincg.h"
#include <iostream>

using namespace std;
using namespace arma;

#define M_TRANING 12
#define M_TEST	  21

double sigmoid(double z)
{
	return (1/(1+exp(-z)));
}

double cost_func_reg(mat x, mat y, mat theta, double lamba)
{
	double ret, tmp, reg;
	int i, j, length, featureNum;

	length = x.n_rows;
	featureNum = x.n_cols;
	ret = tmp = 0;

	for ( i = 0; i < length; i++)
	{
		tmp = 0;
		for ( j = 0; j < featureNum; j++ )
			tmp += x(i, j)*theta(j, 0);


		ret += pow(tmp - y(i, 0), 2);;

	}

	ret = ret/(2*length);

	for ( i = 1; i < featureNum; i++ )
	{
		reg = reg + theta(i, 0)*theta(i, 0);
	}

	reg = reg*lamba;
	reg /= 2*length;

	return (ret + reg);
}

void gradient_reg(mat *grad, mat x, mat y, mat theta, double lamba)
{
	double ret, tmp, reg;
	int i, j, k, length, featureNum;
	mat grad_tmp(x.n_cols, 1);

	length = x.n_rows;
	featureNum = x.n_cols;

	for ( i = 0; i < featureNum; i++ )
	{
		ret = 0;
		reg = 0;

		for ( j = 0; j < length; j++ )
		{
			tmp = 0;
			for ( k = 0; k < featureNum; k++ )
				tmp += x(j, k)*theta(k, 0);

			ret += (tmp - y(j, 0))*x(j, i);
		}

		if ( i >= 1 )
			reg = theta(i, 0)*lamba/length;

		grad_tmp(i, 0) = ret/length + reg;
	}
	*grad = grad_tmp;
}

void costFunc(COSTDATA *cost_data, double* costVal, mat* grad)
{
	*costVal = cost_func_reg(
				cost_data->x,
				cost_data->y,
				*cost_data->theta,
				cost_data->lambda);

	gradient_reg(grad, cost_data->x, cost_data->y, *cost_data->theta, cost_data->lambda);
}

mat polynomial_mat(mat A, int ith)
{
	int i;
	mat tmp = A;

	for ( i = 0; i < ith; i++ )
	{
		A = A%tmp;
	}
	return A;
}

mat featureNormalize(mat A, mat &means, mat &std)
{
	mat mu = mean(A, 0);
	mat sigma;
	int i;

	for ( i = 0; i < (int)A.n_cols; i++ )
	{
		A.col(i) = A.col(i) - mu(0, i);
	}

	sigma = stddev(A, 0);

	for ( i = 0; i < (int)A.n_cols; i++ )
	{
		A.col(i) = A.col(i) / sigma(0, i);
	}

	means = mu;
	std = sigma;
	return A;
}

int main(void)
{
	int i;
	mat X(M_TRANING, 1), Xtest(M_TEST, 1), Xval(M_TEST, 1);
	mat y(M_TRANING, 1), ytest(M_TEST, 1), yval(M_TEST, 1);
	mat theta(2, 1), grad(2, 1), padded_mat, tmp_mat;
	double J;
	FILE *f_X, *f_Xtest, *f_Xval, *f_y, *f_ytest, *f_yval;
	FILE *hyp_line, *learning_curve, *learning_curve_poly;
	FILE *learning_curve_poly_comp, *learning_curve_lambda_comp;
	FILE *plot_training, *plot_learning_curve;
	FILE *plot_learning_curve_poly, *plot_learning_curve_poly_comp, *plot_learning_curve_lambda_comp;
	COSTDATA cost_data;

	f_X 	= fopen("data/X.dat", "r");
	f_Xtest = fopen("data/Xtest.dat", "r");
	f_Xval 	= fopen("data/Xval.dat", "r");
	f_y 	= fopen("data/y.dat", "r");
	f_ytest = fopen("data/ytest.dat", "r");
	f_yval 	= fopen("data/yval.dat", "r");

	hyp_line = fopen("data/hyp_line", "w");
	learning_curve = fopen("data/learning_curve", "w");
	learning_curve_poly = fopen("data/learning_curve_poly", "w");
	learning_curve_poly_comp = fopen("data/learning_curve_poly_comp", "w");
	learning_curve_lambda_comp = fopen("data/learning_curve_lambda_comp", "w");

	plot_training 	= popen ("gnuplot -persistent", "w");
	plot_learning_curve = popen ("gnuplot -persistent", "w");
	plot_learning_curve_poly = popen ("gnuplot -persistent", "w");
	plot_learning_curve_poly_comp = popen ("gnuplot -persistent", "w");
	plot_learning_curve_lambda_comp = popen ("gnuplot -persistent", "w");

	for ( i = 0; i < M_TRANING; i++ )
	{
		fscanf(f_X, "%lf", &X(i, 0));
		fscanf(f_y, "%lf", &y(i, 0));
	}

	for ( i = 0; i < M_TEST; i++ )
	{
		fscanf(f_Xtest, "%lf", &Xtest(i, 0));
		fscanf(f_Xval, "%lf", &Xval(i, 0));
		fscanf(f_ytest, "%lf", &ytest(i, 0));
		fscanf(f_yval, "%lf", &yval(i, 0));
	}

	fclose(f_X);
	fclose(f_Xtest);
	fclose(f_Xval);
	fclose(f_y);
	fclose(f_ytest);
	fclose(f_yval);

	theta(0, 0) = 1;
	theta(1, 0) = 1;
	padded_mat = mat(M_TRANING, 1);
	padded_mat.fill(1);
	J = cost_func_reg(join_rows(padded_mat, X), y, theta, 1);
	printf("Cost at theta = [1 ; 1]: %f (this value should be about 303.993192)\n", J);

	gradient_reg(&grad, join_rows(padded_mat, X), y, theta, 1);
	printf("Gradient at theta = [1 ; 1]:  [%f; %f] "
			"(this value should be about [-15.303016; 598.250744])\n", grad[0], grad[1]);

	cost_data.x = join_rows(padded_mat, X);
	cost_data.y = y;
	cost_data.theta = &theta;
	cost_data.lambda = 0;
	fmincg(&costFunc, &cost_data, 200);

	for (i = 0; i < M_TRANING; i++)
	{
		fprintf(hyp_line, "%f %f\n", X(i, 0), theta[0]+theta[1]*X(i,0));
	}
	fclose(hyp_line);

	fprintf(plot_training, "set title \"Training Data\"\n");
	fprintf(plot_training, "set xlabel \"Change in water level(x)\"\n");
	fprintf(plot_training, "set ylabel \"Water flowing out of the dam(y)\" \n");
	fprintf(plot_training, "set xr [-60.0:40.0] \n");
	fprintf(plot_training, "set yr [-10.0:40.0] \n");
	fprintf(plot_training, "plot 'data/ori.dat' using 1:2 t \"Training Data\" with points pointtype 1,");
	fprintf(plot_training, "     'data/hyp_line' using 1:2 t \"Hypothesis\" with lines\n");

	/***************** Learning curve *********************/
	mat sub_X, sub_y;

	for ( i = 0; i < M_TRANING; i++ )
	{
		padded_mat = mat(i+1, 1);
		padded_mat.fill(1);

		sub_X = join_rows(padded_mat, X.submat(0, 0, i, 0));
		sub_y = y.submat(0, 0, i, 0);
		theta.zeros();
		cost_data.x = sub_X;
		cost_data.y = sub_y;
		cost_data.theta = &theta;
		cost_data.lambda = 0;
		fmincg(&costFunc, &cost_data, 200);

		padded_mat = mat(Xval.n_rows, 1);
		padded_mat.fill(1);
		fprintf(learning_curve, "%d %f %f\n",
				i+1, cost_func_reg(sub_X, sub_y, theta, 0), cost_func_reg(join_rows(padded_mat, Xval), yval, theta, 0));
	}

	fprintf(plot_learning_curve, "set title \"Learning curve for linear regression\"\n");
	fprintf(plot_learning_curve, "set xlabel \"Number of training examples\"\n");
	fprintf(plot_learning_curve, "set ylabel \"Error\" \n");
	fprintf(plot_learning_curve, "set xr [0.0:13.0] \n");
	fprintf(plot_learning_curve, "set yr [0.0:150.0] \n");
	fprintf(plot_learning_curve, "plot 'data/learning_curve' using 1:2 t \"Train\" with lines,");
	fprintf(plot_learning_curve, "     'data/learning_curve' using 1:3 t \"Cross Validation\" with lines\n");


	/************** Feature Mapping for Polynomial Regression *************/
	int p = 8;
	mat mu, sigma;
	mat X_poly, X_poly_test, X_poly_val;

	// Map X onto Polynomial Features and Normalize
	X_poly = X;
	for ( i = 1; i < p; i++ )
	{
		X_poly = join_rows(X_poly, polynomial_mat(X, i));
	}

	X_poly = featureNormalize(X_poly, mu, sigma);
	padded_mat = mat(M_TRANING, 1);
	padded_mat.ones();
	X_poly = join_rows(padded_mat, X_poly);

	// Map X_poly_test and normalize (using mu and sigma)
	X_poly_test = Xtest;
	for ( i = 1; i < p; i++ )
	{
		X_poly_test = join_rows(X_poly_test, polynomial_mat(Xtest, i));
	}

	for ( i = 0; i < (int)X_poly_test.n_cols; i++ )
	{
		X_poly_test.col(i) = (X_poly_test.col(i) - mu(0, i)) / sigma(0, i);
	}

	padded_mat = mat(M_TEST, 1);
	padded_mat.ones();
	X_poly_test = join_rows(padded_mat, X_poly_test);

	// Map X_poly_val and normalize (using mu and sigma)
	X_poly_val = Xval;
	for ( i = 1; i < p; i++ )
	{
		X_poly_val = join_rows(X_poly_val, polynomial_mat(Xval, i));
	}

	for ( i = 0; i < (int)X_poly_val.n_cols; i++ )
	{
		X_poly_val.col(i) = (X_poly_val.col(i) - mu(0, i)) / sigma(0, i);
	}

	padded_mat = mat(M_TEST, 1);
	padded_mat.ones();
	X_poly_val = join_rows(padded_mat, X_poly_val);

	/*********** Learning Curve for Polynomial Regression ************/
	double tmpVal;
	mat maxVal, minVal,newX, newX_poly, one_mat;;
	theta = mat(X_poly.n_cols, 1);
	theta.zeros();
	cost_data.x = X_poly;
	cost_data.y = y;
	cost_data.theta = &theta;
	cost_data.lambda = 1;
	fmincg(&costFunc, &cost_data, 200);

	maxVal = max(X, 0);
	minVal = min(X, 0);
	tmpVal = minVal(0, 0) - 15;
	one_mat = mat(1, 1);
	one_mat(0, 0) = tmpVal;
	i = 0;
	newX = one_mat;
	while (tmpVal < maxVal(0, 0)+25)
	{
		tmpVal += 0.05;
		one_mat(0, 0) = tmpVal;
		newX = join_cols(newX, one_mat);

	}

	newX_poly = newX;
	for ( i = 1; i < p; i++ )
	{
		newX_poly = join_rows(newX_poly, polynomial_mat(newX, i));
	}

	for ( i = 0; i < (int)newX_poly.n_cols; i++ )
	{
		newX_poly.col(i) = (newX_poly.col(i) - mu(0, i)) / sigma(0, i);
	}

	padded_mat = mat(newX_poly.n_rows, 1);
	padded_mat.ones();
	newX_poly = join_rows(padded_mat, newX_poly);

	for (i = 0; i < (int)newX_poly.n_rows; i++)
	{
		tmp_mat = newX_poly.row(i)*theta;

		fprintf(learning_curve_poly, "%f %f\n", newX(i, 0), tmp_mat(0,0));
	}
	fclose(learning_curve_poly);

	fprintf(plot_learning_curve_poly, "set title \"Polynomial Regression Fit (lambda = 1)\"\n");
	fprintf(plot_learning_curve_poly, "set xlabel \"Change in water level (x)\"\n");
	fprintf(plot_learning_curve_poly, "set ylabel \"ater flowing out of the dam (y)\" \n");
	fprintf(plot_learning_curve_poly, "set xr [-100.0:100.0] \n");
	fprintf(plot_learning_curve_poly, "set yr [0.0:200.0] \n");
	fprintf(plot_learning_curve_poly, "plot 'data/ori.dat' using 1:2 notitle with points pointtype 1,");
	fprintf(plot_learning_curve_poly, "     'data/learning_curve_poly' using 1:2 notitle with lines\n");

	// learning curve with 8-th polynomial
	for ( i = 0; i < M_TRANING; i++ )
	{
		sub_X = X_poly.submat(0, 0, i, X_poly.n_cols-1);
		sub_y = y.submat(0, 0, i, 0);

		theta.zeros();
		cost_data.x = sub_X;
		cost_data.y = sub_y;
		cost_data.theta = &theta;
		cost_data.lambda = 1;
		fmincg(&costFunc, &cost_data, 200);

		/* Note: If you are using your cost function (linearRegCostFunction)
				 to compute the training and cross validation error, you should
		         call the function with the lambda argument set to 0.
		         Do note that you will still need to use lambda when running
		         the training to obtain the theta parameters.
		*/
		fprintf(learning_curve_poly_comp, "%d %g %g\n",
				i+1, cost_func_reg(sub_X, sub_y, theta, 0), cost_func_reg(X_poly_val, yval, theta, 0));
	}
	fclose(learning_curve_poly_comp);

	fprintf(plot_learning_curve_poly_comp, "set title \"Polynomial Regression Learning Curve(lambda = 1.0)\"\n");
	fprintf(plot_learning_curve_poly_comp, "set xlabel \"Number of training examples\"\n");
	fprintf(plot_learning_curve_poly_comp, "set ylabel \"Error\" \n");
	fprintf(plot_learning_curve_poly_comp, "set xr [0.0:13.0] \n");
	fprintf(plot_learning_curve_poly_comp, "set yr [0.0:150.0] \n");
	fprintf(plot_learning_curve_poly_comp, "plot 'data/learning_curve_poly_comp' using 1:2 t \"Train\" with lines,");
	fprintf(plot_learning_curve_poly_comp, "     'data/learning_curve_poly_comp' using 1:3 t \"Cross Validation\" with lines\n");

	/************* Validation for Selecting Lambda ***************/
	double lambdas[10] = { 0.0, 0.001, 0.003, 0.01, 0.03, 0.1, 0.3, 1, 3, 10 };

	for ( i = 0; i < 10; i++ )
	{
		theta.zeros();
		cost_data.x = X_poly;
		cost_data.y = y;
		cost_data.theta = &theta;
		cost_data.lambda = lambdas[i];
		fmincg(&costFunc, &cost_data, 200);
		fprintf(learning_curve_lambda_comp, "%g %g %g\n",
				lambdas[i], cost_func_reg(X_poly, y, theta, 0), cost_func_reg(X_poly_val, yval, theta, 0));
	}
	fclose(learning_curve_lambda_comp);

	fprintf(plot_learning_curve_lambda_comp, "set title \"Learning Curve with various lambda\"\n");
	fprintf(plot_learning_curve_lambda_comp, "set xlabel \"lambda\"\n");
	fprintf(plot_learning_curve_lambda_comp, "set ylabel \"Error\" \n");
	fprintf(plot_learning_curve_lambda_comp, "set xr [0.0:10.0] \n");
	fprintf(plot_learning_curve_lambda_comp, "set yr [0.0:20.0] \n");
	fprintf(plot_learning_curve_lambda_comp, "plot 'data/learning_curve_lambda_comp' using 1:2 t \"Train\" with lines,");
	fprintf(plot_learning_curve_lambda_comp, "     'data/learning_curve_lambda_comp' using 1:3 t \"Cross Validation\" with lines\n");


	return 0;
}
