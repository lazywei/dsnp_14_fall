/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;
// TODO: Implement memeber functions for class(es) in cirGate.h

unsigned long long dfsFlag = 0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/

void
CirGate::reportGate() const
{
   size_t numstr = getTypeStr().length();
   size_t numId = 1, temp = getId();
   size_t numSymbol = 0;
   if (getSymbol() != "")
      numSymbol = getSymbol().length() + 2;

   while (temp /= 10) ++numId;
   cout << "==================================================" << endl
        << "= " << getTypeStr() << "(" << getId() << ")";
   if (numSymbol != 0) cout << "\"" << getSymbol() << "\"";
   cout << ", line " << setw(9) << left << _lineNo 
        << setw(30 - numstr - numId - numSymbol) << right << "=" << endl;


   cout << "= FECs:";

   if (_fecGrpIdx >= 0) {
      IdList* grp = cirMgr->getFecGrps().at(_fecGrpIdx);

      for (IdList::const_iterator id = grp->begin(); id != grp->end(); ++id) {
         if ((*id) / 2 != _id) {
            cout << " ";
            if ((*id) % 2) { cout << "!"; }
            cout << (*id) / 2;
         }
      }
   }

   cout << endl;




   cout << "= Value: ";

   for (size_t pos = 0; pos < 32; ++pos) {
      cout << cirMgr->getBit(getSimResult(), pos);
      if (pos % 4 == 3 && pos != 31) {
         cout << "_";
      }
   }

   cout << endl;



   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(const int& level) const
{
   assert (level >= 0);
   ++dfsFlag;
   dfsFan(level, level, "in");
   cout << endl;
}

void
CirGate::reportFanout(const int& level) const
{
   assert (level >= 0);
   ++dfsFlag;
   dfsFan(level, level, "out");
   cout << endl;
}

// called in reportFanin and reportFanout
void
CirGate::dfsFan(const int& level, const int& record, const string& type) const
{
   printGate();
   vector<AigGateV> list;
   if (type == "in") list = _faninList;
   else if (type == "out") list = _fanoutList;

   if (isMark()){
      if (level != 0 && list.size() != 0) cout << " (*)" ;
      return;
   }
   if (level != 0) mark();

   for (size_t i = 0; i < list.size() && level > 0; ++i){
      cout << endl << setw(2*(record - level +1 )) << "";
      if (list[i].isInv()) cout << "!";
      list[i].gate()->dfsFan(level - 1, record, type);
   }

}

void
Aig::pGateNet() const
{
   cout << left << setw(4) << "AIG" << getId() << " ";
   if (_faninList[0].gate()->isUndef()) cout << "*";
   if (_faninList[0].isInv()) cout << "!";
   cout << _faninList[0].gate()->getId() << " ";
   if (_faninList[1].gate()->isUndef()) cout << "*";
   if (_faninList[1].isInv()) cout << "!";
   cout << _faninList[1].gate()->getId();
}

void
PI::pGateNet() const
{
   if (getId() == 0) cout << "CONST0";
   else{
      cout << left << setw(4) << "PI" << getId();
      if (getSymbol() != "") cout << " (" << getSymbol() << ")";
   }
}

void
PO::pGateNet() const
{
   cout << left << setw(4) << "PO" << getId() << " "; 
   if (_faninList[0].gate()->isUndef()) cout << "*";
   if (_faninList[0].isInv()) cout << "!";
   cout << _faninList[0].gate()->getId();
   if (getSymbol() != "") cout << " (" << getSymbol() << ")";
}
