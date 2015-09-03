/*
 * fmincg.c
 *
 *  Created on: Aug 24, 2015
 *      Author: iron
 */
#include "fmincg.h"


int fmincg(void (*costFunc)(COSTDATA *cost_data, COST_FUNC_DATATYPE *cost, mat *grad), COSTDATA* cost_data, int maxCostCalls)
{
	int success = 0, costFuncCount = 0, lineSearchFuncCount = 0, ls_failed = 0;
	COST_FUNC_DATATYPE f1, d1, z1, f0, f2, d2, f3, d3, z3, limit, z2, A, B, C;
//	COST_FUNC_DATATYPE df1[nDim], s[nDim], x0[nDim], df0[nDim], df2[nDim], tmp[nDim];
	mat df1, s, x0, df0, df2, tmp;
	mat* x = cost_data->theta;

	(*costFunc)(cost_data, &f1, &df1);

//	for (i = 0; i < nDim; i++)
//		s[i] = -df1[i];
	s = -df1;

	d1 = 0;
//	for (i = 0; i < nDim; i++)
//		d1 += -s[i]*s[i];
	d1 = -accu(s%s);

	z1 = 1.0f / (1 - d1);

	while (1)
	{
//		for (i = 0; i < nDim; i++)
//		{
//			x0[i] = x[i];
//			df0[i] = df1[i];
//		}
		x0 = *x; df0 = df1;

		f0 = f1;
//		for (i = 0; i < nDim; i++)
//			x[i] = x[i] + (z1)*s[i];
		*x = *x + z1*s;

		if (costFuncCount >= maxCostCalls)
			return 1;
		else
			costFuncCount++;

		(*costFunc)(cost_data,&f2,&df2);

		d2 = 0;
//		for (i = 0; i < nDim; i++)
//			d2 += df2[i]*s[i];
		d2 = accu(df2%s);

		f3 = f1;
		d3 = d1;
		z3 = -z1;

		success = 0;
		limit = -1;
		lineSearchFuncCount = 0;
		// begin line search
		while(1)
		{
			while((( (f2) > ((f1) + RHO*(z1)*(d1))) || ( (d2) > -SIG*(d1) )) && lineSearchFuncCount < MAX)
			{
				limit = z1;
				if( (f2) > (f1) )
				{
					z2 = z3 - (0.5f*(d3)*(z3)*(z3))/((d3)*(z3)+(f2)-(f3));
				}
				else
				{
					A = 6*((f2)-(f3))/(z3)+3*((d2)+(d3));
					B = 3*((f3)-(f2))-(z3)*((d3)+2*(d2));
					z2 = (sqrt(B*B-A*(d2)*(z3)*(z3))-B)/A;
				}
				if(isnan(z2) || isinf(z2))
				{
					z2 = (z3) * 0.5f;
				}

				A = ((z2 < INT*(z3)) ? z2 : INT*(z3));
				B = (1-INT)*(z3);

				z2 = A > B ? A : B;
				z1 = z1 + z2;

//				for(i=0;i<nDim;i++)
//				{
//					x[i] = x[i] + (z2)*s[i];
//				}
				*x = *x + z2*s;
				//if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
				lineSearchFuncCount++;
				(*costFunc)(cost_data,&f2,&df2);

				d2 = 0;
//				for(i=0;i<nDim;i++)
//				{
//					d2 += df2[i] * s[i];
//				}
				d2 = accu(df2%s);
				z3 = z3 - z2;
			}
			if( (f2 > f1 + (z1)*RHO*(d1)) || ((d2) > -SIG*(d1)) )
			{
				break; //failure
			}
			else if( d2 > SIG*(d1) )
			{
				success = 1; break;
			}
			else if(lineSearchFuncCount >= MAX)
			{
				break;
			}
			A = 6*(f2-f3)/z3+3*(d2+d3);
			B = 3*(f3-f2)-z3*(d3+2*d2);
			z2 = -d2*z3*z3/(B+sqrt(B*B-A*d2*z3*z3));
			if(!(B*B-A*d2*z3*z3 >= 0) || isnan(z2) || isinf(z2) || z2 < 0)
			{
				if(limit < -0.5f)
				{
					z2 = z1 * (EXT-1);
				}
				else
				{
					z2 = (limit-z1)/2;
				}
			}
			else if((limit > -0.5) && (z2+z1 > limit))
			{
				z2 = (limit-z1)/2;
			}
			else if((limit < -0.5) && (z2+z1 > z1*EXT))
			{
				z2 = z1*(EXT-1.0);
			}
			else if(z2 < -z3*INT)
			{
				z2 = -z3*INT;
			}
			else if((limit > -0.5) && (z2 < (limit-z1)*(1.0-INT)))
			{
				z2 = (limit-z1)*(1.0-INT);
			}
			f3 = f2; d3 = d2; z3 = -z2;
			z1 = z1 + z2;
//			for(i=0;i<nDim;i++)
//			{
//				x[i] = x[i] + z2*s[i];
//			}
			*x = *x + z2*s;
			//if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
			lineSearchFuncCount++;
			(*costFunc)(cost_data,&f2,&df2);

			d2 = 0;
//			for(i=0;i<nDim;i++)
//			{
//				d2 += df2[i]*s[i];
//			}
			d2 = accu(df2%s);
		}
		// line search ended
		if(success)
		{
			f1 = f2;
			printf("%d: Cost: %g\n", costFuncCount, f1);

			A = 0;
			B = 0;
			C = 0;
//			for(i=0;i<nDim;i++)
//			{
//				A += df1[i]*df1[i];
//				B += df2[i]*df2[i];
//				C += df1[i]*df2[i];
//			}
			A = accu(df1%df1);
			B = accu(df2%df2);
			C = accu(df1%df2);

//			for(i=0;i<nDim;i++)
//			{
//				s[i] = ((B-C)/A)*s[i] - df2[i];
//			}
			s = ((B-C)/A)*s - df2;
//			for(i=0;i<nDim;i++)
//			{
//				tmp[i] = df1[i]; df1[i] = df2[i]; df2[i] = tmp[i];
//			}
			tmp = df1; df1 = df2; df2 = tmp;

			d2 = 0;
//			for(i=0;i<nDim;i++)
//			{
//				d2 += df1[i] * s[i];
//			}
			d2 = accu(df1%s);

			if(d2 > 0)
			{
//				for(i=0;i<nDim;i++)
//				{
//					s[i] = -df1[i];
//				}
				s = -df1;
				d2 = 0;
//				for(i=0;i<nDim;i++)
//				{
//					d2 += -s[i]*s[i];
//				}
				d2 = -accu(s%s);
			}

			A = d1/(d2-COST_FUNC_DATATYPE_MIN);
			z1 = z1 * ((RATIO < A) ? RATIO : A);
			d1 = d2;
			ls_failed = 0;
		}
		else
		{
			f1 = f0;
//			for(i=0;i<nDim;i++)
//			{
//				x[i] = x0[i];
//				df1[i] = df0[i];
//			}
			*x = x0; df1 = df0;
			if(ls_failed || costFuncCount > maxCostCalls)
			{
				break;
			}
//			for(i=0;i<nDim;i++)
//			{
//				tmp[i] = df1[i]; df1[i] = df2[i]; df2[i] = tmp[i];
//			}
			tmp = df1; df1 = df2; df2 = tmp;
//			for(i=0;i<nDim;i++)
//			{
//				s[i] = -df1[i];
//			}
			s = -df1;

			d1 = 0;
//			for(i=0;i<nDim;i++)
//			{
//				d1 += -s[i]*s[i];
//			}
			d1 = -accu(s%s);

			z1 = 1/(1-d1);
			ls_failed = 1;
		}
	}
	return 2;
}

