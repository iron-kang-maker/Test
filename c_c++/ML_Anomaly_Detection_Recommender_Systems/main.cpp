/*
 * main.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "fmincg.h"
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"

using namespace std;
using namespace arma;

void estimateGaussian(mat A, mat &means, mat &sigma2)
{
	mat mu = mean(A, 0);
	means = mu;
	sigma2 = mat(1, A.n_cols);
	int i;

	for ( i = 0; i < (int)A.n_cols; i++ )
	{
		sigma2(0, i) = accu((A.col(i) - mu(0, i))%(A.col(i) - mu(0, i)))/A.n_rows;
	}
}

void multivariateGaussian(mat X, mat mu, mat sigma2, mat &p)
{
	int k = mu.n_cols;
	int dim, i;
	mat sigma;

	p = mat(X.n_rows, 1);

	if (mu.n_cols == 1 || mu.n_rows == 1)
	{
		dim = max(mu.n_cols, mu.n_rows);
		sigma = mat(dim, dim);
		sigma.zeros();
		for (i = 0; i < dim; i++)
			sigma(i, i) = sigma2(0, i);
	}

	for (i = 0; i < (int)X.n_rows; i++)
		X.row(i) = X.row(i) - mu;

	p = pow(2*M_PI, -k/2)*pow(det(sigma), -0.5)*exp(-0.5*sum(X*pinv(sigma)%X, 1));

}

void visualizeFit(mat X, mat mu, mat sigma2, mat *outlier)
{
	int grid_size = 35*2;
	int i;
	float step;
	FILE *contour_data = fopen("data/contour_data", "w");
	FILE *plot_contour;
	mat X1(grid_size + 1, grid_size + 1), X2(grid_size + 1, grid_size + 1);
	mat append, p;

	X1.ones();
	step = 0;
	for (i = 0; i <= grid_size; i++)
	{
		X1.col(i) = step*X1.col(i);
		step += 0.5;
	}
	X2 = X1.t();

	X1.reshape((grid_size+1)*(grid_size+1), 1);
	X2.reshape((grid_size+1)*(grid_size+1), 1);

//	mu.print("mu");
//	sigma2.print("sigma");

	append = join_rows(X1, X2);
	multivariateGaussian(append, mu, sigma2, p);

	plot_contour 	= popen ("gnuplot -persistent", "w");

	if (outlier)
	{
		for (i = 0; i < (int)outlier->n_rows; i++)
		{
			fprintf(plot_contour, "set object %d circle at %f,%f radius char 0.5 fillstyle empty border lc rgb '#FF0000' lw 2\n",
							i+1, X((*outlier)(i, 0), 0), X((*outlier)(i, 0), 1));
		}
	}

	for (i = 0; i < (int)p.n_rows; i++)
	{
		fprintf(contour_data, "%g %g %g\n", append(i, 0), append(i, 1), p(i, 0));
		if ((i+1) % 71 == 0)
			fprintf(contour_data, "\n");
	}
	fflush(contour_data);
	fclose(contour_data);

	for (i = 0; i < (int)X.n_rows; i++)
	{
		fprintf(plot_contour, "set object %d circle at %f,%f radius char 0.05 fillstyle empty border lc rgb '#0000FF' lw 2\n",
				i+1, X(i, 0), X(i, 1));
	}

	if (outlier)
	{
		for (i = 0; i < (int)outlier->n_rows; i++)
		{
			cout<<i<<" id: "<<(*outlier)(i, 0)<<endl;
			fprintf(plot_contour, "set object %d circle at %f,%f radius char 0.5 fillstyle empty border lc rgb '#FF0000' lw 2\n",
							i+X.n_rows, X((*outlier)(i, 0), 0), X((*outlier)(i, 0), 1));
		}
	}

	fprintf(plot_contour, "set title \"multivariateGaussian\"\n");
	fprintf(plot_contour, "set contour\n");
	fprintf(plot_contour, "set size square\n");
	fprintf(plot_contour, "unset surface\n");
	fprintf(plot_contour, "set pm3d map\n");
	fprintf(plot_contour, "set xlabel \"Latency (ms)\"\n");
	fprintf(plot_contour, "set ylabel \"Throughput (mb/s)\" \n");
	fprintf(plot_contour, "set cntrparam levels discrete 10e-20, 10e-17, 10e-14, 10e-11, 10e-8, 10e-5, 10e-2\n");
	fprintf(plot_contour, "splot 'data/contour_data' using 1:2:3 notitle\n ");
	fflush(plot_contour);
}

void selectThreshold(mat yval, mat pval, double &bestEpsilon, double &bestF1)
{
	bestEpsilon = 0;
	bestF1 = 0;
	double F1 = 0, prec, rec, tp, fp, fn;
	int i = 1;
	double stepsize = (pval.max() - pval.min()) / 1000;
	double epsilon;
	mat predictions;

	predictions.copy_size(pval);
	predictions.zeros();

	for (epsilon = pval.min(); epsilon <= pval.max(); epsilon += stepsize)
	{
		tp = fp = fn = prec = rec = 0;
		for (i = 0; i < (int)pval.n_rows; i++)
		{
			if (pval(i, 0) < epsilon)
				predictions(i, 0) = 1;
		}


		for (i = 0; i < (int)yval.n_rows; i++)
		{
			tp += predictions(i, 0)*yval(i, 0);
			if (yval(i, 0) == 0)
				fp += predictions(i, 0)*1;

			if (predictions(i, 0) == 0)
				fn += yval(i, 0)*1;
		}

		if ((tp+fp) != 0)
			prec = tp/(tp+fp);

		if ((tp+fn) != 0)
			rec = tp/(tp+fn);

		if ((prec+rec) != 0)
			F1 = 2*prec*rec/(prec+rec);

		if (F1 > bestF1)
		{
			bestF1 = F1;
			bestEpsilon = epsilon;
		}
	}
}

/* Notes: X - num_movies  x num_features matrix of movie features
 *        Theta - num_users  x num_features matrix of user features
 *        Y - num_movies x num_users matrix of user ratings of movies
 *        R - num_movies x num_users matrix, where R(i, j) = 1 if the
 *        i-th movie was rated by the j-th user
 */
