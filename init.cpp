#include"copper.h"
//burn in via mutations

void def_connect_cells() //SEPARATES INTO l_size/st*lsize/st locations
{
	//make l_size a multiple of st
	//this is if burn is true
	//if it is not, then need to redo this
	int reg=0;
	for(int xb=1;xb<l_size;xb+=st)
	{
	for(int yb=1;yb<l_size;yb+=st)
	{
		reg++;
		l(xb,yb).is_port=1;
		for(int x=xb;x<xb+st;x++)
		{
			int x_max=Min(x+1,xb+st-1),x_min=Max(x-1,xb);
			for(int y=yb;y<yb+st;y++)
			{
				int y_max=Min(y+1,yb+st-1),y_min=Max(y-1,yb);
				int k=0;
				//identifies neighbors for each source cell
				
				l(x,y).region=reg;
				l(x,y).num_neigh=(x_max-x_min+1)*(y_max-y_min+1)-1;//neg 1 so that don't migrate to itself
				l(x,y).neighbors.redim(1,2,1,l(x,y).num_neigh);//how many elements to incorporate unequal probabilities, using lowest common denominator. In this 
				for(int i1=x_min;i1<=x_max;i1++)
				{
					for(int j1=y_min;j1<=y_max;j1++)
					{
						if(i1!=x || j1 !=y)
						{
							k++;
							l(x,y).neighbors(1,k)=i1;
							l(x,y).neighbors(2,k)=j1;
						}
					}
				}
			}
		}

		//LONG DISTANCE DISPERSAL - this will connect the meta-populations - model to individaul cells "ports" within each patch.
		//right now this only goes through each first cell (step st) 
		if(burn==false && p(p_ld_disp)>0)
		{
			int nld=l_size/st*l_size/st-1;
			l(xb,yb).num_neigh_ld=nld;
			l(xb,yb).ld_p_neighbors.redim(1,nld);//contains CDF (between 0 - 1)
			l(xb,yb).ld_neighbors.redim(1,2,1,nld);//contains x,y
			int loc=0;
			for(int x=1;x<=l_size;x+=st)
			{
				for(int y=1;y<=l_size;y+=st)
				{
					if(x!=xb || y !=yb)
					{
						loc++;
						l(xb,yb).ld_p_neighbors(loc)=1.0*loc/nld; //for the moment, make probabilities even - at end loc=nld, so p=1, cumulative
						l(xb,yb).ld_neighbors(1,loc)=x;
						l(xb,yb).ld_neighbors(2,loc)=y;
					}
				}
			}
			
		}


	}
	}
	n_reg=reg;
}



void def_fill_landscape()
{
	//will remove bariers after, so simply allow distribution to evolve naturally if burn=false
	//make individuals species identical except for the environmental preferences?
	if(burn==true)
	{
		
		_vf.Open("orig_"+nm+CStr(burn)+CStr(rep)+".csv",ios_base::out,1);
		
		for(int sp=1;sp<=p(num_species);sp++)
		{
			//randomly distribute species
			int x=RND(_r)*l_size+1;
			int y=RND(_r)*l_size+1;
			//have them start off with 10% carrying capacity
			landscape * ll=&l(x,y);				
			(*ll).n_t=p(K)*0.10;
			for(int n=1;n<=(*ll).n_t;n++)
			{
				(*ll).pop(n).ini(&(*ll).env,1,sp);//fill with gender - sexual reproducers,species=2
			}
			_vf.Write(1,sp)<<(*ll).region<<_Cr;
		}
		_vf.Close(1);
	}
}

void fill_landscape_all_patch()
{
	//will remove bariers after, so simply allow distribution to evolve naturally if burn=false
	//make individuals species identical except for the environmental preferences?
	if(burn==true)
	{
		for(int xb=1;xb<l_size;xb+=st)
		{
			for(int yb=1;yb<l_size;yb+=st)
			{
				for(int sp=1;sp<=p(num_species);sp++)
				{
					//randomly distribute species
					int x=RND(_r)*st+xb;
					int y=RND(_r)*st+yb;
					//have them start off with 10% carrying capacity
					landscape * ll=&l(x,y);				
					(*ll).n_t=p(K)*0.10;
					for(int n=1;n<=(*ll).n_t;n++)
					{
						(*ll).pop(n).ini(&(*ll).env,1,sp);//fill with gender - sexual reproducers,species=2
					}
				}
			}
		}
	}
}

