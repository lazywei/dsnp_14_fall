/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <algorithm>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
extern unsigned long long dfsFlag;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printing, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
//static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream fin(fileName.c_str());
   if (fin.is_open()){
      string temp;
      vector <string> aag;
      while (getline(fin,temp))
         aag.push_back(temp);
      fin.close();
      int number[5];//[0]:M, [1]:I, [2]:L, [3]:O, [4]:A
      int& I = number[1]; //L = number[2],
      int& O = number[3];
      int& A = number[4];
      if (aag.size() == 0){
         errMsg = "aag";
         return parseError(MISSING_IDENTIFIER);
      }
      if (!parseHeader(aag[0], number)) return false;
      // pi
      CirGate* CONST0 = new PI(0, 0);
      _all.insert(pair<int, CirGate*>(0, CONST0));
      for (size_t i = 0; i < size_t(I) && (i + 1) < aag.size(); ++i, ++lineNo)
         if (!parsePi(aag[i+1], i)) return false;
      if (lineNo < unsigned(I + 1)){
         errMsg = "PI";
         return parseError(MISSING_DEF);
      }
      // po
      for (size_t i = 0; i < size_t(O) && (i + 1 + I) < aag.size(); ++i, ++lineNo)
         if (!parsePo(aag[i + 1 + I], i)) return false;
      if (lineNo < unsigned(I + O + 1)){
         errMsg = "PO";
         return parseError(MISSING_DEF);
      }
      // aig
      // new all the aig first , store in _all & _aigs
      for (int i = 0; i < A && lineNo < aag.size(); ++i, ++lineNo)
         if (!parseAig(aag[lineNo], i)) return false;
      if (lineNo < unsigned(I + O + A + 1)){
         errMsg = "AIG";
         return parseError(MISSING_DEF);
      }
      // set all the fanin & fanout
      for (size_t i = 0; i < _pos.size(); ++i)
         setPoFan(_pos[i]);
      for (size_t i = 0; i < _aigs.size(); ++i)
         setAigFan(_aigs[i]);
      // symbol
      for (bool comment = false; (lineNo < aag.size()) && !comment; ++ lineNo){
         if (!parseSymbol(aag[lineNo], comment)) return false;
      }
     
      checkFlFanout();
      topoSort();
      return true;
   }
   else cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/

// parse the header (first line)
// store M I L O A in number[0] ~ number[4] respectively
// resize _pis & _pos by I & O
bool
CirMgr::parseHeader(const string& s, int* number)
{
   if (isSpWs(s[0])) return false;
   string temp;
   size_t n =  myStrGetTok(s, temp);
   if (temp != "aag"){
      errMsg = temp;
      return parseError(ILLEGAL_IDENTIFIER);
   }
   colNo = 3;
   if (n == string::npos){
      errMsg = "number of vars";
      return parseError(MISSING_NUM);
   }
   for (int i = 0; i < 5; ++i)
      if (!storeNum(s, i, number)) 
         return false;
   if (s[colNo] != '\0')
      return parseError(MISSING_NEWLINE);
   if (number[0] < number[1] + number[2] + number[4]){
      errMsg = "Num of variables";
      errInt = number[0];
      return parseError(NUM_TOO_SMALL);
   }
   // correct
   _pis.resize(number[1]);
   _pos.resize(number[3]);
   _maxVarNum = number[0];
   ++lineNo;
   colNo = 0;
   return true;
}

bool
CirMgr::parsePi(const string& s, const int& i)
{
   int piId;
   string temp;
   if (!checkSpace(s, PI_GATE)) return false;
   colNo = 0;
   if (!isNum(s, piId, PI_GATE)) return false;
   if (!checkId(piId, PI_GATE)) return false;
   // correct   
   _pis[i] = new PI(piId/2, lineNo+1);  
   _all.insert(pair<int, CirGate*>(piId/2, _pis[i]));
   colNo = 0;
   return true;
}

bool
CirMgr::parsePo(const string& s, const int& i)
{
   int poIn;
   if (!checkSpace(s, PO_GATE)) return false;
   colNo = 0;
   if (!isNum(s,poIn, PO_GATE)) return false;
   // correct
   _pos[i] = new PO(_maxVarNum + 1 + i, lineNo+1, poIn);  
   _all.insert(pair<int, CirGate*>(_maxVarNum + 1 + i, _pos[i]));
   colNo = 0;
   return true;
}

