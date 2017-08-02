#include "copper.h"
gsl_rng *_r;
using namespace vbc_lib;

//Lot P2, arrival port 3 for shuttle??

//compare evolution - start everywhere - don't allow to mix, and allow to mix. - rate of adaptation to copper.
//start as currently done - don't allow mixing and allow mixing.
//would be good to have a paired design, otherwise conditions for simulation would be random

bool plot_grid=true;
bool print_burn=false;
bool sens_analysis=false;
int thin=100;
int st;
string headerfile="copper.h";

void (*fill_landscape)()=&def_fill_landscape;
void (*connect_cells)()=&def_connect_cells;
void (*output)()=&def_output;
void (*print)()=&def_print;
void (*op_head)()=&def_output_head;
void (*after_burn)()=&def_after_burn;
//void (*do_f_event)()=&def_do_f_event;
void (*do_f_event)()=&f_rand_patch;

bool (*chk_break)()=0;//def_chk_break;
void (*set_env_spc)()=&def_set_env_spc;
int n_max;
_vbc_vec<landscape> l, mem_l;
_vbc_vec<double> p, rng_p;
_vbc_vec<int> sp_exist;
_vbc_vec<int> count_migr;
_vbc_vec<int> c_migr_reg;
int n_reg;

int rng_begin=K;
int min_env=100;//beginning environmental location - when force it to change using spc_evolve, it moves the environment locationally
int l_size;
int thread;
int t;//duration
int rep;//number of reps
bool burn;
string nm;
string control_path;
_vbc_vec<double> ops;	


int main(int argc, char *argv[])
{  	
	cout.precision(3);
	cout<<"HELLO"<<endl;
	_r = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(_r,time(0));
	nm=argv[1];
	if(sens_analysis==false)
	{
		cout << "No sensitivity analysis" << endl;
		string hd="head -10 "+headerfile+" | grep 'enum op_info' | sed 's/enum op_info{/''/g' | sed 's/=1/''/g' |sed 's/,end_op_info};//g' > op_head_"+nm;
		system(hd.c_str());
		input_opts();	
		string tmp_s="cp input_params params_"+nm;
		system(tmp_s.c_str());
		//put the iterations here
		for(rep=1; rep <= p(n_rep); rep++)
		{
			cout<<"Simulation Start"<<endl;
			if(ini()==false)//failed to have population at burn in
				continue;
				
			cout<<"SS"<<endl;
			//record initial state, so that can re-run a paired sample with/without dispersal
			mem_l=l;
			op_head();	
			simulate();
			double mem_ld=p(p_ld_disp);
			//turn off dispersal - i.e., all locations are now not connected
			p(p_ld_disp)=0;
			l=mem_l;
			simulate();
			p(p_ld_disp)=mem_ld;
		}
	}
	else
	{
		thread=atoi(argv[2]);
				
		input_opts_rng();	
		string control_path=get_system_output("cat sc_full_path");

		if(thread==0)//write out latin hypercube and then exit
		{
			string tmp_s="cp input_params_rng params_rng_"+nm;
			system(tmp_s.c_str());
			return 0;
		}
		string sens_f="sens_params_"+nm+CStr(thread)+".csv";
		rep=atoi(get_system_output("ssh rebas '~/get_rep 1 "+nm+" "+CStr(thread)+" "+control_path+"'").c_str());
		if(rep==-99)//failed to increment - exit with failure
			return 1;

		//printing column headings - always start sensitivty analysis at K, and have no other separator - need to be a single character
		op_LHC(0,sens_f);

		//check core
		do
		{
			int j=0;
			//fill in the parameters that undergo LHC
			for(int i=rng_begin;i<params_end;i++)
			{
				j++;
				p(i)=rng_p(rep,j);
			}
			
			//saved a file that contained the parameter values associated with each rep, could do additional analysis after that
			if(ini()==true)//failed to have population at burn in
			{
				mem_l=l;
				op_head();
				simulate();

				double mem_ld=p(p_ld_disp);
				//turn on dispersal - i.e., all locations are now not connected
				p(p_ld_disp)=0;
				l=mem_l;
				simulate();
				p(p_ld_disp)=mem_ld;
				//if complete sim - then print LHC
				op_LHC(1,sens_f);

			}
		rep=atoi(get_system_output("ssh rebas '~/get_rep 0 "+nm+" "+CStr(thread)+" "+control_path+"'").c_str());
		if(rep==-99)//failed to increment - exit with failure
			return 1;
		}while(rep<=p(n_rep));
	
	
	}
	return 0;
}

