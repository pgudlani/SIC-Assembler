char registers[7] = {'A','X','L','B','S','T','F'};  // List of all Registers
int base=0;          // used for storing BASE
string end_symbol="$$";  // dummy string
class Pass2
{
  /*
    includes all features of pass2
  */
  public:
    static int print_test_record(ofstream& obj,string test[30], string& start, int& dis, int& index)
    {
    /*
      function for printing text record
    */
        if(index == 0 ) return 0;
        obj<<"T"<<set0(start)<<toHex(dis/2,false,2);
        for(int i=0;i<index;i++) obj<<test[i];
        dis=0;
        index=0;
        obj<<endl;
        return 0;
    }

    static int print_mod_record(ofstream& obj,string mod[20], int& index)
    {
    /*
      function for printing Modification record
    */
        if(index == 0 ) return 0;
        for(int i=0;i<index;i++) obj<<"M"<<toHex(hexToInt(mod[i])+1,false,6)<<"05\n";
        index=0;
        return 0;
    }

    static string get_opcode(string opcode,int n=1,int i=1)
    {
    /*
      function for getting the opcode
    */
      int k = hexToInt(opcode);
      k += n*2+i;
      return toHex(k,false,2);
    }

    static string get_dis(string initial,string final,int x=0, int e=0, int format=3)
    {
    /*
      function to get displacement
    */
      if (format==1) return "";
      int b=0,p=0;
      int i = hexToInt(initial);
      int f = hexToInt(final);
      int dis=f-i;
      if(e==0)
      {
        if(dis>2048||dis<=-2048){
           b=1;
           dis = f-base;
           if(dis>4096||dis<0) cout<<"Error: Targeting Address was out of Bounds"<<endl;
        }
        else
        {
          p=1;
          if(dis<0) dis+=4096;
        }
      }
      else dis=f;
      int xbpe = x*8 + b*4 + p*2 + e;
      string s = toHex(xbpe,false,1) + toHex(dis,false,3+2*e);
      return s; 
    }

