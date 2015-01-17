/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

class myHashKey
{
public:
  myHashKey(CirGate* g) {
    _fanin1 = g->getFanin(0);
    _fanin2 = g->getFanin(1);
  }

  // Ref: http://stackoverflow.com/a/919661/1371471
  size_t operator() () const {
    size_t tmp1 = (_fanin1.gate()->getId() + _fanin2.gate()->getId()) % 1119;
    size_t tmp2 = (_fanin1.gate()->getId() + _fanin2.gate()->getId() + 1) % 1119;
    size_t res = (tmp1 * tmp2) % 1119;
    return  res;
  }

  bool operator == (const myHashKey& k) const {
    return (isSameAGV(k._fanin1, _fanin1) && isSameAGV(k._fanin2, _fanin2)) ||
            (isSameAGV(k._fanin1, _fanin2) && isSameAGV(k._fanin2, _fanin1));
  }

private:
  AigGateV _fanin1;
  AigGateV _fanin2;

  bool isSameAGV(const AigGateV& agv1, const AigGateV& agv2) const {
    return (agv1.gate()->getId() == agv2.gate()->getId()) && (agv1.isInv() == agv2.isInv());
  }
};

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
void
CirMgr::strash()
{
  HashMap<myHashKey, CirGate*> myHash;

  for (size_t i = 0, dfsSize = _dfsOrder.size(); i < dfsSize; ++i)
  {
    CirGate* gate = _dfsOrder.at(i);

    if (gate->getTypeStr() != "AIG") {
      continue;
    }

    CirGate* existedGate = new Aig();
    myHashKey key(gate);

    if (myHash.check(key, existedGate)) {
      cout << "Strashing: " << existedGate->getId() << " merging " << gate->getId() << "..." << endl;

      AigGateV newFanin(existedGate, 0);
      gate->connectFaninToEachFanout(newFanin);
      _aigs.erase(remove(_aigs.begin(), _aigs.end(), gate), _aigs.end());

      _dfsOrder.erase(_dfsOrder.begin() + i);
      --i, --dfsSize;

      delete gate;
    } else {
      myHash.insert(key, gate);
    }
  }

  _dfsOrder.clear();
  topoSort();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