void fill_landscape_all_patch2()
{
	//will remove bariers after, so simply allow distribution to evolve naturally if burn=false
	//make individuals species identical except for the environmental preferences?
	if(burn==true)
	{
		//choose one location at random to get the environmental conditions for each species. 
		_vbc_vec<landscape> tmp_l(1,p(num_species));
		for(int sp=1;sp<=p(num_species);sp++)
		{
			int x=RND(_r)*l_size+1;
			int y=RND(_r)*l_size+1;
			landscape * ll=&l(x,y);				
	//	Then population all regions with all species, and let the best species win.
			for(int xb=1;xb<l_size;xb+=st)
			{
				for(int yb=1;yb<l_size;yb+=st)
				{
					//randomly distribute species
					int x1=RND(_r)*st+xb;
					int y1=RND(_r)*st+yb;
					landscape * ll1=&l(x1,y1);				
					(*ll1).n_t=p(K)*0.10;
					for(int n=1;n<=(*ll).n_t;n++)
					{
						(*ll1).pop(n).ini(&(*ll).env,1,sp);//fill with gender - sexual reproducers,species=2, env adapted is a single form
					}
				}
			}	
		}
	}
}



void def_set_env_spc()
{
	//create spatial autocorrelation
		
		int a=int(2*RND(_r));//this makes it equal to 1 or -1
			if(a==0)a=-1;
			min_env+=p(spc_evolve)*a;
	
	for(int i=1;i<=l_size;i++)
	{
		for(int j=1;j<=l_size;j++)
		{
			for(int i1=1;i1<=p(env_dim);i1++)
			{
				l(i,j).env_ac(i1)=p(env_range)*RND(_r)+min_env;
			}
		}		
	}
	for(int i=1;i<=l_size;i++)
	{
		for(int j=1;j<=l_size;j++)
		{
			for(int e=1;e<=p(env_dim);e++)
			{
				double mean=0;
				int cnt=0;
				for(int k=max(1,i-1);k<=min(l_size,i+1);k++)
				{
					for(int k1=max(1,j-1);k1<=min(l_size,j+1);k1++)
					{
						if(k!=0 && k1 !=0)
						{
							mean+=l(k,k1).env_ac(e);
							cnt++;
						}
					}
				}
				l(i,j).env_ac(e)=l(i,j).env_ac(e)*(1-p(spc_aut_cor))+p(spc_aut_cor)*mean/cnt;//keep correlations constant over time
			}
		}
	}
}
void def_after_burn()
{
	//shift specific locations (e.g., due to copper contamination at ports).
	for(int i=1;i<=l_size;i+=st)
	{
		for(int j=1;j<=l_size;j+=st)
		{
			l(i,j).env_ac(1)+=p(burn_diff);
		}
	}
	connect_cells();
}
void f_rand_patch()
{
	//randomize individual patches
	int xb=int((l_size/st)*RND(_r))*st+1;
	int yb=int((l_size/st)*RND(_r))*st+1;
	cout<<"QQ "<<xb<<" "<<yb<<endl;
	int a=int(2*RND(_r));//this makes it equal to 1 or -1
		if(a==0)a=-1;
		min_env+=p(spc_evolve)*a;

	for(int i=xb;i<xb+st;i++)
	{
		for(int j=yb;j<yb+st;j++)
		{
			for(int i1=1;i1<=p(env_dim);i1++)
			{
				l(i,j).env_ac(i1)=p(env_range)*RND(_r)+min_env;
			}
			l(i,j).env_ac(1)+=p(burn_diff);
		}
		
	}
}


void def_do_f_event()
{
	set_env_spc();
	for(int i=1;i<=l_size;i+=st)
	{
		for(int j=1;j<=l_size;j+=st)
		{
			l(i,j).env_ac(1)+=p(burn_diff);
		}
	}
}
bool died_transport(individual i)
{
	//may not be entirely justifiable - takes the expected value of change in env, but arguably much stronger on ships.
	//problem is that there is a disconnect with how tolerance might behave in reality, as it is simply a distance from env optimum here.
	double v=i.env_values(1)-(min_env+p(env_range)/2+p(burn_diff));

	if(RND(_r) > exp(-(v*v)/(2*p(sd_fit_transport)*p(sd_fit_transport))))
	{
		return true;
	}
	 return false;

}