bool
CirMgr::parseAig(const string& s, const int& i)
{
   int n, in1, in2, aigId;
   string temp;
   // check aigId
   if (!checkSpace(s, AIG_GATE)) return false;
   colNo = n = myStrGetTok(s,temp);
   if (!isNum(temp, aigId, AIG_GATE)) return false;
   if (!checkId(aigId, AIG_GATE)) return false;
   // aig first input
   if (!checkSpace(s.substr(++colNo), 4)) return false; 
   colNo = n = myStrGetTok(s, temp, n);
   if (!isNum(temp, in1, 4)) return false;
   if (in1 > (2*_maxVarNum) + 1){
      errInt = in1;
      return parseError(MAX_LIT_ID);
   }
   // aig second input
   if (!checkSpace(s.substr(++colNo), 5)) return false;
   colNo = myStrGetTok(s, temp, n);
   if (!isNum(temp, in2, 4)) return false;
   if (in2 > (2*_maxVarNum) + 1){
      errInt = in2;
      return parseError(MAX_LIT_ID);
   }
   // correct
   CirGate* aig = new Aig(aigId/2, in1, in2, lineNo+1);   
   _all.insert(pair<int, CirGate*>(aigId/2, aig));
   _aigs.push_back((Aig*)aig);
   colNo = 0;
   return true;
}

bool
CirMgr::parseSymbol(const string& s, bool& comment)
{
   if (isSpWs(s[0])) return false;
   if (s[0] == 'i')
      return pSymbol(s.substr(1), PI_GATE);
   else if(s[0] == 'o')
      return pSymbol(s.substr(1), PO_GATE);
   else if (s[0] == 'c'){
      if (s[++colNo] == '\0')
         comment = true;
      else 
         return parseError(MISSING_NEWLINE);
   }
   else{
      errMsg = s[0];
      ++colNo;
      return parseError(ILLEGAL_SYMBOL_TYPE);
   }
   return true;
}

// called in parseSymbol
// set the symbol of PI/PO
bool
CirMgr::pSymbol(const string& s, const int& type)
{
   ++colNo;
   if (isSpWs(s[0])) return false;
   string temp;
   size_t n = myStrGetTok(s,temp);
   int idx;
   if (!myStr2Int(temp, idx) || idx < 0){
      errMsg = "symbol index(" + temp + ")";
      return parseError(ILLEGAL_NUM);
   }
   if ((type == PO_GATE && ( size_t(idx + 1) > _pos.size())) ||
       (type == PI_GATE && ( size_t(idx + 1) > _pis.size()))){
      errMsg = getTypeStr(type) + " index";
      errInt = idx;
      return parseError(NUM_TOO_BIG);
   }
   if (n == string::npos || s[n+1] == '\0'){
      errMsg = "symbolic name";
      return parseError(MISSING_IDENTIFIER);
   }
   if (type == PI_GATE){
      if (_pis[idx]->getSymbol() != ""){
         errMsg = "i" + temp;
         return parseError(REDEF_SYMBOLIC_NAME);
      }
   }
   else if (type == PO_GATE){
      if (_pos[idx]->getSymbol() != ""){
         errMsg = "o" + temp;
         return parseError(REDEF_SYMBOLIC_NAME);
      }
   }
   colNo = n + 1;
   myStrGetTok(s, temp, n);
   while (s[colNo] != '\0'){
      if (!isprint(s[colNo])){
         errInt = s[colNo];
         return parseError(ILLEGAL_SYMBOL_NAME);
      }
      ++colNo;
   }
   size_t m = s.find_first_of(' ');
   if (type == PI_GATE)
      _pis[idx]->setSymbol(s.substr(m + 1));
   else if (type == PO_GATE)
      _pos[idx]->setSymbol(s.substr(m + 1));
   
   colNo = 0;
   return true;
}

/**************************************************************/
/******   Helper functions to find error when parsing     *****/
/**************************************************************/

// aig first input type = 4 second input type = 5
string
CirMgr::getTypeStr(const int& type) const
{
   switch(type){
      case PI_GATE: return "PI";
      case PO_GATE: return "PO";
      case AIG_GATE: return "AIG";
      case 4: return "AIG input";
      default: return "AIG input";
   }
   return "";
}

// return false if 
// check whether char c is space or white space
// return true if yes 
bool 
CirMgr::isSpWs(const char& c)
{
   if (c == ' ')
      return !parseError(EXTRA_SPACE);
   else if (isspace(c)){
      errInt = 9;
      return !parseError(ILLEGAL_WSPACE);
   }
   return false;
}

