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

      for (vector<int>::const_iterator i = _orderedFaninList.begin(); i != _orderedFaninList.end(); ++i) {

         fanin = cirMgr->getGateInAll(*i);
         isInverted = isFaninInverted(fanin);

         if (fanin->isGlobalRef()) {
            fanin->reportFaninWithSpace(numSpace+1, numSpace+1, isInverted, true);
         } else {

            if (fanin->getFanin().size() > 0 && numSpace+1 < level) {
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

      for (map<CirGate*, bool>::const_iterator i = _fanoutList.begin(); i != _fanoutList.end(); ++i) {
         fanout = i->first;
         isInverted = i->second;

         if (fanout->isGlobalRef()) {
            fanout->reportFanoutWithSpace(numSpace+1, numSpace+1, isInverted, true);
         } else {

            if (fanout->getFanout().size() > 0 && numSpace+1 < level) {
               fanout->setToGlobalRef();
            }

            fanout->reportFanoutWithSpace(level, numSpace+1, isInverted, false);
         }


         /* if (!fanout->isGlobalRef()) { */
         /*    /1* fanout->setToGlobalRef(); *1/ */

         /*    if (fanout->getFanout().size() > 0 && numSpace+1 < level) { */
         /*       fanout->setToGlobalRef(); */
         /*    } */

         /*    fanout->reportFanoutWithSpace(level, numSpace+1, isInverted, false); */
         /* } */
      }
   }
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
   cout << _typeStr << "  " << _id << " ";

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
   cout << _typeStr << " " << _id << " ";

   if (cirMgr->getGate(*iter) == 0) {
      cout << "*";
   }
   if (_faninList.at(cirMgr->getGateInAll(*iter))) {
      cout << "!";
   }
   cout << *iter << " ";

   ++iter;
   if (cirMgr->getGate(*iter) == 0) {
      cout << "*";
   }
   if (_faninList.at(cirMgr->getGateInAll(*iter))) {
      cout << "!";
   }
   cout << *iter;

   cout << endl;
};
