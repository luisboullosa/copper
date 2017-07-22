#include<vbc_stringfuncs.h>

namespace vbc_lib
{

	GetStr GS;
	
	string chr(int i)
	{
		return string(i,1);
	}

	int asc(const string& s)
	{
		return int(s[0]);
	}

	int asc(char s)
	{
		return int(s);
	}


	bool isNumeric(const string &v)
	{
	bool allowSign=true;	
	bool allowDec=true;
	bool allowExp=true;
		for(int i=0;i<v.size();i++)
		{

			if(v[i]<'0'||v[i]>'9')
			{
				switch(v[i])
				{
				case '+':
				case '-':
					if(allowSign==false) return false;
				break;
				case 'D':
				case 'd':
				case 'E':
				case 'e':
					if(allowExp==false) return false;
				allowExp=false;
				allowDec=false;
				allowSign=true;
				continue;
				case '.':
					if(allowDec==false) return false;
				allowDec=false;
				break;
				default:
				return false;
				}
		}
			allowSign=false;
		}
		return true;
	}

	strBuf::strBuf()
	{
		setg(c,c,&c[99]);
		setp(c,&c[99]);
	}
	void strBuf::str(string* s)
	{
		s->assign(pbase(),pptr()-pbase());
		setg(c,c,&c[99]);
		setp(c,&c[99]);
	}
	strBuf::~strBuf(){};
	GetStr::GetStr():ostream(&sb){};
	string GetStr::str()
	{
		sb.str(&s);
		return s;
	}
	void GetStr::retStr(string *s1)
	{
		sb.str(s1);
	}

}
