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

   _faninList.insert(pair<CirGate*, bool>(fanin, isInverted));
}

void
CirGate::addTmpFanin(int gateId, bool isInverted)
{
   // Similarly, we should have only 2 tmp
   // fanins on 1 gate
   assert (_tmpFaninList.size() < 2);
   _tmpFaninList.insert(pair<int, bool>(gateId, isInverted));
}

// -----------------
//    CirAndGate
// -----------------
