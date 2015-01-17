/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
void
CirMgr::sweep()
{
  for (map<int, CirGate*>::iterator iter = _all.begin(); iter != _all.end();) {

    CirGate* gate = iter->second;

    if (find(_dfsOrder.begin(), _dfsOrder.end(), gate) == _dfsOrder.end()) {

      if (gate->getTypeStr() == "PI" ||
          gate->getTypeStr() == "CONST" ||
          (gate->getOutListSize() > 0 && gate->getFanout(0).gate()->getTypeStr() == "PO")) {

        ++iter;
        _flFanout.push_back(gate->getId());

      } else {

        cout << "Sweeping: " << gate->getTypeStr() <<
          "(" << gate->getId() << ") removed ..." << endl;

        removeFromAigs(gate);
        removeFromFlFanin(gate->getId());
        removeFromFlFanout(gate->getId());

        _all.erase(iter++);

        delete gate;

      }
    } else {

      ++iter;

    }
  }
}

void
CirMgr::optimize()
{
  for (size_t i = 0, dfsSize = _dfsOrder.size(); i < dfsSize; ++i) {
    CirGate* gate = _dfsOrder.at(i);
    cout << "Visiting: " << gate->getId() << "..." << endl;

    if (gate->getTypeStr() == "PI" ||
        gate->getTypeStr() == "PO" ||
        gate->getTypeStr() == "CONST" ||
        gate->getTypeStr() == "UNDEF" ||
        gate->getInListSize() != 2) {
      continue;
    }

    AigGateV fanin1 = gate->getFanin(0);
    AigGateV fanin2 = gate->getFanin(1);

    if (fanin1.gate()->getTypeStr() == "CONST") {

      cout << "Simplifying: " << gate->getId() << "..." << endl;

      if (fanin1.isInv()) {
        // One of the fanin is 1 (TRUE)
        // Remove and replace by otherFanin
        gate->connectFaninToEachFanout(fanin2);
        deleteAndCleanUpGate(gate);
      } else {
        // One of the fanin is 0 (FALSE)
        // Remove and replace by 0 (itself)
        gate->connectFaninToEachFanout(fanin1);
        deleteAndCleanUpGate(gate);

        if (fanin2.gate()->getOutListSize() == 0) {
          if (fanin2.gate()->getTypeStr() == "PI" || fanin2.gate()->getTypeStr() == "AIG") {
            _flFanout.push_back(fanin2.gate()->getId());
          }
        }
      }

    } else if (fanin2.gate()->getTypeStr() == "CONST") {

      cout << "Simplifying: " << gate->getId() << "..." << endl;

      if (fanin2.isInv()) {
        gate->connectFaninToEachFanout(fanin1);
        deleteAndCleanUpGate(gate);
      } else {
        gate->connectFaninToEachFanout(fanin2);
        deleteAndCleanUpGate(gate);

        if (fanin1.gate()->getOutListSize() == 0) {
          if (fanin1.gate()->getTypeStr() == "PI" || fanin1.gate()->getTypeStr() == "AIG") {
            _flFanout.push_back(fanin1.gate()->getId());
          }
        }
      }

    } else if (fanin1.gate() == fanin2.gate()) {

      cout << "Simplifying: " << gate->getId() << "..." << endl;

      if (fanin1.isInv() == fanin2.isInv()) {
        // Two fanins are the same
        // Remove and replace by fanin1
        gate->connectFaninToEachFanout(fanin1);

      } else {
        // One is inverse of the other
        // Remove and replace by 0

        gate->connectFaninToEachFanout(fanin1);
      }

      deleteAndCleanUpGate(gate);
    }

  }

  _dfsOrder.clear();
  topoSort();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/


// Update _aigs, _flFanin, _flFanout
void
CirMgr::deleteAndCleanUpGate(CirGate* gate) {
  _all.erase(gate->getId());
  removeFromAigs(gate);
  removeFromFlFanin(gate->getId());
  removeFromFlFanout(gate->getId());
  delete gate;
}
