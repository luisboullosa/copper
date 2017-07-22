
#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H
#pragma warning(disable:4786)
#include<iostream>
#include<fstream>
#include<vector>
#include<vbc_genlib.h>
//#include<vbc_stringfuncs.h>
//#include<randomize.h>
#include<sstream>

using namespace std;
//using std::string;
//using std::ofstream;
//using std::ios_base; 
namespace vbc_lib
{
	enum retLine  
	{
		_Cont=0, _Cr='\n'

	};

	const string Quote(1,34);
	const string _Ret(1,'\n');

	class test_class
	{
		public:
		test_class();
		~test_class();
	};
	
	const int BUFSIZE=99;
	string get_system_output(string command);

	class _vbc_file
	{
	private:
//		vector<fstream *>::iterator curFile;
		fstream ** curFile;
		bool inWrite;
		char buf[BUFSIZE+1];
		char endline; 
	public:
		vector<fstream *> f;
		_vbc_file();
		~_vbc_file();

		void tests();
		template<class T>
		_vbc_file& Input(int num, T& s)
		{
			
			if(!f[num]) throw _vbc_UDTError();
			try
			{
				curFile=&f[num];
				getInput(s);
			}
			catch(...)
			{

			}
			return *this;

		}
		
		template<class T>
		_vbc_file& Write(int num,const T& s)
		{
			
			if(!f[num]) throw _vbc_UDTError();
			try
			{
				curFile=&f[num];
				//must already be in the middle of a write - any retline will exit inWrite
				if(inWrite)
					(**curFile)<<","<<s;
				else
					(**curFile)<<s;

				inWrite=true;
			}
			catch(...)
			{

			}
			return *this;
		}

		template<class T>
		_vbc_file& Print(int num, const T &s)
		{
			if(!f[num]) throw _vbc_UDTError();
			try
			{
				curFile=&f[num];
				(**curFile)<<" "<<s;
				inWrite=false;
			}
			catch(...)
			{

			}
			return *this;
		}

		template<class T>
		_vbc_file& operator<<(const T& s)
		{
			if(inWrite)
				(**curFile)<<","<<s;
			else
				(**curFile)<<" "<<s;

			return *this;
		}

		template<class T> 
		_vbc_file& operator>>(T& s)
		{
			getInput(s);
			return *this;
		}

	
	private:
		template<class T>
		void getInput(T& s)
		{
			char* c=&buf[0];
			*c=(*curFile)->get();
			while(*c!=',' && *c!=_Cr && !(*curFile)->eof())
			{
				c++;
				*c=(*curFile)->get();
			}
			*c=0;//make null terminated C style string
			conv(s);
			
		}


	public:
		bool eof(int num);  
		bool Open(const string& s,ios_base::openmode o, int num);
		void Close(int num);
		void Close();

		
		
		_vbc_file& Write(int num,const string& s);
		_vbc_file& Write(int num,const char* s);
		_vbc_file& Write(int num,retLine s);
		_vbc_file& Print(int num,retLine s);
	
		_vbc_file& operator<<(const string& s);
		_vbc_file& operator<<(retLine s);
		_vbc_file& operator<<(const char* s);


		_vbc_file& Input(int num, string& s);
		inline int n_lines(string file_name)
		{
			string tmp= "wc -l < " + file_name;
			return(atoi(get_system_output(tmp).c_str()));
		}
		inline bool file_exists(string file_name)
		{
			string tmp= "if [ -f "+file_name+" ]; then echo 1; else echo 0; fi;";
			return(atoi(get_system_output(tmp).c_str()));
		}

	private:
		void getInput(string &s);
		inline void chkBuf(char** c,string& s)
		{
			if(*c==&buf[BUFSIZE])
			{
				**c=0;
				s+=buf;
				*c=&buf[0];
			}

		}
		inline void conv(float& s)
		{
			s=atof(buf);
		}
		inline void conv(double& s)
		{
			s=atof(buf);
		}
		inline void conv(long& s)
		{
			s=atol(buf);
		}
		inline void conv(int& s)
		{
			s=atol(buf);
		}
		inline void conv(short& s)
		{
			s=atoi(buf);
		}
	};

