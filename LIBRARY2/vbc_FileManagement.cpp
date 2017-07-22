#include"vbc_FileManagement.h"
using namespace vbc_lib;
namespace vbc_lib
{
	_vbc_file _vf;
	_vbc_screen _scr;
//	 retLine vbc_lib::rr;
	test_class::test_class(){}
	test_class::~test_class(){}

	 void f_input(bool *v)
 	{
		string waste;
		int tmpv;
		_vf.Input(1,waste);
		_vf.Input(1,tmpv);
		*v=tmpv;
 	}
 	string get_system_output(string command)
	{
		FILE *fp=popen(command.c_str(),"r");
		char buffer[128];
		fgets(buffer, sizeof(buffer), fp);
		pclose(fp);
		
		return buffer;

	}


};

_vbc_file::_vbc_file():curFile(0)
{
//	endline='\r';
	endline='\n';
};
_vbc_file::~_vbc_file()
{
		for(vector<fstream*>::iterator vi=f.begin();vi!=f.end();vi++)
		{
			if(*vi) delete *vi;
		}
};

bool _vbc_file::eof(int num)
{
	if(!f[num])
		throw _vbc_UDTError();
	
	if(f[num]->eof())
		return true;
	else
		return false;
}

void _vbc_file::tests(){}

bool _vbc_file::Open(const string& s, ios_base::openmode o, int num)
{
	int sz=f.size();
	if(num>sz-1)
	{
		f.resize(num+1);
	}
	else
	{
		if(f[num])
		{
			throw _vbc_UDTError();
		}
	}
	f[num]=new fstream(s.c_str(),o);
	if(!(*f[num]))
	{
		throw _vbc_UDTError();
	}

	return true;  
}

void _vbc_file::Close(int num)
{
	if(f.size()>num)
	{

		if(f[num]) delete f[num];
		f[num]=0;
	}
}

void _vbc_file::Close()
{
	for(int i=0;i<f.size();i++)
	{
		if(f[i])
		{
			delete f[i];
			f[i]=0;
		}
	}
}


_vbc_file& _vbc_file::Write(int num,const string& s)
{
	if(!f[num]) throw _vbc_UDTError();
	try
	{
		curFile=&f[num];
		if(inWrite)
			(**curFile)<<","<<Quote<<s<<Quote;
		else
			(**curFile)<<Quote<<s<<Quote;

		inWrite=true;
	}
	catch(...)
	{

	}
	return *this;
}
_vbc_file& _vbc_file::Write(int num,const char* s)
{
	if(!f[num]) throw _vbc_UDTError();
	try
	{
		curFile=&f[num];
		if(inWrite)
			(**curFile)<<","<<Quote<<s<<Quote;
		else
			(**curFile)<<Quote<<s<<Quote;

		inWrite=true;
	}
	catch(...)
	{

	}
	return *this;
}
_vbc_file& _vbc_file::Write(int num,retLine s)
{
	if(!f[num]) throw _vbc_UDTError();

	try
	{
		curFile=&f[num];
		(**curFile)<<endl;//'\r\n';
//		(**curFile)<<endline;//'\r\n';
		inWrite=false;
	}
	catch(...)
	{

	}
	return *this;
}
_vbc_file& _vbc_file::Print(int num,retLine s)
{
	if(!f[num]) throw _vbc_UDTError();
	try
	{
		curFile=&f[num];
		(**curFile)<<endl;
//		(**curFile)<<endline;
		inWrite=false;
	}
	catch(...)
	{

	}
	return *this;
}


_vbc_file& _vbc_file::operator<<(const string& s)
{
	if(inWrite)
		(**curFile)<<","<<Quote<<s<<Quote;
	else
		(**curFile)<<" "<<s;

	return *this;
}

_vbc_file& _vbc_file::operator<<(const char* s)
{
	if(inWrite)
		(**curFile)<<","<<Quote<<s<<Quote;
	else
		(**curFile)<<" "<<s;

	return *this;
}
_vbc_file& _vbc_file::operator<<(retLine s)
{
	if(s==_Cr)
	{
		(**curFile)<<endl;
//		(**curFile)<<endline;//'\n';
	}
	else
	{
		if(inWrite)
			(**curFile)<<",";
		else
			(**curFile)<<" ";
	}
	inWrite=false;
	return *this;
}

void _vbc_file::getInput(string &s)
{
	s="";
	char* c=&buf[0];
	*c=(*curFile)->get();
	while(*c!=',' && *c!='\n' && !(*curFile)->eof())
	{
		if(*c==34)
		{
			
			c++;
			while(*c!=34 && !(*curFile)->eof())
			{
				*c=(*curFile)->get();
				c++;
			}
				chkBuf(&c,s);
		}

		c++;
		chkBuf(&c,s);
		*c=(*curFile)->get();
	}
	*c=0;
	s+=buf;
}

_vbc_file& _vbc_file::Input(int num, string& s)
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
_vbc_screen::_vbc_screen():suppress(false){}
_vbc_screen::~_vbc_screen(){}

/*int _vbc_file::n_lines(string file_name)
{
	string tmp= "wc -l < " + file_name;
	FILE *fp=popen(tmp.c_str(),"r");
	char buffer[128];
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);
	return atoi(buffer);
}*/
	
