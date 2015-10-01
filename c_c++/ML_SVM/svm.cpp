/*
 * svm.cpp
 *
 *  Created on: Sep 24, 2015
 *      Author: iron
 */
#include "svm.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define _max(a, b) (a > b ? a : b)
#define _min(a, b) (a < b ? a : b)
#define abs(x)	  (x>=0 ? x : -x)

double linearKernel(mat x1, mat x2)
{
	double ret;
	mat tmp = x1.t()*x2;

	ret = tmp(0, 0);

	return ret;
}

double gaussianKernel(mat x1, mat x2, double sigma)
{
	double ret;

	ret = exp(-accu((x1 - x2)%(x1 - x2)) / (2*sigma*sigma));

	return ret;
}

double svmPredict(MODEL model, mat X, KERNELFUNCTION kernel, double sigma)
{
	int m = X.n_rows, i, j;
//	int pred;
	//mat pred = mat(m, 1);
	mat p(m, 1);
	mat X1, X2, K;

//	pred.zeros();
	p.zeros();

	if (kernel == gaussian)
	{
		double k;

		X1 = sum(X%X, 1);
		X2 = sum(model.X%model.X, 1);
		K = -2*X*model.X.t();
		X2 = X2.t();

//		printf("X1(%d, %d), X2(%d, %d), K(%d, %d)\n",
//				(int)X1.n_rows, (int)X1.n_cols, (int)X2.n_rows, (int)X2.n_cols, (int)K.n_rows, (int)K.n_cols);
		for (i = 0; i < (int)K.n_rows; i++)
		{
			K.row(i) = K.row(i) + X2;
		}

		for (i = 0; i < (int)K.n_cols; i++)
		{
			K.col(i) = K.col(i) + X1;
		}

		k = gaussianKernel(mat(1, 1).ones(), mat(1, 1).zeros(), sigma);
		for (i = 0; i < (int)K.n_rows; i++)
		{
			for (j = 0; j < (int)K.n_cols; j++)
			{
				K(i, j) = pow(k, K(i, j));
			}
		}

		for (i = 0; i < (int)K.n_rows; i++)
		{
			for (j = 0; j < (int)K.n_cols; j++)
			{
				K(i, j) = K(i, j)*model.y(j, 0)*model.alphas(j, 0);
			}
		}

		p = sum(K, 1);
	}

//	pred.copy_size(p);

//	for (i = 0; i < (int)pred.n_rows; i++)
//	{
//		if (p(i, 0) >= 0)
//			pred(i, 0) = 1;
//		else
//			pred(i, 0) = 0;
//	}
//	if (p(0, 0) >= 0)
//		pred = 1;
//	else
//		pred = 0;

	return (double)p(0, 0);//pred;
}