	extern _vbc_file _vf;

	class _vbc_screen
	{
		public:
		_vbc_screen();
		~_vbc_screen();
		bool suppress;

		template<class T>
		_vbc_screen& operator<<(const T& s)
		{
			if(!suppress)
				cout<<" "<<s;
			return *this;
		}
		
		inline _vbc_screen& operator<<(retLine s)
		{
	
			if(!suppress)
				cout<<endl;
			return *this;
		}
	
	};

	extern _vbc_screen _scr;

	template<class T>
	void f_input(T *v)
	{
		string waste;
		T tmpv;
		_vf.Input(1,waste);
		_vf.Input(1,tmpv);
		*v=tmpv;
//		cout<<waste<<" "<<*v<<endl;
	}
	template<class T>
	void f_input(T *lb, T*ub)
	{
		string waste;
		T tmpv;
		_vf.Input(1,waste);
		_vf.Input(1,tmpv);
		*lb=tmpv;
		_vf.Input(1,tmpv);
		*ub=tmpv;
	}
	 template<class T>
	 void f_input(_vbc_vec<T>* v, int num_sim, bool mult)
	 {
		string waste;
		T v1, v2;
		_vf.Input(1,waste);
		_vf.Input(1,v1);
		_vf.Input(1,v2);
		(*v).redim(1,num_sim);
//		cout<<v1<<" "<<v2<<endl;
		latin_hypercube(&(*v).V,num_sim,v1,v2,mult);
	
	
	 } 
	 template<class T>
	 string f_input(_vbc_vec<T>* v, int col, int num_sim, bool mult)
	 {
		//filling 2 dimensional array - useful for parameter list
		string var_name;
		T v1, v2;
		_vf.Input(1,var_name);
		_vf.Input(1,v1);
		_vf.Input(1,v2);
		_vbc_vec<T> tmp_v;
		tmp_v.redim(1,num_sim);
//		cout<<v1<<" "<<v2<<endl;
		latin_hypercube(&tmp_v.V,num_sim,v1,v2,mult);
		for(int i=1;i<=num_sim;i++)
		{
			(*v)(i,col)=tmp_v(i);
		}
		return var_name;
	 } 



	 void f_input(bool *v);

	 template<class T>
	 void f_output(_vbc_vec<T>* v, int num_sim, int fn)
	 {
		for(int i=1;i<=num_sim;i++)
		{
			_vf.Write(fn,(*v)(i))<<_Cr;
		}
	 } 

	 template<class T>
	 void f_input(_vbc_vec<T>* v, int num_sim, int fn)
	 {
		(*v).redim(1,num_sim);
		for(int i=1;i<=num_sim;i++)
		{
			_vf.Input(fn,(*v)(i));
		}
	 } 

	//need to do these still
	 template<class T>
	 void f_output(_vbc_vec<T>* v, string filename, int nrow, int ncol, int fn)
	 {
		_vf.Open(filename,ios_base::out,2);
		for(int i=1;i<=nrow;i++)
		{
			for(int j=1;j<=ncol;j++)
			{
				_vf.Write(fn,(*v)(i,j));
			}
			_vf.Write(fn,_Cr);
		}
		_vf.Close(2);
	 } 

	 template<class T>
	 void f_input(_vbc_vec<T>* v, string filename, int nrow, int ncol, int fn)
	 {
		cout<<"AA"<<endl;
		_vf.Open(filename,ios_base::in,2);
		for(int i=1;i<=nrow;i++)
		{
			for(int j=1;j<=ncol;j++)
			{
				_vf.Input(fn,(*v)(i,j));
			}
		}
		_vf.Close(2);
	 } 

	template <class T>
	inline string CStr(T val)
	{
		ostringstream GS;
		GS<<val;
		return GS.str();
	}




	 
};
#endif;
