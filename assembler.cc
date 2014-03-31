#include "utils.cc"
#include "pass1.cc"
#include "pass2.cc"


int main()
{
  createOptab();
    ifstream in;
    in.open("newinput.txt");    //name of the input file other input file could be used is 'fibo.txt'
    ofstream inter;
    inter.open("newinter.txt");  // name of the intermediate file
    ofstream eout;
    eout.open("newerror.txt");   // name of the error file
    ofstream list;
    list.open("newlist.txt");    // name of the listing file
  Pass1::writeInterFile(in,inter,eout);
    ifstream iinter;
    iinter.open("newinter.txt");
    ofstream objprog;
    objprog.open("newobj.txt");    // name of the object program file
  Pass2::writeListingFile(iinter,list,objprog,eout);
}