MODEL svmTrain(mat X, mat Y, double C, KERNELFUNCTION kernel, double tol, int max_passes, double sigma)
{
	MODEL model;
	int m, i, j, passes, dots;
	int num_changed_alphas, num;
	mat alphas, E, K, X2;
	double alpha_i_old, alpha_j_old, b1, b2, b, eta, L, H;

	if (tol == 0) tol = 0.001;
	if (max_passes == 0) max_passes = 5;

	m = X.n_rows;
	alphas = mat(m, 1);
	alphas.zeros();
	E = mat(m, 1);
	E.zeros();
	passes = 0;
	eta = 0;
	L = 0;
	H = 0;
	dots = 12;
	b = 0;

	for (i = 0; i < (int)Y.n_rows; i++)
	{
		if (Y(i, 0) == 0)
			Y(i, 0) = -1;
	}

	if (kernel == linear)
	{
		K = X*X.t();
		model.kernelFunciton = linearKernel;
	}
	else if (kernel == gaussian)
	{
		mat tmp = -2*X*X.t();
		double k;
		X2 = sum(X%X, 1);
		for (i = 0; i < (int)tmp.n_rows; i++)
		{
			tmp.row(i) = tmp.row(i) + X2.t();
		}

		for (i = 0; i < (int)tmp.n_cols; i++)
		{
			tmp.col(i) = tmp.col(i) + X2;
		}

		K.copy_size(tmp);
		k = gaussianKernel(mat(1, 1).ones(), mat(1, 1).zeros(), sigma);
		for (i = 0; i < (int)tmp.n_rows; i++)
		{
			for (j = 0; j < (int)tmp.n_cols; j++)
			{
				K(i, j) = pow(k, tmp(i, j));
			}
		}
	}

	srand( (unsigned)time(NULL));

	while (passes < max_passes)
	{
		num_changed_alphas = 0;

		for (i = 0; i < m; i++)
		{
			// Calculate Ei = f(x(i)) - y(i) using (2).
			// E(i) = b + sum (X(i, :) * (repmat(alphas.*Y,1,n).*X)') - Y(i);
			E(i, 0) = b + accu(alphas%Y%K.col(i)) - Y(i, 0);

			if ((Y(i, 0)*E(i, 0) < -tol && alphas(i, 0) < C) || (Y(i)*E(i) > tol && alphas(i, 0) > 0))
			{
				// In practice, there are many heuristics one can use to select
				// the i and j. In this simplified code, we select them randomly.
				j = ceil(m*(rand()%1000)/1000);

				while (j == i) j = ceil(m*(rand()%1000)/1000); // Make sure i =\= j
				// Calculate Ej = f(x(j)) - y(j) using (2).
				E(j, 0) = b + accu(alphas%Y%K.col(j)) - Y(j, 0);

				// Save old alphas
				alpha_i_old = alphas(i, 0);
				alpha_j_old = alphas(j, 0);

				// Compute L and H by (10) or (11).
				if (Y(i, 0) == Y(j, 0))
				{
					L = _max(0, alphas(j, 0) + alphas(i, 0) - C);
					H = _min(C, alphas(j, 0) + alphas(i, 0));
				}
				else
				{
					L = _max(0, alphas(j, 0) - alphas(i, 0));
					H = _min(C, C + alphas(j, 0) - alphas(i, 0));
				}

				if (L == H)	// continue to next i.
					continue;

				// Compute eta by (14).
				eta = 2*K(i,j) - K(i,i) - K(j,j);
				if (eta >= 0) // continue to next i.
					continue;

				// Compute and clip new value for alpha j using (12) and (15).
				alphas(j, 0) = alphas(j, 0) - (Y(j, 0) * (E(i, 0) - E(j, 0))) / eta;

				// Clip
				alphas(j, 0) = _min (H, alphas(j, 0));
				alphas(j, 0) = _max (L, alphas(j, 0));

				// Check if change in alpha is significant
				if (abs(alphas(j, 0) - alpha_j_old) < tol)
				{
					//continue to next i.
					//replace anyway
					alphas(j, 0) = alpha_j_old;
					continue;
				}

				// Determine value for alpha i using (16).
				alphas(i, 0) = alphas(i, 0) + Y(i, 0)*Y(j, 0)*(alpha_j_old - alphas(j, 0));

				// Compute b1 and b2 using (17) and (18) respectively.
				b1 = b - E(i, 0) - Y(i, 0) * (alphas(i, 0) - alpha_i_old) *  K(i,j)
					             - Y(j, 0) * (alphas(j, 0) - alpha_j_old) *  K(i,j);
				b2 = b - E(j, 0) - Y(i, 0) * (alphas(i, 0) - alpha_i_old) *  K(i,j)
					             - Y(j, 0) * (alphas(j, 0) - alpha_j_old) *  K(j,j);

				// Compute b by (19).
				if (0 < alphas(i, 0) && alphas(i, 0) < C)
					b = b1;
				else if (0 < alphas(j, 0) && alphas(j, 0) < C)
					b = b2;
				else
					b = (b1+b2)/2;


				num_changed_alphas = num_changed_alphas + 1;

			}
		}

		if (num_changed_alphas == 0)
			passes = passes + 1;
		else
			passes = 0;

		printf(".");
		dots = dots + 1;
		if (dots > 78)
		{
			dots = 0;
			printf("\n");
		}
	}

	printf("Done! \n\n");

	num = 0;
	// Save the model
	for (i = 0; i < m; i++)
	{
		if (alphas(i, 0) > 0)
			num++;
	}

	mat tmp1(num, 2), tmp2(num, 1), tmp3(num, 1);
	printf("num = %d\n", num);
	num = 0;
	for (i = 0; i < m; i++ )
	{
		if (alphas(i, 0) > 0)
		{
			tmp1.row(num) = X.row(i);
			tmp2.row(num) = Y.row(i);
			tmp3.row(num) = alphas.row(i);
			num++;
		}
	}

	model.X = tmp1;
	model.y = tmp2;
	model.b = b;
	model.alphas = tmp3;
	model.w = ((alphas%Y).t()*X).t();

//	model.X.print("X: ");
//	model.y.print("y: ");
//	model.alphas.print("alphas: ");
//	model.w.print("w: ");
//	printf("b = %f\n", b);

	return model;
}