//double cofiCostFunc(mat params, mat Y, mat R, mat &grad, int num_users, int num_movies, int num_features, float lambda)
void cofiCostFunc(COSTDATA *cost_data, double* costVal, mat* grad)
{
	mat X = reshape( cost_data->params->submat(0, 0,
			                                   cost_data->num_movies*cost_data->num_features -1, 0 ),
			         cost_data->num_movies, cost_data->num_features );
	mat Theta = reshape( cost_data->params->submat(cost_data->num_movies*cost_data->num_features, 0,
			                                       cost_data->params->n_rows -1 , 0),
			             cost_data->num_users, cost_data->num_features );

	double J = 0;
	int i;
	mat X_grad, Theta_grad, tmp;
	mat Theta_t, Y_t, X_t;
	uvec idx;

	X_grad.copy_size(X);
	Theta_grad.copy_size(Theta);
	X_grad.zeros();
	Theta_grad.zeros();

	tmp = (X*Theta.t() - cost_data->Y)%cost_data->R;
	J = accu(tmp%tmp)/2 + accu(pow(Theta, 2))*cost_data->lambda/2 + accu(pow(X, 2))*cost_data->lambda/2;

	for ( i = 0; i < (int)X.n_rows; i++ )
	{
		idx = find(cost_data->R.row(i) == 1);
		Theta_t = Theta.rows(idx);
		tmp = cost_data->Y.row(i);
		Y_t = tmp.cols(idx);
		X_grad.row(i) = (X.row(i)*Theta_t.t() - Y_t)*Theta_t + cost_data->lambda*X.row(i);

	}

	for ( i = 0; i < (int)Theta.n_rows; i++ )
	{
		idx = find(cost_data->R.col(i) == 1);
		X_t = X.rows(idx);
		tmp = cost_data->Y.col(i);
		Y_t = tmp.rows(idx);
		Theta_grad.row(i) = (X_t*Theta.row(i).t() - Y_t).t()*X_t + cost_data->lambda*Theta.row(i);
	}

	*grad = join_cols( reshape( X_grad, X_grad.n_elem, 1 ), reshape( Theta_grad, Theta_grad.n_elem, 1) );

	*costVal = J;
}

