#ifndef VBC_STRINGFUNCS_H
#define VBC_STRINGFUNCS_H
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#pragma warning(disable:4800)

//#include"vbc_genlib.h"
#include<string>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
using namespace std;

	const string nullString="";
namespace vbc_lib
{
	const string Quote(1,34);
	const string _Ret(1,'\n');


	class strBuf:public streambuf
	{
		char c[100];//maximum 100 characters
	public:
		strBuf();
		void str(string* s);
		~strBuf();

	};

	class GetStr:public ostream
	{
		strBuf sb;
		string s;
	public:
		GetStr();
		string str();
		void retStr(string *s1);
	};

	string chr(int i);
	int asc(const string& s);
	int asc(char s);
	extern GetStr GS;




	template <class T>
	inline string Str(T val)
	{
		GS<<val;
		return GS.str();
	}

	template <class T>
	inline string CStr(T val)
	{
		GS<<val;
		return GS.str();
	}


	inline float CFloat(const string& i)
	{
	return atof(i.c_str());
	}

	inline int CLng(const string& i)
	{
	return atoi(i.c_str());
	}

	inline short CShort(const string& i)
	{
	return atoi(i.c_str());
	}

	inline double CDbl(const string& i)
	{
	return atof(i.c_str());
	}
	
	bool isNumeric(const string &v);


};

#endif;