    static void writeListingFile(ifstream& in, ofstream& out, ofstream& obj, ofstream& eout)
    {
      /*
        function for writing listing file(out) and object program file(obj)
      */
      string start,locctr;
      int dis=0,index=0,mindex=0;
      string line,symbol,label,loc,nextloc,op,test[30],mod[20];
      while(getline(in,line))
      {
        char point='$'; // dummy char
        if(dis>54) print_test_record(obj,test, locctr,dis,index);
        int n=1,i=1,x=0,e=0,format=3;
        string opcode,disp;
        stringstream ss(line);
        ss>>label>>op>>symbol>>loc>>nextloc;
        bool plus=false;
        string sop = op.substr(1);
        if(op[0] == '+') plus=true;                     // For format 4 instructions
        if(label[0] == '.'){
          out<<line<<endl;
          continue;
        }
        int k = symbol.length();
        if(symbol[k-2] == ',' && ((indexOf(symbol[0],registers,7)==-1 && k==3) || k!=3)){ x =1; symbol = symbol.substr(0,k-2); }
        if((!plus && optab[op].opcode != "")||(plus && optab[sop].opcode != "")) 
        {
          format=optab[op].format;
          if(plus) e=1;
          if(index==0) locctr = loc;
          if(symbol[0] == '#'){ n=0;i=1; point=symbol[0]; symbol = symbol.substr(1);}
          if(symbol[0] == '@'){ n=1;i=0; point=symbol[0]; symbol = symbol.substr(1);}
          if(op == "RSUB") 
          {
            out<<loc<<"\t";
            if(label!="!!") out<<label;
            out<<"\tRSUB\t"<<get_opcode(optab["RSUB"].opcode)<<"0000\n";
               if(index==0) locctr = op;
            test[index++]=get_opcode(optab["RSUB"].opcode)+"0000";
               dis += (test[index-1].length());
              continue;
          }
          if(format==1)
          {
            out<<loc<<"\t";
            if(label != "!!") out<<label;
            out<<"\t"<<op<<"\t";
            opcode=get_opcode(optab[op].opcode,0,0);
            out<<"\t"<<opcode<<endl;
            test[index++]=opcode;
            dis += (test[index-1].length());
            continue;
          }
          if(format==2)
          {
            out<<loc<<"\t";
            if(label != "!!") out<<label;
            out<<"\t"<<op<<"\t"<<symbol;
            int mno1 = indexOf(symbol[0],registers,7);
            int mno2 = indexOf(symbol[2],registers,7);
            opcode=get_opcode(optab[op].opcode,0,0);
            if(k==1 && mno1>=0) disp=set0(intToString(mno1*10),2);
            if(k==3 && mno1>=0 && mno2>=0) disp=set0(intToString(mno1*10+mno2),2);
            out<<"\t"<<opcode<<disp<<endl;
            test[index++]=opcode+disp;
            dis += (test[index-1].length());
            continue;
          }
          if(symtab[symbol] || isNumeric(symbol))
          {
            out<<loc<<"\t";
            if(label != "!!") out<<label;
            out<<"\t"<<op<<"\t";
            if(point != '$') out<<point;
            out<<symbol;
            if(x) out<<",X";
            if(plus)
            {
              mod[mindex++] = loc;
              opcode=get_opcode(optab[sop].opcode,n,i);
              disp=get_dis(nextloc,toHex(symtab[symbol]),x,e,format);
            }
            else
            {
              opcode=get_opcode(optab[op].opcode,n,i);
              disp=get_dis(nextloc,toHex(symtab[symbol]),x,e,format);
            }
            if(isNumeric(symbol))
            {
              disp = set0(toHex(toInt(symbol)),3+2*e); 
            }
            out<<"\t"<<opcode<<disp<<endl;
            test[index++]=opcode+disp;
            dis += (test[index-1].length());
            if(op=="LDB" && symbol[0]=='#') base=symtab[symbol];  // assign base a value
          }
          else
          {
            out<<". . .  Error: '"<<symbol<<"' is not defined in the program\n";
          }
        }
       if(op=="BASE")
       {
         base=symtab[symbol];  //assigning base a value
         out<<"\t";
         if(label!="!!") out<<label;
         out<<"\t";
         out<<op<<"\t"<<symbol<<endl;
       }
       if(op=="LTORG")
       {
         out<<"\t";
         if(label!="!!") out<<label;
         out<<"\t";
         out<<"LTORG"<<endl;
       }
       if(label=="*")
       {
         out<<loc<<"\t*\t"<<op<<"\t\t"<<byte_code(op)<<endl;  //byte_code(op) returns its byte value eg: X'05'-> 05 and C'EO'->454F
         test[index++]=byte_code(op);
         dis += (test[index-1].length());
       }
       if(op=="EQU")
       {
         out<<loc<<"\t"<<label<<"\tEQU\t"<<symbol<<endl;
       }
       if(op=="RESW" || op=="RESB") { print_test_record(obj,test, locctr,dis,index); out<<loc<<"\t"<<label<<"\t"<<op<<"\t"<<symbol<<endl;}
       if(op=="BYTE")
       {
         if(symbol[0] == 'X')
         {
           out<<loc<<"\t"<<label<<"\t"<<op<<"\t"<<symbol<<"\t"<<symbol.substr(2,symbol.length()-3)<<endl;
            if(index==0) locctr = loc;
           test[index++]=symbol.substr(2,symbol.length()-3);
            dis += (test[index-1].length());
         }
         if(symbol[0] == 'C')
         {
           out<<loc<<"\t"<<label<<"\t"<<op<<"\t"<<symbol<<"\t";
           test[index] = "";
           for(int i=2;i<symbol.length()-1;i++)
           { out<<toHex((int)symbol[i],false,2);
             test[index] += toHex((int)symbol[i],false,2);
           }
            if(index==0) locctr = loc;
           index++;
            dis += (test[index-1].length());
           out<<endl;
         }
       }
       if(op=="WORD")
       {
         out<<loc<<"\t"<<label<<"\t"<<op<<"\t"<<symbol<<"\t"<<toHex(toInt(symbol),false,6)<<endl;
            if(index==0) locctr = loc;
         test[index++]=toHex(toInt(symbol),false,6);
            dis += (test[index-1].length());
       }
       if(op=="START")
       {
         locctr = set0(symbol);
         base = hexToInt(symbol);
         obj<<"H"<<setWidth(label)<<set0(symbol)<<toHex(progLength,false,6)<<endl;
         out<<symbol<<"\t"<<label<<"\t"<<op<<"\t"<<symbol<<endl;
       }
       if(op == "END") 
       {
            out<<loc<<"\t";
            if(label != "!!") out<<label;
            out<<"\t"<<op<<"\t"<<symbol<<endl;
            end_symbol = symbol;
       }
      }
      if(test[0].length() != 0) print_test_record(obj,test, locctr,dis,index);        // printing text record
      if(mod[0] != "") print_mod_record(obj,mod,mindex);                              // printing modification record
      if(end_symbol != "$$") obj<<"E"<<set0(toHex(symtab[end_symbol],false,6))<<endl;  // using dummy string
    }
   
  


};
