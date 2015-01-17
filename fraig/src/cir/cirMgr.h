/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
class Aig;
class PO;
class PI;
class AigGateV;

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() { clear(); }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      map<int,CirGate*>::const_iterator li = _all.find(gid);
      if (li == _all.end()) return 0;
      return li->second;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void setAigFan(Aig*);
   void setPoFan(PO*);
   void checkFlFanout();

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();
   bool parseHeader(const string&, int*);
   bool parsePi(const string&, const int&);
   bool parsePo(const string&, const int&);
   bool parseAig(const string&, const int&);
   bool parseSymbol(const string&, bool&);
   bool pSymbol(const string& s, const int&);
   bool isSpWs(const char& c);
   bool storeNum(const string& s, const int&, int*);
   bool checkId(const int& id, const int&) const;
   bool checkSpace(const string& s, const int&);
   bool isNum(const string& s, int&, const int&);
   string getTypeStr(const int&) const;
   void clear();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void topoSort();
   void dfs(CirGate*);
   void printFECPairs() const;

   // Private function for optimize

   // Ref: http://stackoverflow.com/a/3385251/1371471
   void removeFromAigs(CirGate* gate) {
      _aigs.erase(remove(_aigs.begin(), _aigs.end(), gate), _aigs.end());
   }

   void removeFromFlFanin(int id) {
      _flFanin.erase(remove(_flFanin.begin(), _flFanin.end(), id), _flFanin.end());
   }

   void removeFromFlFanout(int id) {
      _flFanout.erase(remove(_flFanout.begin(), _flFanout.end(), id), _flFanout.end());
   }

   void deleteAndCleanUpGate(CirGate*);

private:
   map<int, CirGate*> _all;
   vector<PI*>        _pis;
   vector<PO*>        _pos;
   vector<Aig*>       _aigs;

   ofstream           *_simLog;

   vector <CirGate*> _dfsOrder;

   mutable vector <int> _flFanin; // not defined
   mutable vector <int> _flFanout; // not used
   int _maxVarNum;
};

#endif // CIR_MGR_H