bool def_chk_break()
{
	for(int i=1;i<=p(num_species);i++)
	{
		if(sp_exist(i)==0)
			return true;
	}
//	if(burn==0 && (tot_n==0 || tot_an==0))//one has outcompeted the other
		return false;		
		

}
 


//*********************************INPUT

void input_opts_rng()
{

	_vf.Open("input_params_rng",ios_base::in,1);	
	p.redim(1,params_end-1);
	//input all the single values first
	for(int i=1;i<rng_begin;i++)
	{
		f_input(&(p(i)));
	}
	l_size=p(l_size1); //because it is used so much
	rng_p.redim(1,p(n_rep),1,params_end-rng_begin);


	//input the ranges for LHC
	if(thread==0)//generate scenarios for the first thread, then read in as appropriate
	{
		for(int i=1;i<=params_end-rng_begin;i++)
		{
			bool tf=false;
			if(i>=fit_d_sd && i<=fit_dist_sd)
				tf=true;
			f_input(&rng_p,i,p(n_rep),tf);
		}
		//save LHC to file
		f_output(&rng_p, "LHC_scenarios_"+nm+".csv", p(n_rep),params_end-rng_begin,2);
	}
	else
	{
		f_input(&rng_p, "LHC_scenarios_"+nm+".csv", p(n_rep),params_end-rng_begin,2);
	}

	_vf.Close(1);
}



void input_opts()
{
	_vf.Open("input_params",ios_base::in, 1);	
	p.redim(1,params_end-1);


	//input all the single values first
	for(int i=1;i<params_end;i++)
	{
		f_input(&(p(i)));
	}

	cout<<"NUM SPP "<<p(num_species)<<endl;;
	l_size=p(l_size1);//because it is used so much
	_vf.Close(1);
}

void simulate()
{
	int fe=p(force_event);
	for(t=1;t<=p(end_t);t++)
	{
		ops.redim(1,p(num_species),1,end_op_info-1);
		sp_exist.redim(1,p(num_species));
		count_migr.redim(1,p(num_species));
		c_migr_reg.redim(1,n_reg,1,n_reg,1,p(num_species));
		if(fe !=0 && t % fe==0 && burn==0)//only do this for non-burn in
		{
			do_f_event();
		}
		
		for(int tp=1;tp<=2;tp++)//2 stages , repr, migration
		{
//			cout<<"TP_IN"<<tp<<endl;


			for(int i=1;i<=l_size;i++)
			{
				for(int j=1;j<=l_size;j++)
				{
					if((l(i,j).n_t)>0)//in repr did not go through - then migr into and must go through. Thus, did not reset mem_n_sex
					{
						if(tp ==1 && RND(_r)<p(disturbance))
						{
							//make sure delete necessary output variables too, since would reflect previous time interval
							l(i,j).killall();
							cout<<"DISTURBED"<<endl;
						}	
						else
						{
							if(tp==1)
							{
								l(i,j).calc_fitness();
								l(i,j).death();
								l(i,j).get_pop_info();//if record output at this point here, will have all n_g info.
								if(t % thin == 1)
									l(i,j).op_fill_info();

								l(i,j).growth();
							}
							else if(tp==2)
							{
								l(i,j).migrate();
							}
						}
					}

				}
			}
//			cout<<"TPOUT"<<endl;
		}
		//only output after growth/mortality - keep signal cleaner
		
		if(t % thin == 1)
		{
			op_fill_info();
			output();
			print();
		}	
		space_flux();//it's really here that fitness is modified - otherwise, don't need to recalculate
	
		//CONDITIONS
		if(chk_break && chk_break()==true)
			break;		
		
	}
}

void reset_spc()
{
	min_env=100;//reset at ini, or would evolve to different levels. 
	
	l.redim(1,l_size,1,l_size);
//	env.redim(1,l_size,1,l_size);
	for(int i=1;i<=l_size;i++)
	{
		for(int j=1;j<=l_size;j++)
		{
			l(i,j).ini(i,j);
		}
	}
}