// aig first input type = 4 second input type = 5
// (1) first char is space or whiteSpace or \0
// (2) no new line in the end (for Pi & Po & aig second input)
// (3) no space after aigId/aigfirst input
bool
CirMgr::checkSpace(const string& s, const int& type)
{
   errMsg = getTypeStr(type) + " literal ID";
   if (isSpWs(s[0])) return false;
   else if (s[0] == '\0')
      return parseError(MISSING_NUM);
   size_t n = s.find_first_of(' ');
   if (type == AIG_GATE || type == 4){
      if (n == string::npos)
         return parseError(MISSING_SPACE);
   }
   else{
      if (n != string::npos){
         colNo = n;
         return parseError(MISSING_NEWLINE);
      }
   }
   return true;
}

// convert string s to int n
// if s is not valid, return false
bool 
CirMgr::isNum(const string& s, int& n, const int& type)
{
   if (!myStr2Int(s, n) || n < 0){
      errMsg = getTypeStr(type) +" literal ID(" + s + ")";
      return parseError(ILLEGAL_NUM);
   }
   return true;
}

// check whether id is valid, return false if
// (1) id is larger than max var num
// (2) id is redefined 
// (3) id is inverted (for Pi& Aig)
bool
CirMgr::checkId(const int& id, const int& type) const
{
   errInt = id;
   errMsg = getTypeStr(type);
   if (id > 2*_maxVarNum)
      return parseError(MAX_LIT_ID);
   if (id/2 == 0)
      return parseError(REDEF_CONST);
   errGate = getGate(id/2);
   if (errGate != 0)
      return parseError(REDEF_GATE);
   if (id%2)
      return parseError(CANNOT_INVERTED);
   return true;
}
 
// called in parseHeader
// parse MILOA  n = 0~4
// store in numer[0~4]
bool
CirMgr::storeNum(const string& s, const int& n, int* number)
{
   string temp, errtemp;
   switch(n){
      case 0: errtemp = "vars";
      break;
      case 1: errtemp = "PIs";
      break;
      case 2: errtemp = "latches";
      break;
      case 3: errtemp = "POs";
      break;
      case 4: errtemp = "AIGs";
      break;
   }
   if (isSpWs(s[++colNo])) return false;
   if (s[colNo] == '\0'){
      errMsg = "number of " + errtemp;
      return parseError(MISSING_NUM);
   }
   for (; s[colNo] != ' '; ++colNo){
      if (s[colNo] == '\0'){ 
         if (n != 4) --colNo; 
         break;
      }
      if (isspace(s[colNo])){
         if (n == 4) break;
         else return parseError(MISSING_SPACE);
      }
      temp += s[colNo];
   }
   if (!myStr2Int(temp, number[n]) || number[n] < 0){
      errMsg = "number of " + errtemp + "(" + temp + ")";
      return parseError(ILLEGAL_NUM);
   }
   if (n == 2 && number[n] != 0){
      errMsg = errtemp;
      return parseError(ILLEGAL_NUM);
   }
   return true;
}

// set all the fanin and fanout
// set fanin of p
// set fanout which is p
// also handle Undefined Gate
void 
CirMgr::setAigFan(Aig* p)
{
   bool inFlFaninList = false;
   for (size_t i = 0; i < 2; ++i){
      size_t temp = p->getIn(i);
      CirGate* g = getGate(temp/2);
      if (g == 0){
          g = new Undefined(temp/2);
         _all.insert(pair<int, CirGate*>(temp/2,g));
         if (!inFlFaninList){
            _flFanin.push_back(p->getId());
            inFlFaninList = true;
         }
      }
      else if (g->isUndef()){
         _flFanin.push_back(p->getId());
        inFlFaninList = true;
      }
      AigGateV in(g, temp % 2), out(p, temp % 2);
      assert(p!=0);
      assert(g!=0);
      p->addFanin(in);
      g->addFanout(out);
   }
}

// set all the fanin and fanout
// set fanin of p
// set fanout which is p
// also handle Undefined Gate
void 
CirMgr::setPoFan(PO* p)
{
   int poIn = p->getIn();
   CirGate* g = getGate(poIn/2);
   if (g == 0) {
      g = new Undefined(poIn/2);
      _all.insert(pair<int, CirGate*>(poIn/2, g));
      _flFanin.push_back(p->getId());
   }
   else if (g->isUndef()) _flFanin.push_back(p->getId());
   AigGateV poFanin(g, poIn % 2), aigFanout(p, poIn % 2);   
   p->addFanin(poFanin); 
   g->addFanout(aigFanout);

}