void checkCostFunction(float lambda)
{
	mat X_t = randu<mat>(4, 3);
	mat Theta_t = randu<mat>(5, 3);
	mat Y = X_t*Theta_t.t();
	mat R;
	int num_users = Y.n_cols;
	int num_movies = Y.n_rows;
	int num_features = Theta_t.n_cols;

	R.copy_size(Y);
	R.zeros();

	mat a = randu<mat>(Y.n_rows, Y.n_cols);
	Y.elem(find(a > 0.5)).zeros();
	R.elem(find(Y != 0)).ones();

	mat X = randu<mat>(X_t.n_rows, X_t.n_cols);
	mat Theta = randu<mat>(Theta_t.n_rows, Theta_t.n_cols);
	mat param, tmp;
	param = join_cols(reshape(X, X.n_elem, 1), reshape(Theta, Theta.n_elem, 1));

	mat numgrad, perturb, grad;
	numgrad.copy_size(param);
	perturb.copy_size(param);
	numgrad.zeros();
	perturb.zeros();
	double e = 1e-4;
	double loss1, loss2;
	COSTDATA cost_data;
	cost_data.num_features = num_features;
	cost_data.num_movies = num_movies;
	cost_data.num_users = num_users;
	cost_data.Y = Y;
	cost_data.R = R;
	cost_data.lambda = lambda;

	for ( int i = 0; i < (int)param.n_elem; i++ )
	{
		// Set perturbation vector
		perturb(i, 0) = e;
		tmp = param - perturb;
		cost_data.params = &tmp;
//		loss1 = cofiCostFunc((param - perturb), Y, R, grad, num_users, num_movies, num_features, lambda);
		cofiCostFunc( &cost_data, &loss1, &grad);
		tmp = param + perturb;
		cost_data.params = &tmp;
//		loss2 = cofiCostFunc((param + perturb), Y, R, grad, num_users, num_movies, num_features, lambda);
		cofiCostFunc( &cost_data, &loss2, &grad);
		// Compute Numerical Gradient
		numgrad(i, 0) = (loss2 - loss1)/(2*e);
		perturb(i, 0) = 0;
	}

	cost_data.params = &param;
//	cofiCostFunc(param, Y, R, grad, num_users, num_movies, num_features, lambda);
	cofiCostFunc( &cost_data, &loss2, &grad);
	join_rows(numgrad, grad).print("grad");
	printf("The above two columns you get should be very similar.\n"
			"(Left-Your Numerical Gradient, Right-Analytical Gradient)\n");

	double diff = norm(numgrad - grad)/norm(numgrad + grad);
	printf("If your backpropagation implementation is correct, then\n"
			"the relative difference will be small (less than 1e-9). \n"
			"\nRelative Difference: %g\n", diff);
}

vector<string> loadMovieList()
{
	ifstream fin("data/movie_ids.txt", ios::in);
	string str;
	vector<string> movieList;

	while (getline(fin, str))
		movieList.push_back(str);

	return movieList;
}

void normalizeRatings(mat &Ynorm, mat &Ymean, mat Y, mat R)
{
	mat means(Y.n_rows, 1);
	mat norms, tmp;
	uvec idx;

	means.zeros();
	norms.copy_size(Y);
	norms.zeros();

	for (int i = 0; i < (int)Y.n_rows; i++)
	{
		idx = find(R.row(i) == 1);
		tmp = Y.row(i);

		means(i, 0) = mean(mean(tmp.cols(idx), 1));

		for (int j = 0; j < (int)idx.n_elem; j++)
		{
			norms(i, idx(j)) = Y(i, idx(j)) - means(i, 0);
		}
	}

//	means.print("means");
//	norms.print("norms");
	Ymean = means;
	Ynorm = norms;

}

