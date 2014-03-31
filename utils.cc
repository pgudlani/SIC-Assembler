#include<iostream>
#include<fstream>
#include<map>
#include<queue>
#include<string.h>
#include<sstream>
#include<iomanip>

using namespace std;

int progLength=0;

struct opobj   // struct for optab
{
  string opcode;
  int format;
};

map<string, opobj> optab;                  // To create an optab key->value pairs 
map<string, int> symtab;                 
map<string, int> littab;                 

void createOptab()
{
  /*
    function to create optab
  */
     ifstream in;
     in.open("newoptab.txt");
     while(!in.eof())
     {
         string key, value;
         int format;
         in >> key >> value >> format;
         if(key=="") continue;
         opobj op={value,format};
         optab[key] = op;
     }
}


string toHex(int dec, bool indexed=false, int width=4)
{
/*
  function to convert into hex from dec , indexed for ',X' and width for giving width of string
*/
  stringstream ss;
  ss<<setfill('0')<<setw(width)<<hex<<dec;
  string out;
  ss>>out;
  for(int i=0;i<out.length();i++) if (out[i]>=97 && out[i]<=122) out[i] -= 32;
  if(indexed) out[0]+=8;
  return out;
}

int hexToInt(string s)
{
  stringstream ss;
  ss<<hex<<s;
  int k;
  ss>>k;
  return k;
}

int toInt(string s)
{
  stringstream ss;
  int k;
  ss<<s;
  ss>>k;
  return k;
}
string setWidth(string s)
{
  string sss=s;
  for(int i=s.length();i<6;i++) sss += " ";
  return sss;
}
string set0(string k,int width=6)
{
  stringstream ss;
  ss<<setfill('0')<<setw(width)<<k;
  string out;
  ss>>out;
  return out;
}

string intToString(int s)
{
  stringstream ss;
  string k;
  ss<<s;
  ss>>k;
  return k;
}

bool isNumeric(string s)
{
  for(int i=0; i<s.length(); i++) if(!isdigit(s[i])) return false;
  return true;
}

template<class T>
int indexOf(T data, T k[],int len)
{
  for(int i=0; i<len; i++) if(k[i]==data) return i;
  return -1;
}

int byte_length(string s)
{
  int k=s.length()-4;
  if(s[1]=='X') return k/2;
  if(s[1]=='C') return k;
  return 0;
}

string byte_code(string s)
{
         if(s[1] == 'X') return s.substr(3,s.length()-4);
         if(s[1] == 'C')
         {
           string test = "";
           for(int i=3;i<s.length()-1;i++) test += toHex((int)s[i],false,2);
           return test;
         }
         return "";
}

int answer(string exp)
{
  int value=0,prev=0,temp=0;
  char prevchar=0;
  for(int i=0;i<=exp.length();i++)
  {
    if(exp[i]=='+'||exp[i]=='-'||exp[i]=='/'||exp[i]=='*'||exp[i]=='\0')
    {
      string symbol = exp.substr(prev,i-prev);
      if(symtab[symbol]) temp = symtab[symbol];
      else if(isNumeric(symbol)) temp = toInt(symbol);
      else cout<< "Error: Symbol Inappropiate"<<endl;
      if(prevchar==0) value=temp;
      else if(prevchar=='+') value+=temp;
      else if(prevchar=='-') value-=temp;
      else if(prevchar=='*') value*=temp;
      else if(prevchar=='/') value/=temp;
      else cout<<"wrong operator"<<endl;
      prev=i+1;
      prevchar=exp[i];
    }
  }
  return value;
}