bool ini()
{
	//easiest way of doing burn in may be to re-use l.
	//may not need to use as large a landscape - decide later
	//memorize this
	st=p(step);
	n_max=p(K)*(p(fecundity)+2);//make it +2 to account for old indiv and allow migrations
	//or migrate specific individuals
	int mem_t=p(end_t);
	p(end_t)=p(n_burn);
	ops.redim(1,p(num_species),1,end_op_info-1);
	//************BURN IN
	burn=true;
	double mem_env_rng=p(env_range);
	int mem_size=l_size;
	reset_spc();
	set_env_spc();
	space_flux();
	connect_cells();
	fill_landscape();//will need to determine species traits here too

	//memorize state here, then run 2 sets of simlations
	
	
	//simple - choose one individuals and change sex - this way all individuals begin as identical, except for gender, then variation arises due to mutations
	op_head();
	simulate();
	//something has survived
/*	cout<<"FF "<<tot_n<<" "<<tot_an<<endl;
	//FIX THIS
	if(cells+cells_a<l.V.size()*0.5 || tot_n+tot_an<p(K)*l.V.size()*0.5)//if less than half cells filled, species not able to thrive even in isolation - interested in competition, when both would do well by themselves
	{
		cout<<"D"<<endl;
		return false;
	}
*/
	burn=false;
	//****************EXIT BURN IN
	//assign info to new environment
	//choose a certain number of individuals
	p(env_range)=mem_env_rng;
	l_size=mem_size;
	p(end_t)=mem_t;
	after_burn();
	cout<<"F"<<endl;
	return true;
}


void space_flux()
{
	
	for(int i=1;i<=l_size;i++)
	{
		for(int j=1;j<=l_size;j++)
		{
			for(int e=1;e<=p(env_dim);e++)
			{
				l(i,j).env(e)=l(i,j).env_ac(e)+gsl_ran_gaussian(_r,p(env_ch_sd));
			}
		}
	}
}
landscape::landscape(){}
landscape::~landscape(){}
void landscape::ini(int i, int j)
{
	x=i;
	y=j;
	l_K=p(K);
	n_t=0;
	mem_n=0;
	num_neigh=0;
	num_neigh_ld=0;
	is_port=false;
	region=0;
	env.redim(1,p(env_dim));
	env_ac.redim(1,p(env_dim));
	pop.redim(1,n_max);
	pop_max=n_max;
	male.redim(1,p(num_species),1,n_max);
	n_male.redim(1,p(num_species));
	n_sp.redim(1,p(num_species));//this is relevant for spatial plotting
	avg_phen.redim(1,p(env_dim),1,p(num_species));
}
void landscape::get_pop_info()
{
	//why not put all g_info here.
	n_male.redim(1,p(num_species));
	n_sp.redim(1,p(num_species));
	for(int i=1;i<=n_t;i++)
	{
		n_sp(pop(i).species)++;
		sp_exist(pop(i).species)=1;

		if(pop(i).gender==0)
		{
			n_male(pop(i).species)++;	
			male(pop(i).species,n_male(pop(i).species))=i;//index which individuals are males
		}
	}
}
void landscape::killall()
{
	n_t=0;
	mem_n=0;
	avg_phen.redim(1,p(env_dim),1,p(num_species));
	n_sp.redim(1,p(num_species));

}
void landscape::growth()
{
	//stochasticity could come in to who reproduces (everyone, random choice, or also a function of fitness)
	int cnt=n_t;
	for(int i=1;i<=n_t;i++)
	{
		if(pop(i).gender!=0 && pop(i).lv_growth>0)//don't need to do for males
		{
			if(pop(i).gender==1 && n_male(pop(i).species)==0)
				continue;

			int new_n=gsl_ran_poisson(_r,pop(i).lv_growth);
//			cout<<"NEW "<<new_n<<" "<<old(i).lv_growth<<endl;
//		cout<<"A "<<i<<" "<<n_t<<" "<<pop_max<<" "<<new_n<<" "<<n_male<<endl;
			if(cnt+new_n>pop_max)//allow population to be dynamic
			{
				pop_max*=2;
				pop.resizeUB(1,pop_max);
				male.resizeUB(1,p(num_species),1,pop_max);
			}
			for(int j=1;j<=new_n;j++)
			{
				cnt++;
				if(pop(i).gender==1)
				{
					int i1=int(n_male(pop(i).species)*RND(_r)+1);
					pop(cnt).assort(&pop(i),&pop(male(pop(i).species,i1)));
				}
				else
				{
					pop(cnt)=pop(i);
				}
				pop(cnt).mutate();
				pop(cnt).calc_env_val();
			}
		}
	}
	n_t=cnt;
	mem_n=n_t;	
}
void landscape::calc_fitness()
{
	for(int i=1;i<=n_t;i++)
	{//recalculate total interactions (including distances) for each individual & sum
		pop(i).fitness_di=pop(i).calc_fitness(&env,p(fit_di_sd));
		pop(i).fitness=pop(i).calc_fitness(&env,p(fit_d_sd));
	}
	for(int i=1;i<=n_t;i++)
	{//recalculate total interactions (including distances) for each 	
		double comp=0;
		for(int j=1;j<=n_t;j++)
		{//calculates the numerator?
			comp+=pop(j).fitness*pop(j).calc_fitness(&pop(i).env_values,p(fit_dist_sd));
		}
		pop(i).fitness_dist=comp;
		comp/=pop(i).fitness*pop(i).fitness_di*l_K;
		pop(i).lv_growth=p(fecundity)*(1-comp);//comp replaces N/k - this is just DN/DT - thus, need to include in mortality, probability that individual dies
	}
}
void landscape::death()
{
	for(int i=1;i<=n_t;i++)
	{
		double mort=0;
		if(pop(i).lv_growth<0)
			mort=-pop(i).lv_growth;
		
		mort=mort+p(mort_bg)-mort*p(mort_bg);//death can occur via background or too great competition.

		if(RND(_r)<mort)//due to random causes
		{
			pop(i)=pop(n_t);
			n_t--;
			i--;
		}
	}
}