int main(void)
{
	//================== Part 1: Load Example Dataset  ===================
	int i, j, sample_num;
	mat X;
	FILE *plot_sample;
	fstream fin;

	fin.open("data/X.dat",ios::in);
	sample_num = 307;
	X = mat(sample_num, 2);
	plot_sample 	= popen ("gnuplot -persistent", "w");

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < 2; j++)
		{
			fin>>X(i, j);
		}
	}

	fin.close();

	fprintf(plot_sample, "set title \"Raw data\"\n");
	fprintf(plot_sample, "set xlabel \"Latency (ms)\"\n");
	fprintf(plot_sample, "set ylabel \"Throughput (mb/s)\" \n");
	fprintf(plot_sample, "set size square\n");
	fprintf(plot_sample, "set xr [0.0:30.0] \n");
	fprintf(plot_sample, "set yr [0.0:30.0] \n");
	fprintf(plot_sample, "plot 'data/X.dat' using 1:2 notitle\n");
	fflush(plot_sample);

	//================== Part 2: Estimate the dataset statistics ===================
	mat mu, sigma2, p;
	estimateGaussian(X, mu, sigma2);
	multivariateGaussian(X, mu, sigma2, p);
	visualizeFit(X, mu, sigma2, NULL);

	//================== Part 3: Find Outliers ===================
	mat Xval(sample_num, 2), yval(sample_num, 1);
	mat pval, id(1,1), tmp(1, 1);
	fstream fin_Xval, fin_yval;
	double bestEpsilon, bestF1;

	fin_Xval.open("data/Xval.dat",ios::in);
	fin_yval.open("data/yval.dat",ios::in);

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < 2; j++)
		{
			fin_Xval>>Xval(i, j);
		}
		fin_yval>>yval(i, 0);
	}

	fin_Xval.close();
	fin_yval.close();

	multivariateGaussian(Xval, mu, sigma2, pval);
	selectThreshold(yval, pval, bestEpsilon, bestF1);
	cout<<"bestEpsilon: "<<bestEpsilon<<", bestF1: "<<bestF1<<endl;
	id.zeros();
	for (i = 0; i < (int)p.n_rows; i++)
	{
		if (p(i, 0) < bestEpsilon)
		{
			if (id(0, 0) == 0)
				id(0, 0) = i;
			else {
				tmp(0, 0) = i;
				id = join_cols(id, tmp);
			}
		}
	}

	visualizeFit(X, mu, sigma2, &id);

	//================== Part 4: Multidimensional Outliers ===================
	mat X_2(1000, 11), Xval_2(100, 11), yval_2(100, 1);
	int cnt = 0;

	fin.open("data/X_2.dat",ios::in);
	fin_Xval.open("data/Xval_2.dat",ios::in);
	fin_yval.open("data/yval_2.dat",ios::in);

	for ( i = 0; i < 1000; i ++ )
	{
		for ( j = 0; j < 11; j++ )
		{
			fin>>X_2(i, j);
		}
	}

	for ( i = 0; i < 100; i ++ )
	{
		for ( j = 0; j < 11; j++ )
			fin_Xval>>Xval_2(i, j);
		fin_yval>>yval_2(i, 0);
	}

	estimateGaussian(X_2, mu, sigma2);
	multivariateGaussian(X_2, mu, sigma2, p);
	multivariateGaussian(Xval_2, mu, sigma2, pval);
	selectThreshold(yval_2, pval, bestEpsilon, bestF1);

	for (i = 0; i < (int)p.n_rows; i++)
	{
		if (p(i, 0) < bestEpsilon)
		{
			cnt++;
		}
	}

	printf("Best epsilon found using cross-validation: %e\n", bestEpsilon);
	printf("Best F1 on Cross Validation Set:  %f\n", bestF1);
	printf("# Outliers found: %d\n", cnt);

	//=============== Part 1: Loading movie ratings dataset ================
	int num_movies = 1682;
	int num_users = 943;
	float mean;
	mat Y(num_movies, num_users), R(num_movies, num_users);
	fstream fin_Y, fin_R;

	fin_Y.open("data/Y.dat",ios::in);
	fin_R.open("data/R.dat",ios::in);
	cnt = 0;

	for ( i = 0; i < num_movies; i++ )
	{
		for ( j = 0; j < num_users; j++ )
		{
			fin_Y>>Y(i, j);
			fin_R>>R(i, j);
		}
	}

	fin_Y.close();
	fin_R.close();

	for ( j = 0; j < num_users; j++ )
	{
		if ( R(0, j) == 1)
		{
			cnt++;
			mean += Y(0, j);
		}
	}

	mean /= cnt;

	printf("Average rating for movie 1 (Toy Story): %f / 5\n", mean);
	getchar();

	//============ Part 2: Collaborative Filtering Cost Function ===========
	int num_features = 10;
	double J;
	mat Theta(num_users, num_features);
	mat grad;
	fstream fin_X, fin_Theta;

	X = mat(num_movies, num_features);
	fin_X.open("data/X_movies.dat",ios::in);
	fin_Theta.open("data/Theta.dat",ios::in);

	for ( i = 0; i < num_movies; i++ )
	{
		for ( j = 0; j < num_features; j++ )
			fin_X>>X(i, j);
	}

	for ( i = 0; i < num_users; i++ )
	{
		for ( j = 0; j < num_features; j++ )
			fin_Theta>>Theta(i, j);
	}

	num_users = 4;
	num_movies = 5;
	num_features = 3;

	mat X_sub = X.submat(0, 0, num_movies - 1, num_features - 1);
	mat Y_sub = Y.submat(0, 0, num_movies - 1, num_users - 1);
	mat Theta_sub = Theta.submat(0, 0, num_users - 1, num_features - 1);
	mat R_sub = R.submat(0, 0, num_movies - 1, num_users - 1);

	mat param;
	param = join_cols(reshape(X_sub, X_sub.n_elem, 1), reshape(Theta_sub, Theta_sub.n_elem, 1));

	COSTDATA cost_data;
	cost_data.num_features = num_features;
	cost_data.num_movies = num_movies;
	cost_data.num_users = num_users;
	cost_data.Y = Y_sub;
	cost_data.R = R_sub;
	cost_data.lambda = 0;
	cost_data.params = &param;

	cofiCostFunc( &cost_data, &J, &grad);
	printf("Cost at loaded parameters: %f \n(this value should be about 22.22)\n", J);
	getchar();

	//============== Part 3: Collaborative Filtering Gradient ==============
	checkCostFunction(0);

	//========= Part 4: Collaborative Filtering Cost Regularization ========
	cost_data.lambda = 1.5;
	cofiCostFunc( &cost_data, &J, &grad);
	printf("Cost at loaded parameters (lambda = 1.5): %f"
			"\n(this value should be about 31.34)\n", J);
	getchar();

	//======= Part 5: Collaborative Filtering Gradient Regularization ======
	checkCostFunction(1.5);

	//============== Part 6: Entering ratings for a new user ===============
	int n = 1682;
	mat my_ratings(n, 1);
	vector<string> movieList = loadMovieList();

	my_ratings.zeros();
	my_ratings(0, 0) = 4;
	my_ratings(97, 0) = 2;
	my_ratings(6, 0) = 3;
	my_ratings(11, 0) = 5;
	my_ratings(53, 0) = 4;
	my_ratings(63, 0) = 5;
	my_ratings(65, 0) = 3;
	my_ratings(68, 0) = 5;
	my_ratings(182, 0) = 4;
	my_ratings(225, 0) = 5;
	my_ratings(354, 0) = 5;

	for (i = 0; i < n; i++)
	{
		if (my_ratings(i, 0) > 0)
			cout<<"Rated "<<my_ratings[i]<<" for "<<movieList[i]<<endl;
	}

	//================== Part 7: Learning Movie Ratings ====================
	mat Ymean, Ynorm;
	/* Add our own ratings to the data matrix */
	Y = join_rows(my_ratings, Y);
	my_ratings.elem(find(my_ratings != 0)).ones();
	R = join_rows(my_ratings, R);

	/* Normalize Ratings */
	normalizeRatings(Ynorm, Ymean, Y, R);
	num_users = Y.n_cols;
	num_movies = Y.n_rows;
	num_features = 10;

	/* Set Initial Parameters (Theta, X) */
	X = randn<mat>(num_movies, num_features);
	Theta = randn<mat>(num_users, num_features);
	mat initial_parameters = join_cols(reshape(X, X.n_elem, 1), reshape(Theta, Theta.n_elem, 1));
	double lambda = 10;
	cost_data.lambda = lambda;
	cost_data.R = R;
	cost_data.Y = Y;
	cost_data.params = &initial_parameters;
	cost_data.num_features = num_features;
	cost_data.num_movies = num_movies;
	cost_data.num_users = num_users;

	fmincg(&cofiCostFunc, &cost_data, 100);
	/* Unfold the returned theta back into U and W */
	X = reshape(cost_data.params->submat(0, 0, num_movies*num_features-1, 0), num_movies, num_features);
	Theta = reshape(cost_data.params->submat(num_movies*num_features, 0, cost_data.params->n_elem-1, 0), num_users, num_features);
	cout<<"Recommender system learning completed."<<endl;

	//================== Part 8: Recommendation for you ====================
	p = X*Theta.t();
	mat my_predictions = p.col(0) + Ymean;
	uvec ix = sort_index(my_predictions, "descend");

	printf("\nTop recommendations for you:\n");
	for (i = 0; i < 10; i++)
	{
		printf("Predicting rating %.1f for movie %s\n", my_predictions(ix(i), 0), movieList[ix(i)].c_str());
	}

	return 0;
}




