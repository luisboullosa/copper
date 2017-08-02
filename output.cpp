#include"copper.h"

 
//******************************************************OUTPUT

void op_LHC(int st, string f)
{

	//can put all of the options here
	if(st==0)
	{
		string comm="echo 'rep,K'`cut -d',' -f1 input_params_rng | tr '\\n' ',' | cut -d'K' -f2` | sed 's/,rng_end.*,$/\\n/g' > " + f;
		system(comm.c_str());
	}
	else
	{
		_vf.Open(f,ios_base::out | ios_base::app ,1);
		_vf.Write(1,rep);
		for(int i=1;i<=params_end-rng_begin;i++)
		{
			_vf.Write(1,rng_p(rep,i));
		}
		_vf.Write(1,_Cr);
		_vf.Close(1);
	}
	
}

void def_output_head()
{
	if(burn==0 || print_burn==true)
	{
		_vf.Open("op_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out,1);
		_vf.Write(1,"t")<<"p_ld_disp";
		_vf.Open("op_head_"+nm,ios_base::in,2);
		string s;
		//need to write info for each species
		for(int i=0;i<end_op_info-1;i++)
		{
			_vf.Input(2,s);
			for(int j=1;j<=p(num_species);j++)
			{	
				_vf.Write(1,s+"_sp"+CStr(j));
			}
		}
		_vf.Write(1,_Cr);
		_vf.Close(1);
		_vf.Close(2);

	}

	if(plot_grid==true && (burn==0 || print_burn==true))
	{
		_vf.Open("grid_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out,1);
//		_vf.Open("grid_"+nm+CStr(rep)+".csv",ios_base::out,1);
		_vf.Write(1,"t")<<"ld_disp"<<"x"<<"y"<<"env1"<<"env2"<<"region"<<"is_port";
		for(int j=1;j<=p(num_species);j++)
		{	
			_vf.Write(1,"abund_sp"+CStr(j))<<"avg_pheno_e1_sp"+CStr(j)<<"avg_pheno_e2_sp"+CStr(j);
		}
		_vf.Write(1,_Cr);
		
		_vf.Close(1);

		_vf.Open("ld_disp_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out,1);
//		_vf.Open("grid_"+nm+CStr(rep)+".csv",ios_base::out,1);
		_vf.Write(1,"t")<<"src_reg"<<"dst_reg";
		for(int j=1;j<=p(num_species);j++)
		{	
			_vf.Write(1,"sp"+CStr(j));
		}
		_vf.Write(1,_Cr);
		
		_vf.Close(1);

	}
}

void def_print()
{
	cout<<"t="<<t<<endl;
	for(int j=1;j<=p(num_species);j++)
	{	
		cout<<"Species "<<j;
		for(int i=1;i<end_op_info;i++)
		{
			cout<<"\t "<<ops(j,i);
		}
		cout<<endl;
	}
	cout<<endl;
}
void def_output()
{			
	if(burn==0 || print_burn==true)
	{
		_vf.Open("op_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out | ios_base::app,1);
		_vf.Write(1,t)<<p(p_ld_disp);
		for(int i=1;i<end_op_info;i++)
		{
			for(int j=1;j<=p(num_species);j++)
			{	
				_vf.Write(1,ops(j,i));
			}
		}
		_vf.Write(1,_Cr);
		_vf.Close(1);
	}

	if(plot_grid==true && (burn==0 || print_burn==true))
	{

		_vf.Open("grid_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out | ios_base::app,1);
//		_vf.Open("grid_"+nm+CStr(rep)+".csv",ios_base::out | ios_base::app,1);
		for(int i=1;i<=l_size;i++)
		{
			for(int j=1;j<=l_size;j++)
			{
	//			_vf.Write(2,i)<<j<<l(i,j).env(1)<<l(i,j).env(2)<<l(i,j).env(3)<<_Cr;

				_vf.Write(1,t)<<p(p_ld_disp)<<i<<j<<l(i,j).env(1)<<l(i,j).env(2)<<l(i,j).region<<l(i,j).is_port;
				for(int k=1;k<=p(num_species);k++)
				{
					_vf.Write(1,l(i,j).n_sp(k))<<l(i,j).avg_phen(1,k)<<l(i,j).avg_phen(2,k);
				}
				_vf.Write(1,_Cr);
//			cout<<i<<"\t"<<j<<"\t"<<l(i,j).env(1)<<"\t"<<l(i,j).env(2)<<endl;
			}
		}
		_vf.Close(1);

		_vf.Open("ld_disp_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out | ios_base::app ,1);
		for(int i=1;i<=n_reg;i++)
		{
			for(int j=1;j<=n_reg;j++)
			{
				_vf.Write(1,t)<<i<<j;
				for(int k=1;k<=p(num_species);k++)
				{	
					_vf.Write(1,c_migr_reg(i,j,k));
				}
				_vf.Write(1,_Cr);
			}
		}
		_vf.Close(1);
	}	
}

void op_fill_info()
{
	for(int i=1;i<=p(num_species);i++)
	{
//		ops(i,n_migr)=count_migr(i);//change this to species specific
		if(ops(i,tot_n)>1)
		{	//calculate variances
			ops(i,v_fit_d)=(ops(i,v_fit_d)-ops(i,m_fit_d)*ops(i,m_fit_d)/ops(i,tot_n))/(ops(i,tot_n)-1);
			ops(i,v_fit_di)=(ops(i,v_fit_di)-ops(i,m_fit_di)*ops(i,m_fit_di)/ops(i,tot_n))/(ops(i,tot_n)-1);
//			ops(i,v_fit_dist)=(ops(i,v_fit_dist)-ops(i,m_fit_dist)*ops(i,m_fit_dist)/ops(i,tot_n))/(ops(i,tot_n)-1);
//			ops(i,v_lv_growth)=(ops(i,v_lv_growth)-ops(i,m_lv_growth)*ops(i,m_lv_growth)/ops(i,tot_n))/(ops(i,tot_n)-1);
		}
			//calculate means;
			ops(i,m_fit_d)/=ops(i,tot_n);
			ops(i,m_fit_di)/=ops(i,tot_n);
//			ops(i,m_fit_dist)/=ops(i,tot_n);
//			ops(i,m_lv_growth)/=ops(i,tot_n);
	}
		
}

void landscape::op_fill_info()
{

	avg_phen.redim(1,p(env_dim),1,p(num_species));
	for(int k=1;k<=n_t;k++)//these are all recorded in get_pop_info -does not include mort/repr/disp after
	{
		individual * ind=&pop(k);
		int sp=ind->species;
		ops(sp,m_fit_d)+=ind->fitness;
		ops(sp,m_fit_di)+=ind->fitness_di;
//		ops(sp,m_fit_dist)+=ind->fitness_dist;
//		ops(sp,m_lv_growth)+=ind->lv_growth;

		ops(sp,v_fit_d)+=ind->fitness*ind->fitness;
		ops(sp,v_fit_di)+=ind->fitness_di*ind->fitness_di;
//		ops(sp,v_fit_dist)+=ind->fitness_dist*ind->fitness_dist;
//		ops(sp,v_lv_growth)+=ind->lv_growth*ind->lv_growth;
		for(int j=1;j<=p(env_dim);j++)
		{
			avg_phen(j,sp)+=ind->env_values(j);
		}
	}
	for(int k=1;k<=p(num_species);k++)
	{
		if(n_sp(k)>0)
		{
			ops(k,tot_n)+=n_sp(k);
			ops(k,cells)++;
			
			for(int j=1;j<=p(env_dim);j++)
			{
				avg_phen(j,k)/=n_sp(k);
			}
		}	
	}		

}