void landscape::migrate()
{

	//only migrate to neighbors for now
	for(int i=1;i<=mem_n-2;i++)//this occurs after reproduction, where mem_n is set to nt. Any additional individuals added at that point will not be included in migration in this time interval.
	{

		double p1=(pop(i).c_migr(1)+pop(i).c_migr(2))/2;
		double pm=RND(_r);
		bool is_dead=false;
		if(pm<p1)
		{
			int k,k1;
			if( num_neigh_ld>0 && pm<p(p_ld_disp)*p1)
			{
			//long distance
				int c=find_ld_pos(pm/(p(p_ld_disp)*p1));//so this scales it between zero and one
			//CHANGED
//				k=ld_neighbors(1,c)+int(st*RND(_r));
//				k1=ld_neighbors(2,c)+int(st*RND(_r));
				//Actually, k and k1 are the first position in a patch. We could actually randomize, to change the entry point.
				k=ld_neighbors(1,c);
				k1=ld_neighbors(2,c);
				//src, dst, sp
				c_migr_reg(region,l(k,k1).region,pop(i).species)++;
				is_dead=died_transport(pop(i));
			}
			else
			{
			//short distance dispersal
				int c=int(num_neigh*RND(_r))+1;
				k=neighbors(1,c);
				k1=neighbors(2,c);
			}

			if(is_dead==false)
				l(k,k1).immigrate(pop(i));
			
			count_migr(pop(i).species)++;//change this to species specific
				//need to do 2 separate moves here, to keep track of which ones were recent migrants
			pop(i)=pop(mem_n);
			pop(mem_n)=pop(n_t);//swap last position of possibly new individuals into mem_n position - then adjust all
			n_t--;
			mem_n--;
			i--;
		}
	}
	
}
int landscape::find_ld_pos(double pm)
{
	int ub=num_neigh_ld;
	int lb=0;

	for(;;)
	{
		int mid=(ub-lb)/2+lb;
		if(mid==lb)//rounding will make this lower than ub
			return mid+1;// > (pm<ld_p_neighbors(mid));
		if(pm<=ld_p_neighbors(mid))
			ub=mid;
		else
			lb=mid;	
	};
}
void landscape::immigrate(const individual& i)
{
	if(n_t==pop_max)
	{
		pop_max*=2;
		pop.resizeUB(1,pop_max);
		male.resizeUB(1,p(num_species),1,pop_max);
	}

	n_t++;
	pop(n_t)=i;
}

