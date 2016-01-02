/*
 * main.cpp
 *
 *  Created on: Oct 14, 2015
 *      Author: iron
 */
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"

using namespace arma;
using namespace std;
using namespace cv;

#define fabs(x) x>=0 ? x:-x
#define _max(a, b) (a > b ? a : b)
#define _min(a, b) (a < b ? a : b)

mat findClosestCentroids(mat X, mat centroids)
{
	mat idx, tmp_mat;
	int K = (int)centroids.n_rows;
	double dist = 65536, id = 0;
	int i, j;
	double tmp;

	idx = mat(X.n_rows, 1);

	for (i = 0; i < (int)X.n_rows; i++)
	{
		for (j = 0; j < K; j++)
		{
			tmp_mat = X.row(i) - centroids.row(j);
			tmp = accu(tmp_mat%tmp_mat);
			if (tmp < dist)
			{
				dist = tmp;
				id = j+1;
			}
		}
		idx(i, 0) = id;
		id = 0;
		dist = 65536;
	}

	return idx;
}

mat computeCentroids(mat X, mat idx, int K)
{
	mat centroids(K, X.n_cols);
	mat tmp(1, X.n_cols);
	uvec vec;
	int i, j;

	tmp.zeros();

	for (i = 0; i < K; i++)
	{
		tmp.zeros();
		vec = find(idx == i+1);
		for (j = 0; j < (int)vec.n_rows; j++)
		{
			tmp = tmp + X.row(vec(j));
		}

		centroids.row(i) = tmp/vec.n_rows;
	}

	return centroids;
}

void runkMeans(mat X, mat initial_centroids, int max_iters, mat &centroids, mat &idx, bool display)
{
	FILE *f_centroids, *plot_kmean;
	int m = (int)X.n_rows;
	int K = (int)initial_centroids.n_rows;
	int i, j, k, num;
	mat previous_centroids;
	uvec vec;

	idx = mat(m, 1);

	idx.zeros();
	centroids = initial_centroids;
	f_centroids = fopen("data/f_centroids", "w");
	plot_kmean 	= popen ("gnuplot -persistent", "w");

	for (j = 0; j < K; j++)
	{
		fprintf(f_centroids, "%lf %lf ", centroids.row(j)(0, 0), centroids.row(j)(0, 1));
	}
	num = 0;
	fprintf(f_centroids,"\n");
	fflush(f_centroids);
	for (i = 0; i < max_iters; i++)
	{
		printf("K-Means iteration %d/%d...\n", i+1, max_iters);
		idx = findClosestCentroids(X, centroids);
		centroids = computeCentroids(X, idx, K);
		for (j = 0; j < K; j++)
		{
			fprintf(f_centroids, "%lf %lf ", centroids.row(j)(0, 0), centroids.row(j)(0, 1));
			vec = find(idx == j+1);

			for (k = 0; k < (int)vec.n_rows; k++)
			{

				if (j == 0)
					fprintf(plot_kmean, "set object %d circle at %lf,%lf radius char 0.6 fillstyle empty border lc rgb '#FF0000' lw 1\n",
								++num, X.row(vec(k))(0, 0), X.row(vec(k))(0, 1));
				else if (j == 1)
					fprintf(plot_kmean, "set object %d circle at %lf,%lf radius char 0.6 fillstyle empty border lc rgb '#00FF00' lw 1\n",
								++num, X.row(vec(k))(0, 0), X.row(vec(k))(0, 1));
				else if (j == 2)
					fprintf(plot_kmean, "set object %d circle at %lf,%lf radius char 0.6 fillstyle empty border lc rgb '#0000FF' lw 1\n",
								++num, X.row(vec(k))(0, 0), X.row(vec(k))(0, 1));

			}

		}

		fprintf(f_centroids,"\n");
		fflush(f_centroids);
		if (!display)
			continue;
		fprintf(plot_kmean, "set title \"iteration number %d\"\n", i+1);
		fprintf(plot_kmean, "set size square\n");
		fprintf(plot_kmean, "set xr [-2.0:10.0] \n");
		fprintf(plot_kmean, "set yr [0.0:6.0] \n");
		fprintf(plot_kmean, "plot 'data/f_centroids' using 1:2 notitle lc rgb '#000000' with linespoints,");
		fprintf(plot_kmean, "     'data/f_centroids' using 3:4 notitle lc rgb '#000000' with linespoints,");
		fprintf(plot_kmean, "     'data/f_centroids' using 5:6 notitle lc rgb '#000000' with linespoints\n");
		fflush(plot_kmean);
		//getchar();
		sleep(1);
		num = 0;
	}

	fclose(f_centroids);

}

