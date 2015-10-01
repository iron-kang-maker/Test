/*
 * svm.h
 *
 *  Created on: Sep 23, 2015
 *      Author: iron
 */

#ifndef SVM_H_
#define SVM_H_

#include "/home/iron/lib/armadillo-5.400.2/include/armadillo"

using namespace arma;

typedef enum {
	linear = 0,
	gaussian
}KERNELFUNCTION;

typedef struct model {
	mat X;
	mat y;
	double (*kernelFunciton)(mat x1, mat x2);
	double b;
	mat alphas;
	mat w;
}MODEL;

MODEL svmTrain(mat X, mat Y, double C, KERNELFUNCTION kernel, double tol, int max_passes, double sigma);
double svmPredict(MODEL model, mat X, KERNELFUNCTION kernel, double sigma);
double gaussianKernel(mat x1, mat x2, double sigma);
double linearKernel(mat x1, mat x2);


#endif /* SVM_H_ */
