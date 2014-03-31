queue<string> litpool;
class Pass1
{
  /*
    -> Creating intermediate file
    -> Symbol table definition
    -> Assigning LOCCTR
  */
  public:
    static void addLiteral(stringstream& ss)
    {
    /*
      adding literal to literal pool
    */
          string literal;
          stringstream litss(ss.str());
          while(litss>>literal)
          if(literal[0]=='=') litpool.push(literal);
    }
    static void empty_litpool(ofstream& out, int& locctr)
    {
    /*
      emptying literal pool to LITTAB
    */
          while(!litpool.empty())
          {
            string tmp = litpool.front();
            if(!symtab[tmp])
            {
              symtab[tmp] = locctr;
              littab[tmp] = locctr;
            
              out<<"*\t"<<tmp<<"\t!!\t"<<toHex(locctr);
              locctr += byte_length(tmp);
              out<<"\t"<<toHex(locctr,false,4)<<endl;
            }
            litpool.pop();
          }
    }
  static void writeInterFile(ifstream& in, ofstream& out, ofstream& eout)
  {
  /*
    writing into intermediate file
  */
  int start=0;
  int locctr=start;
    string line,sline,prev="";
    while(getline(in,line)){
      stringstream ss(line);
    if(ss>>sline)
    {
      if(sline == ".") continue;
      if(sline != "START")
      {
        prev=sline;
        ss>>sline;
      }
      if(sline == "START"){ ss>>hex>>start;}
      else start=0;
      out<<prev<<"\t"<<"START\t"<<toHex(start)<<"\n";
      break;
    }}
   locctr=start;
    while(getline(in,line))
    {
      bool sym=false;
      stringstream ss(line);
      while(ss>>sline)
      {
        bool plus=false;
        string ssline = sline.substr(1);
        if(sline[0] == '+') plus=true;                     // For format 4 instructions
        if(sline == "."){out<<line.substr(0,line.length()-1)<<endl; break;}
        if(sline == "END")
        {
          if(!sym) out<<"!!\t";
          progLength = locctr-start;
          out<<sline<<"\t";
          ss>>sline;
          if(sline != "") out<<sline<<"\t";
          out<<toHex(locctr)<<"\n";
          empty_litpool(out,locctr);         // Emptying litpool
          progLength = locctr-start;
          break;
        }
        addLiteral(ss);
	if(sline=="LTORG")
        {
          if(!sym) out<<"!!\t";
          out<<"LTORG\t!!\n";
          empty_litpool(out,locctr);
          continue;
        }
        if(((!plus && optab[sline].opcode == "")||(plus && optab[ssline].opcode == "")) && sline != "WORD" )
        {
          if(sline=="BASE")
          {
            ss>>sline;
            if(!sym) out<<"!!\t";
            out<<"BASE\t"<<sline<<"\t\n";
            continue;
          }
          if(sline == "RESW")
          {
            int len;
            ss>>len;
            out<<"RESW\t"<<len<<"\t"<<toHex(locctr);
            locctr += 3*len;
            out<<"\t"<<toHex(locctr)<<endl;
            break;
          }
          if(sline == "RESB")
          {
            int len;
            ss>>len;
            out<<"RESB\t"<<len<<"\t"<<toHex(locctr);
            locctr += len;
            out<<"\t"<<toHex(locctr)<<endl;
            break;
          }
          if(sline == "BYTE")
          {
            out<<"BYTE\t";
            ss>>sline;
            out<<sline<<"\t"<<toHex(locctr);
            if(sline[0] == 'X') locctr += (sline.length() - 3)/2;
            if(sline[0] == 'C') locctr += (sline.length() - 3);
            out<<"\t"<<toHex(locctr)<<endl;
            break;
          }
          if(symtab[sline]) eout<<"Error: Dupliacte Symbol '"<<symtab[sline]<<"' found\n";
          else
          {
            out<<sline<<"\t";
            sym = true;
            string exp;
            stringstream expss(ss.str());
            expss>>exp;
            expss>>exp;
            if(exp=="EQU")
            {
              expss>>exp;
              int val;
              if(exp=="*") val=locctr;
              else val = answer(exp);
              out<<"EQU\t"<<exp<<"\t"<<toHex(val)<<"\t"<<toHex(locctr)<<endl;
              symtab[sline] = val;
              ss.str("");
              continue;
            }
            symtab[sline] = locctr;
          }
        }
        else
        { 
          if(!sym) out<<"!!\t";
          prev=sline;
          out<<sline<<"\t";
          ss>>sline;
          if(sline!=prev) out<<sline<<"\t";
          else out<<"!!\t";
          out<<toHex(locctr);
          if(prev=="WORD") locctr +=3;
          if(plus) locctr += 4;
          else locctr += optab[prev].format;
            out<<"\t"<<toHex(locctr)<<endl;
          break;
        }
        prev = sline;
      }
    }
    out<<".     End Of File"<<endl;
    
  }
};