double individual::calc_fitness(_vbc_vec<double>* env, double sd_fit)
{
	//this is F
	double dev=0;	
	for(int i=1;i<=p(env_dim);i++)
	{
		
		double v=env_values(i)-(*env)(i);
//		cout<<"F "<<i<<" "<<v<<endl;
		dev+=v*v/(2*sd_fit*sd_fit);//this will give use the multivariate normal distribution;
	}
	//note - divided by env_dim to correct for and separate complexity with total distance (i.e., selective pressure).
	return exp(-dev/p(env_dim));//right now, don't calculate it via the exponential, because can sum and save computational time later
//	return exp(-dev);//right now, don't calculate it via the exponential, because can sum and save computational time later
}
individual::~individual(){}

individual::individual()
{
	//initialize
	values.redim(1,2,1,p(env_dim),1,p(tr_per_env));	
	env_values.redim(1,p(env_dim));
	c_migr.redim(1,2);

}
void individual::ini(_vbc_vec<double>* env, int g, int sp)
{
//only done in the beginning of the simulation. Elsewhere, it will be filled in via immigration and reproduction
//may also want to consider starting nearer the true optimum as another scenario
	c_migr(1)=p(migrate_rate);
	c_migr(2)=p(migrate_rate);
	if(g==2)
		gender=2;
	else
		gender=int(2*RND(_r));
	species=sp;
	//this is an open question how to initialize things
	for(int i=1;i<=p(env_dim);i++)
	{
		double tot=0,tot1=0;//now everything will be around the optimal range
		for(int j=1;j<=p(tr_per_env);j++)
		{
			//CHANGED
			values(1,i,j)=(*env)(i);//-10;
			values(2,i,j)=(*env)(i);//-10;
		}
	}
	calc_env_val();
}
void individual::assort(individual* i1, individual* i2)
{
	//ATTENTION, LUIS
	int p1=int(2*RND(_r)+1);
	int p2=int(2*RND(_r)+1);
	c_migr(1)=i1->c_migr(p1);	
	c_migr(2)=i2->c_migr(p2);	
	gender=int(2*RND(_r));
	species=i1->species;
	for(int i=1;i<=p(env_dim);i++)
	{
		for(int j=1;j<=p(tr_per_env);j++)
		{
			int p1=int(2*RND(_r)+1);
			int p2=int(2*RND(_r)+1);
			values(1,i,j)=i1->values(p1,i,j);
			values(2,i,j)=i2->values(p2,i,j);
		}
	}	
}
void individual::mutate()
{
	for(int k=1;k<=2;k++)
	{
		if(p(migr_evolve)>0.5)//avoid issues with integers
		{
			c_migr(k)+=gsl_ran_gaussian(_r,p(m_evol_sd));
			if(c_migr(k)<0)
				c_migr(k)=0;
		}
			
		for(int i=1;i<=p(env_dim);i++)
		{
			for(int j=1;j<=p(tr_per_env);j++)
			{
				if(RND(_r)<p(mut_rate))
				{
					values(k,i,j)+=gsl_ran_gaussian(_r,1);
				}
			}
		}
	}
}

void individual::calc_env_val()
{
	if(p(gene_weights) == 0.0) 			// No epistasis
	{
		for(int i = 1; i <= p(env_dim); i++)
		{
			env_values(i) = 0;

			for(int k = 1; k <= 2; k++)
			{	
				for(int j=1; j <= p(tr_per_env); j++)
				{
					env_values(i) += values(k,i,j);
				}
			}
			env_values(i) /= (2 * p(tr_per_env));
			// Why are two parents always considered?
		}
	}
	else 											// In case of epigenesis
	{
		for(int i = 1; i <= p(env_dim); i++)		// for every trait
		{
			double totalWeight = 0.0;
			env_values(i) = 0;
			for(int l = 1; l <= p(env_dim); l++)	// for every trait-related gene
			{
				for(int k = 1; k <= 2; k++)			// for every parent
				{	
					for(int j=1; j <= p(tr_per_env); j++)
					{
						if(l == i) // Diagonal of the genotype-phenotype matrix
						{
							env_values(i) += values(k,i,j);
							totalWeight += 1.0;
						}
						else		// 
						{
							env_values(i) += values(k,i,j) * p(gene_weights);
							totalWeight += p(gene_weights);
						}
					}
				}
			}
			env_values(i) /= (2 * p(tr_per_env) * totalWeight);
		}
	}
}

