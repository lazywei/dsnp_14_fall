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
    _faninID1 = g->getFanin(0).gate()->getId();
    _faninID2 = g->getFanin(1).gate()->getId();
  }
  myHashKey() : _faninID1(0), _faninID2(0) {}

  // Ref: http://stackoverflow.com/a/919661/1371471
  size_t operator() () const {
    size_t tmp1 = (_faninID1 + _faninID2) % 1119;
    size_t tmp2 = (_faninID1 + _faninID2 + 1) % 1119;
    size_t res = (tmp1 * tmp2) % 1119;
    return  res;
  }

  bool operator == (const myHashKey& k) const {
    return ((k._faninID1 == _faninID1) && (k._faninID2 == _faninID2)) ||
            ((k._faninID1 == _faninID2) && (k._faninID2 == _faninID1));
  }

private:
  size_t _faninID1;
  size_t _faninID2;
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

      _dfsOrder.erase(_dfsOrder.begin() + i);
      --i, --dfsSize;

      delete gate;
    } else {
      myHash.insert(key, gate);
    }
  }
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

