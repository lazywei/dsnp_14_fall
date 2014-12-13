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

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr() : _constGate(0) {}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if (_undefList.count(gid)) {
         return 0;
      } else {
         return getGateById(gid);
      }
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

   void createConstGate() {
      if (_constGate == 0) {
         _constGate = new CirConstGate(0, -1, -1);
      }
   }

   CirConstGate* getConstGate() const { return _constGate; }

private:
   void parseId(const int&, int&, bool&);
   void realizePoFanin(CirPoGate*);
   void realizeAndFanin(CirAndGate*);
   CirGate* getGateById(const int&) const;

   vector<CirGate*>        _gateList;

   map<int, CirPiGate*>    _piList;
   map<int, CirPoGate*>    _poList;
   map<int, CirAndGate*>   _andList;
   map<int, CirUndefGate*> _undefList;

   CirConstGate* _constGate;

   int _nMax, _nInput, _nOutput, _nAnd;
};

#endif // CIR_MGR_H
