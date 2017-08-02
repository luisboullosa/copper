#ifndef _RANDOMIZE.H
#define _RANDOMIZE.H
#include<vbc_vector.h>
#include<iostream>
#include <gsl_rng.h>
#include <gsl_randist.h>
#define RND gsl_rng_uniform

extern gsl_rng *_r;

using namespace std;
using namespace vbc_lib;

template<class T>
void randomize_vector(vector<T>* V)
{
		int sz=(*V).size();
		int sz1=sz;
		vector<T> V1=(*V);
		for(int i=0; i<sz;i++)
		{
			int j= int(sz1*RND(_r));
			(*V)[i]=V1[j];
			V1[j]=V1[sz1-1];	
			sz1--;
		}
}
template<class T>
void fill(vector<T>* V, int N, T lb, T ub, bool mult)
{
		double st;
		if(mult==true)
		{
			st=pow(1.0*ub/lb,1.0/(N-1));
		}
		else
		{
			st=1.0*(ub-lb)/(N-1);
		}
	(*V)[0]=lb;
	double s_st=st;//need to do this to avoid rounding errors
	for(int i=1;i<N;i++)
	{
		
		if(mult==true)
		{
			(*V)[i]=(*V)[0]*s_st;
			s_st*=st;
		}
		else
		{
			(*V)[i]=(*V)[0]+s_st;
			s_st+=st;
		}
	}
}
template<class T>
void latin_hypercube(vector<T>* V, int N, T lb, T ub, bool mult)
{
	fill(V,N,lb,ub,mult);
	randomize_vector(V);

}

#endif;
