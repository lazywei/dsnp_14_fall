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

extern CirMgr *cirMgr;

unsigned CirGate::_globalRef = 0;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

void
CirGate::addFanin(CirGate* fanin, bool isInverted)
{
   // We can have only 2 fanins on 1 gate
   assert (_faninList.size() < 2);

   _orderedFaninList.push_back(fanin->getId());
   _faninList.insert(pair<CirGate*, bool>(fanin, isInverted));

   fanin->_fanoutList.insert(pair<CirGate*, bool>(this, isInverted));
}

bool
CirGate::isFloating() const
{
   for (vector<int>::const_iterator i = _orderedFaninList.begin(); i != _orderedFaninList.end(); ++i) {
      if (cirMgr->getGate(*i) == 0) {
         return true;
      }
   }
   return false;
}

void
CirGate::dfsTraversal(int& counter) const
{
   CirGate* targetGate;
   for (vector<int>::const_iterator i = _orderedFaninList.begin(); i != _orderedFaninList.end(); ++i) {
      targetGate = cirMgr->getGate(*i);

      if (targetGate == 0) {
         continue;
      }

      if (!targetGate->isGlobalRef()) {
         targetGate->setToGlobalRef();
         targetGate->dfsTraversal(counter);
      }
   }
   cout << "[" << counter << "] ";
   printGate();
   counter++;
}

// -----------------
//    CirPoGate
// -----------------
void
CirPoGate::printGate() const {
   vector<int>::const_iterator iter = _orderedFaninList.begin();
   cout << "PO " << _id << " ";

   CirGate* fanin = cirMgr->getGateInAll(*iter);

   // Check if undef
   if (isFloating()) {
      cout << "*";
   }
   // Check if inverted
   if (isFaninInverted(fanin)) {
      cout << "!";
   }
   cout << *iter << " ";

   cout << endl;
};

// -----------------
//    CirAndGate
// -----------------
void
CirAndGate::printGate() const {
   vector<int>::const_iterator iter = _orderedFaninList.begin();
   cout << "AIG " << _id << " ";

   if (_faninList.at(cirMgr->getGateInAll(*iter))) {
      cout << "!";
   }
   cout << *iter << " ";

   ++iter;
   if (_faninList.at(cirMgr->getGateInAll(*iter))) {
      cout << "!";
   }
   cout << *iter;

   cout << endl;
};