// called after construct ckt
// record gates with floating fanout in _flFanout
void 
CirMgr::checkFlFanout()
{
   for (size_t i = 0; i < _pis.size(); ++i){
      if (_pis[i]->getOutListSize() == 0)
         _flFanout.push_back(_pis[i]->getId());
   }
   for (size_t i = 0; i < _aigs.size(); ++i){
      if (_aigs[i]->getOutListSize() == 0)
         _flFanout.push_back(_aigs[i]->getId());
   }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl
        << "==================" << endl
        << "  PI" << right << setw(12) << _pis.size() << endl
        << "  PO" << right << setw(12) << _pos.size() << endl
        << "  AIG" << right << setw(11) << _aigs.size() << endl
        << "------------------" << endl
        << "  Total" << right << setw(9) << _pis.size() + _pos.size() + _aigs.size() << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for (size_t i = 0; i < _dfsOrder.size(); ++i){
      cout << "[" << i << "] ";
      _dfsOrder[i]->pGateNet();
      cout << endl;
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (size_t i = 0; i < _pis.size(); ++i)
      cout << " " << _pis[i]->getId();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (size_t i = 0; i < _pos.size(); ++i)
      cout << " " << _pos[i]->getId();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if (_flFanin.size() != 0){
      ::sort(_flFanin.begin(), _flFanin.end());
      cout << "Gates with floating fanin(s):";
      for (size_t i = 0; i < _flFanin.size(); ++i)
         cout << " " << _flFanin[i];
      cout << endl;
   }
   if (_flFanout.size() != 0){
      ::sort(_flFanout.begin(), _flFanout.end());
      cout << "Gates defined but not used  :";
      for (size_t i = 0; i < _flFanout.size(); ++i)
         cout << " " << _flFanout[i];
      cout << endl;
   }
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
   // find the aigGate can be reached from PO
   vector <Aig*> aigInDfs;
   for (size_t i = 0; i < _dfsOrder.size(); ++i)
      if (_dfsOrder[i]->getTypeStr() == "AIG")
         aigInDfs.push_back((Aig*)_dfsOrder[i]);

   outfile << "aag " << _maxVarNum << " "  << _pis.size() << " 0 "
           << _pos.size() << " "<< aigInDfs.size() << endl; 
   for (size_t i = 0; i < _pis.size(); ++i)
      outfile << 2*(_pis[i]->getId()) << endl;
   for (size_t i = 0; i < _pos.size(); ++i)
      outfile << ((PO*)_pos[i])->getIn() << endl;
   for (size_t i = 0; i < aigInDfs.size(); ++i)
      outfile << 2*(aigInDfs[i]->getId()) << " " 
              << aigInDfs[i]->getIn(0) << " " 
              << aigInDfs[i]->getIn(1) << endl;
   // print symbol
   for (size_t i = 0; i < _pis.size(); ++i)
      if (_pis[i]->getSymbol() != "")
         outfile << "i" << i << " " << _pis[i]->getSymbol() << endl;
   for (size_t i = 0; i < _pos.size(); ++i)
      if (_pos[i]->getSymbol() != "")
         outfile << "o" << i << " " << _pos[i]->getSymbol() << endl;
}

// called after construct ckt;
// sort all the gate in topological order
// start from PO
// store in _dfsOrder
void
CirMgr::topoSort()
{
   ++dfsFlag;
   for (size_t i = 0; i < _pos.size(); ++i)
      dfs(_pos[i]);
}

// called in topoSort
// put gate in _dfsOrder by postorder
void
CirMgr::dfs(CirGate* g) 
{
   if (g->isMark()) return;
   g->mark();
   const size_t ListSize = g->getInListSize();
   for (size_t i = 0; i < ListSize; ++i){
      dfs(g->getFanin(i).gate());
   }
   if (g->isUndef()) return;
   _dfsOrder.push_back(g);
}

// called in destructor
// delete all the gate
// remove all the data in map & vectors
void
CirMgr::clear(){
   map<int,CirGate*>::iterator li = _all.begin();
   for (; li != _all.end(); ++li)
      delete (li->second);
   _pis.clear();
   _pos.clear();
   _aigs.clear();
   _dfsOrder.clear();
   _flFanin.clear();
   _flFanout.clear();
   _all.clear();
   dfsFlag = _maxVarNum = lineNo = colNo = 0;
}
