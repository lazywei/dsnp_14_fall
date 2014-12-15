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
   stringstream output;
   output << "= " << _typeStr << "(" << _id << "), line " << _lineNo;
   cout << "==================================================" << endl;
   cout.width(49);
   cout << left << output.str() << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   CirGate::setGlobalRef();
   reportFaninWithSpace(level, 0, false, false);
}

void
CirGate::reportFaninWithSpace(int level, int numSpace, bool prtInv, bool prtStar) const
{
   assert (level >= 0);

   CirGate* fanin;
   bool isInverted;

   for (int n = 0; n < numSpace; ++n) {
      cout << "  ";
   }

   if (prtInv) {
      cout << "!";
   }

   // Print self.
   cout << _typeStr << " " << _id;

   if (prtStar) {
      cout << " (*)";
   }

   cout << endl;

   if (numSpace < level) {

      for (vector<GateIdInv>::const_iterator i = _faninList.begin(); i != _faninList.end(); ++i) {

         fanin = cirMgr->getGateInAll(i->first);
         isInverted = i->second;

         if (fanin->isGlobalRef()) {
            fanin->reportFaninWithSpace(numSpace+1, numSpace+1, isInverted, true);
         } else {

            if (fanin->getFaninList().size() > 0 && numSpace+1 < level) {
               fanin->setToGlobalRef();
            }

            fanin->reportFaninWithSpace(level, numSpace+1, isInverted, false);
         }

      }
   }
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   CirGate::setGlobalRef();
   reportFanoutWithSpace(level, 0, false, false);
}

void
CirGate::reportFanoutWithSpace(int level, int numSpace, bool prtInv, bool prtStar) const
{
   assert (level >= 0);

   CirGate* fanout;
   bool isInverted;

   for (int n = 0; n < numSpace; ++n) {
      cout << "  ";
   }

   if (prtInv) {
      cout << "!";
   }

   // Print self.
   cout << _typeStr << " " << _id;

   if (prtStar) {
      cout << " (*)";
   }

   cout << endl;

   if (numSpace < level) {

      for (vector<GateIdInv>::const_iterator i = _fanoutList.begin(); i != _fanoutList.end(); ++i) {
         fanout = cirMgr->getGateInAll(i->first);
         isInverted = i->second;

         if (fanout->isGlobalRef()) {
            fanout->reportFanoutWithSpace(numSpace+1, numSpace+1, isInverted, true);
         } else {

            if (fanout->getFanoutList().size() > 0 && numSpace+1 < level) {
               fanout->setToGlobalRef();
            }

            fanout->reportFanoutWithSpace(level, numSpace+1, isInverted, false);
         }
      }
   }
}

void
CirGate::addFanin(CirGate* fanin, bool isInverted)
{
   // We can have only 2 fanins on 1 gate
   assert (_faninList.size() < 2);

   _faninList.push_back(make_pair(fanin->getId(), isInverted));
   fanin->_fanoutList.push_back(make_pair(_id, isInverted));
}

bool
CirGate::isFloating() const
{
   for (vector<GateIdInv>::const_iterator i = _faninList.begin(); i != _faninList.end(); ++i) {
      if (cirMgr->getGate(i->first) == 0) {
         return true;
      }
   }
   return false;
}

void
CirGate::dfsTraversal(vector<int>& dfsIdList) const
{
   CirGate* nxtGate;
   for (vector<GateIdInv>::const_iterator i = _faninList.begin(); i != _faninList.end(); ++i) {
      nxtGate = cirMgr->getGate(i->first);

      if (nxtGate == 0) {
         continue;
      }

      if (!nxtGate->isGlobalRef()) {
         nxtGate->setToGlobalRef();
         nxtGate->dfsTraversal(dfsIdList);
      }
   }
   dfsIdList.push_back(_id);
}

// -----------------
//    CirPoGate
// -----------------
void
CirPoGate::printGate() const {
   vector<GateIdInv>::const_iterator iter = _faninList.begin();
   cout << _typeStr << "  " << _id << " ";

   // Check if undef
   /* if (isFloating()) { */
   if (cirMgr->getGate(iter->first) == 0) {
      cout << "*";
   }
   // Check if inverted
   if (iter->second) {
      cout << "!";
   }
   cout << iter->first << endl;
};

// -----------------
//    CirAndGate
// -----------------
void
CirAndGate::printGate() const {
   vector<GateIdInv>::const_iterator iter = _faninList.begin();
   cout << _typeStr << " " << _id << " ";

   if (cirMgr->getGate(iter->first) == 0) {
      cout << "*";
   }
   if (iter->second) {
      cout << "!";
   }
   cout << iter->first << " ";

   ++iter;
   if (cirMgr->getGate(iter->first) == 0) {
      cout << "*";
   }
   if (iter->second) {
      cout << "!";
   }

   cout << iter->first << endl;
};
