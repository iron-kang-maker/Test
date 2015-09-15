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
	mat df1, s, x0, df0, df2, tmp;
	mat* x = cost_data->theta;

	(*costFunc)(cost_data, &f1, &df1);


	s = -df1;

	d1 = 0;

	d1 = -accu(s%s);

	z1 = 1.0f / (1 - d1);

	while (1)
	{

		x0 = *x; df0 = df1;

		f0 = f1;

		*x = *x + z1*s;

		if (costFuncCount >= maxCostCalls)
			return 1;
		else
			costFuncCount++;

		(*costFunc)(cost_data,&f2,&df2);

		d2 = 0;

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
			while((( (f2) > ((f1) + RHO*(z1)*(d1))) || ( (d2) > -SIG*(d1) )) && lineSearchFuncCount < Max)
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

				*x = *x + z2*s;
				//if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
				lineSearchFuncCount++;
				(*costFunc)(cost_data,&f2,&df2);

				d2 = 0;

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
			else if(lineSearchFuncCount >= Max)
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

			*x = *x + z2*s;
			//if(costFuncCount >= maxCostCalls) return 1; else costFuncCount++;
			lineSearchFuncCount++;
			(*costFunc)(cost_data,&f2,&df2);

			d2 = 0;

			d2 = accu(df2%s);
		}
		// line search ended
		if(success)
		{
			f1 = f2;
//			printf("%d: Cost: %g\n", costFuncCount, f1);

			A = 0;
			B = 0;
			C = 0;

			A = accu(df1%df1);
			B = accu(df2%df2);
			C = accu(df1%df2);

			s = ((B-C)/A)*s - df2;

			tmp = df1; df1 = df2; df2 = tmp;

			d2 = 0;

			d2 = accu(df1%s);

			if(d2 > 0)
			{

				s = -df1;
				d2 = 0;

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

			*x = x0; df1 = df0;
			if(ls_failed || costFuncCount > maxCostCalls)
			{
				break;
			}

			tmp = df1; df1 = df2; df2 = tmp;

			s = -df1;

			d1 = 0;

			d1 = -accu(s%s);

			z1 = 1/(1-d1);
			ls_failed = 1;
		}
	}
	return 2;
}