mat kMeansInitCentroids(mat X, int K)
{
	mat centroids(K, X.n_cols);
	vec  v = randi<vec>(X.n_rows, distr_param(0, X.n_rows - 1));
	int i;


	centroids.zeros();

	for (i = 0; i < K; i++)
	{
		centroids.row(i) = X.row(v(i));
	}

	return centroids;
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

void pca(mat X, mat &U, vec &S)
{
	mat sigma = X.t()*X/(int)X.n_rows;
	mat V;

	svd( U, S, V, sigma);
}

mat projectData(mat X, mat U, int K)
{
	int i;
	mat Z(X.n_rows, K);
	Z.zeros();

	for (i = 0; i < (int)X.n_rows; i++)
	{
		Z.row(i) = X.row(i)*U.submat(0, 0, U.n_rows-1, K-1);//U.col(0);
	}

	return Z;
}

mat recoverData(mat Z, mat U, int K)
{
	mat X_rec(Z.n_rows, U.n_rows);
	int i;

	for (i = 0; i < (int)Z.n_rows; i++)
	{
		X_rec.row(i) = Z.row(i)*U.submat(0, 0, U.n_rows-1, K-1).t();//U.col(0).t();
	}

	return X_rec;
}

int main(void)
{
	int i, j, sample_num;
	mat X;
	FILE *plot_sample;
	fstream fin;

	// ================= Find Closest Centroids ====================
	mat idx, initial_centroids(3, 2);
	fin.open("data/X.dat",ios::in);

	sample_num = 300;
	X = mat(sample_num, 2);
	plot_sample 	= popen ("gnuplot -persistent", "w");
//	cout.precision(20);

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < 2; j++)
		{
			fin>>X(i, j);
		}
	}

	fin.close();

	fprintf(plot_sample, "set title \"Raw data\"\n");
	fprintf(plot_sample, "set size square\n");
	fprintf(plot_sample, "set xr [-2.0:10.0] \n");
	fprintf(plot_sample, "set yr [0.0:6.0] \n");
	fprintf(plot_sample, "plot 'data/X.dat' using 1:2 notitle with points pointtype 71\n");
	fflush(plot_sample);

	initial_centroids(0, 0) = 3;
	initial_centroids(0, 1) = 3;
	initial_centroids(1, 0) = 6;
	initial_centroids(1, 1) = 2;
	initial_centroids(2, 0) = 8;
	initial_centroids(2, 1) = 5;

	idx = findClosestCentroids( X, initial_centroids);


	//===================== Compute Means =========================
	int K = 3;
	mat centroids = computeCentroids(X, idx, K);

	centroids.print("centroids: ");

	//=================== K-Means Clustering ======================
	int max_iters = 10;
	runkMeans( X, initial_centroids, max_iters, centroids, idx, true);

	//============= K-Means Clustering on Pixels ===============
	FILE *f_X = fopen("data/X_bird.dat", "r");
	sample_num = 16384;
	X = mat(sample_num, 3);
	K = 16;
	max_iters = 10;

	for (i = 0; i < sample_num; i++)
	{
		fscanf(f_X, "%lf %lf %lf", &X(i, 0), &X(i, 1), &X(i, 2));
	}

	X /= 255;

	initial_centroids = kMeansInitCentroids(X, K);

	runkMeans( X, initial_centroids, max_iters, centroids, idx, false);

	centroids.print("centroids: ");

	//================= Image Compression ======================
	idx = findClosestCentroids(X, centroids);
	mat X_recovered;
	X_recovered.copy_size(X);
	cv::Mat ori = imread("data/bird_small.png");
	cv::Mat image = ori.clone();

	for (i = 0; i < sample_num; i++)
	{
		X_recovered.row(i) = centroids.row(idx.row(i)(0, 0) - 1);

		image.at<cv::Vec3b>(i%128, i/128)[0] = X_recovered.row(i)(0, 2)*255;
		image.at<cv::Vec3b>(i%128, i/128)[1] = X_recovered.row(i)(0, 1)*255;
		image.at<cv::Vec3b>(i%128, i/128)[2] = X_recovered.row(i)(0, 0)*255;
	}

	imshow("ori", ori);
	imshow("Recover image", image);
	waitKey(200000);

	//================== PCA: Load Example Dataset  ===================
	FILE *U_1, *U_2;
	mat X_norm, mu, sigma, U;
	vec S;
	int num;

	fin.open("data/PCA_X.dat",ios::in);

	sample_num = 50;
	X = mat(sample_num, 2);

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < 2; j++)
		{
			fin>>X(i, j);
		}
	}

	U_1 = fopen("data/U_1", "w");
	U_2 = fopen("data/U_2", "w");

	X_norm = featureNormalize(X, mu, sigma);
	pca(X_norm, U, S);
	mu.print("mu:");
	U.print("U:");
	S.print("S:");
	X_norm.print("X norm:");

	fprintf(U_1, "%lf %lf\n", mu(0, 0), mu(0, 1));
	fprintf(U_1, "%lf %lf\n", mu(0, 0)+1.5*S(0)*U(0, 0), mu(0, 1)+1.5*S(0)*U(0, 1));
	fprintf(U_2, "%lf %lf\n", mu(0, 0), mu(0, 1));
	fprintf(U_2, "%lf %lf\n", mu(0, 0)+1.5*S(1)*U(1, 0), mu(0, 1)+1.5*S(1)*U(1, 1));
	fflush(U_1);
	fflush(U_2);
	fclose(U_1);
	fclose(U_2);

	fprintf(plot_sample, "set title \"PCA Raw data\"\n");
	fprintf(plot_sample, "set size square\n");
	fprintf(plot_sample, "set xr [0.5:6.5] \n");
	fprintf(plot_sample, "set yr [2.0:8.0] \n");
	fprintf(plot_sample, "plot 'data/PCA_X.dat' using 1:2 notitle with points pointtype 71,");
	fprintf(plot_sample, "     'data/U_1' using 1:2 notitle lc rgb '#000000' with linespoints,");
	fprintf(plot_sample, "     'data/U_2' using 1:2 notitle lc rgb '#000000' with linespoints\n");
	fflush(plot_sample);

	//=================== Dimension Reduction ===================
	FILE *plot_proj, *f_xrec;

	num = 0;
	plot_proj = popen ("gnuplot -persistent", "w");

	for (i = 0; i < (int)X_norm.n_rows; i++)
		fprintf(plot_proj, "set object %d circle at %lf,%lf radius char 0.6 fillstyle empty border lc rgb '#0000FF' lw 1\n",
									++num, X_norm.row(i)(0, 0), X_norm.row(i)(0, 1));

	K = 1;
	mat Z = projectData(X_norm, U, K);
	Z.print("Z:");
	mat X_rec  = recoverData(Z, U, K);
	X_rec.print("X rec:");
	f_xrec = fopen("data/f_xrec", "w");

	for (i = 0; i < (int)X_rec.n_rows; i++)
	{
		fprintf(f_xrec, "%lf %lf\n", X_rec(i, 0), X_rec(i, 1));
	}

	fflush(f_xrec);
	fclose(f_xrec);

	fprintf(plot_proj, "set title \"Dimension reduction on example dataset\"\n");
	fprintf(plot_proj, "set size square\n");
	fprintf(plot_proj, "set xr [-4.0:3.0] \n");
	fprintf(plot_proj, "set yr [-4.0:3.0] \n");
	fprintf(plot_proj, "plot 'data/f_xrec' using 1:2 notitle with points pointtype 1\n");
	fflush(plot_proj);

	//=============== Loading and Visualizing Face Data =============
	int g, h;
	double max_pix = 0;
	mat X_face(5000, 1024);
	cv::Mat face_ori = cv::Mat( 320, 320, CV_64FC1);
	fstream fin_2;
	fin_2.open("data/X_face.dat",ios::in);


	sample_num = 5000;

	for (i = 0; i < sample_num; i++)
	{
		for (j = 0; j < 1024; j++)
		{
			fin_2>>X_face(i, j);
			if (X_face(i, j) > max_pix)
				max_pix = fabs(X_face(i, j));
		}
	}

	mat face_mat = X_face / max_pix;

	for ( g = 0; g < 10; g++ )
	{
		for (h = 0; h < 10; h++ )
		{
			for ( i = 0; i < 32; i++ )
			{
				for ( j = 0; j < 32; j++ )
					face_ori.at<double>(i+g*32, j+h*32) = (face_mat(h+g*10, i+32*j)+1)/2;
			}
		}
	}

	imshow("face", face_ori);
	waitKey(2000000);

	//=========== PCA on Face Data: Eigenfaces  ===================
	X_norm = featureNormalize(X_face, mu, sigma);
	pca(X_norm, U, S);
	max_pix = _max(fabs(U.min()), U.max());
	cout<<"max: "<<max_pix<<endl;
	face_ori = cv::Mat( 32*6, 32*6, CV_64FC1);

	mat U_shift = U/max_pix;

	for ( g = 0; g < 6; g++ )
	{
		for (h = 0; h < 6; h++ )
		{
			for ( i = 0; i < 32; i++ )
			{
				for ( j = 0; j < 32; j++ )
					face_ori.at<double>(i+g*32, j+h*32) = (U_shift( i+32*j, h+g*10)+1)/2;//(X_face(h+g*10, i+32*j)+1)/2;
			}
		}
	}

	imshow("face eigen", face_ori);
	waitKey(2000000);

	//============= Dimension Reduction for Faces =================
	K = 100;
	Z = projectData(X_norm, U, K);

	cout<<"size: "<<Z.n_rows<<", "<<Z.n_cols<<endl;

	//==== Visualization of Faces after PCA Dimension Reduction ====
	X_rec  = recoverData(Z, U, K);
	max_pix = _max(fabs(X_rec.min()), X_rec.max());

	mat rec_mat;
	rec_mat.zeros();
	face_ori = cv::Mat( 32*10, 32*10, CV_64FC1);
	rec_mat.copy_size(X_rec);
	double a, b;

	for (i = 0; i < 100; i++)
	{
		a = fabs(X_rec.row(i).min());
		b = X_rec.row(i).max();
		max_pix = _max(a, b);
		rec_mat.row(i) = X_rec.row(i)/max_pix;

	}

	for ( g = 0; g < 10; g++ )
	{
		for (h = 0; h < 10; h++ )
		{
			for ( i = 0; i < 32; i++ )
			{
				for ( j = 0; j < 32; j++ )
					face_ori.at<double>(i+g*32, j+h*32) = (rec_mat(h+g*10, i+32*j)+1)/2;
			}
		}
	}

	imshow("face recovery", face_ori);
	waitKey(2000000);

	return 0;
}

