#ifndef GEN_EVOL_H
#define GEN_EVOL_H
//don't move enum line: must be in the top 10 lines - also don't have spaces inbetween commas
enum op_info{tot_n=1,cells,m_fit_d,v_fit_d,m_fit_di,v_fit_di,end_op_info};
//n_migr,m_fit_dist,v_fit_dist,m_lv_growth,v_lv_growth,m_migr,end_op_info};

#include<vbc_FileManagement.h>
#include<vbc_vector.h>
//#include<statistics.h>
#include <gsl_rng.h>
#include <gsl_randist.h>
#include<iostream>
#include<gsl_cdf.h>
using namespace std;
using namespace vbc_lib;


//**density dependent (but not kicking in at K) - gaussian and other distributions
//**density independent mortality dependent of how far from env peak.

//local environment is a distribution of environments, such that can reduce amount of resource and select for alternative phenotypes that may be favoured
//different forms of mutation (e.g., discrete).
#define RND gsl_rng_uniform
//OPTIONS - MAY NEED TO EXTERN
extern bool plot_grid;
extern bool print_burn;
extern bool sens_analysis;
extern int min_env;//beginning environmental location - when force it to change using spc_evolve, it moves the environment locationally
extern int l_size;
extern int st;
extern int thread;
extern int t;//duration
extern int rep;//number of reps
extern bool burn;
extern string nm;
extern string control_path;

//useful for latin hypercube, but also as parameters
extern int rng_begin;
//contain all parameter information in this
extern _vbc_vec<double> p,rng_p;
extern _vbc_vec<int> sp_exist;
extern _vbc_vec<int> count_migr;
extern _vbc_vec<int> c_migr_reg; //migration between regions
extern int n_reg;

//**********************************PARAMETERS
enum params
{
n_rep=1,
end_t,			//duration
l_size1,		//size of landscape
step,			//determining the size of each patch
spc_aut_cor,	//the magnitude of spatial autocorrelation
env_range,		//this is relevant relative to the mutation rates. - this
disturbance,
spc_evolve,		//environmental range shifts
num_species,
migr_evolve,	//migration rules: 0 - migrate based on fixed rate, 1 - migrate evolves and prob depends on fitness, 2 - migrate rate evolves but not based on fitness
m_evol_sd,		//rate of migration evolution, if it occurs
force_event,	//number of time intervals before environment reshifts
n_burn,			//burn in to reach equalibrium genotypes, within local conditions. Modelled with "large" population
K,				//carrying capacity
migrate_rate,
p_ld_disp,		//proability long distance dispersal: separate 3 levels - migrate, high prob and low prob: separation also allows meta-pop dynamics etc.
burn_diff,		//allows burn in environment to differ from competition environment (i.e., introduced to novel habitat)
env_dim,		//the number of environmental dimensions to determine fitness
tr_per_env,		//number of traits per environmental condition //GENES??
env_ch_sd,		//normal distribution for environmental change - also relative to mutation rates/magnitude. May also want to have rare massive effects periodically.
mut_rate,
fecundity,
fit_d_sd,		//relative fitness
fit_di_sd,		//realized carrying capacity (doesn't depend on who is there)
fit_dist_sd,	//frequency dependent interactions
sd_fit_transport,
mort_bg, 		//background mortality (stochastic)
gene_weights, 	//the epistatic weight of each gene on the other traits (0.0 - 1.0)
params_end
};				//rng_end allows it to be flexible so that don't have to count the number of parameters used

//********************MAIN STRUCTURES

class individual
{
public:
	//this would allow offspring to have values exceeding both parents
	_vbc_vec<double> values;//first dimension is the parent, second dimension is the number of environmental conditions, third is the number of values contributing to the environment
	_vbc_vec<double> env_values;//combination across alleles for a given env variable, so that don't need to recalculate, and also can easily determine distance
	_vbc_vec<double> c_migr;
	double fitness,fitness_di,fitness_dist;
	double lv_growth;
//	int age;
	int gender;//0=male, 1=female, 2=asex
	int species;
	individual();
	~individual();
	void ini(_vbc_vec<double>* env,int g, int sp);
	void mutate();
	void assort(individual* i1, individual* i2);
	double calc_fitness(_vbc_vec<double>* env, double sd_fit);//metric of maladaptation
	void calc_env_val();
};

class landscape
{
public:
	int x;
	int y;
	int l_K;
	int region;
	bool is_port;
	landscape();
	~landscape();
	_vbc_vec<double> env;
	_vbc_vec<double> env_ac;
	_vbc_vec<double> avg_phen;
	_vbc_vec<int> neighbors;//this can be replaced by an integer method down to 1% - if do not add up to 100%, treat remainder as non-immigration to avoid bias - this vector will contain the locations
	int num_neigh, num_neigh_ld;
	//it is only relative probababilities that matter: the number of elements needed are sum(p)/lowest common denominator
	_vbc_vec<double> ld_p_neighbors;//low probability long distance neighbors actually need to do a search: all non-zero cells
	_vbc_vec<int> ld_neighbors;//location of long distance neighbors
	 
	int n_t;//this will change depending on how many individuals there are - start each at half the carrying capacity
	_vbc_vec<int> n_sp;
	int pop_max;
	int mem_n;//to remember # after repr, so that do not migrate more than one step. NOTE individuals randomized during death step. 
	_vbc_vec<individual> pop;//population which includes males, females and asexuals and all species
	_vbc_vec<int> male;//keep track so that can randomly select during mating - only need to track index - for each species
	_vbc_vec<int> n_male;
	void ini(int i, int j);
//	double ind_var(int n, _vbc_vec<individual> *s);//,_vbc_vec<individual> * s_mem);//returns a metric of individual level variation in each location
//	void op_fitness();
	int find_ld_pos(double pm);
	void calc_fitness();
	void migrate();
	void death();//includes background death too
	void killall();//e.g., with disturbance
	void get_pop_info();
	void op_fill_info();
	void growth();//after death - survivors reproduce - with sex assorting and asex only mutating.
	void immigrate(const individual& i);

};

extern _vbc_vec<landscape> l, mem_l;

//******************OUTPUT
extern _vbc_vec<double> ops;//output for species	

extern int n_max;

void op_LHC(int st, string sens_f);
void op_fill_info();
//void ind_out();
//int get_rep(bool init_core,int thread);
void def_output_head();
void def_output();
void def_print();
extern void (*output)();
extern void (*op_head)();
extern void (*print)();
//***********INPUT
void input_opts();
void input_opts_rng();

//***********INITIALIZE
bool ini();
bool burn_in(int gender);
void def_connect_cells();//have option for different phases
extern void (*connect_cells)();
void def_fill_landscape();
void fill_landscape_all_patch();
void fill_landscape_all_patch2();
extern void (*fill_landscape)();
void def_set_env_spc();
extern void (*set_env_spc)();
void def_after_burn();
extern void (*after_burn)();

//OLD
//_vbc_vec<individual> ret_burn_p(int st, int e);
//void set_cell(landscape *ll, _vbc_vec<individual> p);



//*****************MAIN
void simulate();//burn=0 competition, burn=1 - sexuals, burn=2 asexuals
void space_flux();
void reset_spc();
bool died_transport(individual i);//use the mean or the theoretical mean of ports - i.e., min_env+env_range/2+burn_diff.
bool def_chk_break();
extern bool (*chk_break)();
void def_do_f_event();
void f_rand_patch();
extern void (*do_f_event)();

#endif;
