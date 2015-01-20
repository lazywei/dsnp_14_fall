/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <set>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

class mySimValKey
{
public:
  mySimValKey(unsigned simVal) {
    _simVal = simVal;
  }

  // Ref: http://stackoverflow.com/a/919661/1371471
  size_t operator() () const {
    return (_simVal % 1119) * (~_simVal % 1119);
  }

  bool operator == (const mySimValKey& k) const {
    return (_simVal == k._simVal) || ((~_simVal) == k._simVal);
  }

private:
  unsigned _simVal;
};

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  size_t sizePi = _pis.size();
  size_t sizePo = _pos.size();

  vector<unsigned> inputs;
  vector<unsigned> outputs;

  for (size_t round = 0; round < sizePi; ++round) {
    inputs.clear();
    outputs.clear();

    for (size_t i = 0; i < sizePi; ++i) {
      unsigned tmp = rnGen(1 << 31);
      PI* pi = _pis.at(i);
      pi->setSimResult(tmp);
      inputs.push_back(tmp);
    }

    for (size_t i = 0, sizeDfs = _dfsOrder.size(); i < sizeDfs; ++i) {
      CirGate* gate = _dfsOrder.at(i);
      gate->simulate();
      gate->setFecGrpIdx(-1);

      if (gate->getTypeStr() == "PO") {
        outputs.push_back(gate->getSimResult());
      }
    }

    if (_simLog) {

      for (size_t pos = 0; pos < 32; ++pos) {

        for (size_t i = 0; i < sizePi; ++i) {
          *_simLog << getBit(inputs.at(i), 32-pos);
        }

        *_simLog << " ";

        for (size_t i = 0; i < sizePo; ++i) {
          *_simLog << getBit(outputs.at(i), 32-pos);
        }

        *_simLog << endl;

      }

    }

    // Handle FEC groups
    if (_fecGrps.size() > 0) {
      checkFEC();
    } else {
      initFEC();
    }
  }
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  size_t sizePi = _pis.size();
  string temp;
  vector <string> lines;
  while (getline(patternFile,temp)) {
    if (temp.size() != sizePi) {
      cerr << "Pattern(" << temp << ") length(" << temp.size() << ") does not match "
        "the number of inputs(" << sizePi << ") in a circuit!!"<< endl;
      cout << "0 patterns simulated." << endl;
      return;
    } else if (temp.find_first_not_of("01") != string::npos) {
      cerr << "Pattern(" << temp << ") contains a non-0/1 character" << endl;
      cout << "0 patterns simulated." << endl;
      return;
    }

    lines.push_back(temp);
  }
  patternFile.close();


  for (size_t i = 0; i < lines.size(); ++i) {

    string line = lines.at(i);
    vector<int> outputs;

    for (size_t j = 0; j < line.size(); ++j) {
      _pis.at(j)->setSimResult(line.at(j) - '0');
    }

    for (size_t j = 0, sizeDfs = _dfsOrder.size(); j < sizeDfs; ++j) {
      CirGate* gate = _dfsOrder.at(j);
      gate->simulate();
      gate->setFecGrpIdx(-1);

      if (gate->getTypeStr() == "PO") {
        outputs.push_back(gate->getSimResult());
      }
    }

    if (_simLog) {
      *_simLog << lines.at(i) << " ";

      for (size_t j = 0; j < _pos.size(); ++j) {
        *_simLog << outputs.at(j);
      }

      *_simLog << endl;
    }

    // Handle FEC groups
    if (_fecGrps.size() > 0) {
      checkFEC();
    } else {
      initFEC();
    }
  }
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

size_t
CirMgr::getBit(unsigned x, size_t pos) {
  unsigned mask = 1 << pos;
  return ((x & mask) >> pos);
}


void
CirMgr::initFEC() {
  HashMap<mySimValKey, IdList*> fecHash;
  set<IdList*> validFECGrp;

  for (size_t i = 0, sizeDfs = _dfsOrder.size(); i < sizeDfs; ++i) {
    CirGate* gate = _dfsOrder.at(i);

    if (gate->getTypeStr() != "AIG" && gate->getTypeStr() != "CONST") {
      // only AIG and CONST 0 can be included in FEC pairs
      continue;
    }

    mySimValKey key(gate->getSimResult());
    IdList* grp;

    if (fecHash.check(key, grp)) {
      int inv = 0;

      if (getGate(grp->front() / 2)->getSimResult() == gate->getSimResult()) {
        inv = 0;
      } else {
        inv = 1;
      }

      grp->push_back(gate->getId() * 2 + inv);
      if (grp->size() > 1) {
        validFECGrp.insert(grp);
      }
    } else {
      grp = new IdList();
      grp->push_back(gate->getId() * 2);
      fecHash.insert(key, grp);
    }

  }

  for (set<IdList*>::const_iterator iter = validFECGrp.begin(); iter != validFECGrp.end(); ++iter) {
    IdList* idList = *iter;
    _fecGrps.push_back(idList);

    for (size_t i = 0, sizeIdList = idList->size(); i < sizeIdList; ++i) {
      CirGate* gate = getGate(idList->at(i) / 2);
      gate->setFecGrpIdx(_fecGrps.size() - 1);
    }
  }
}

void
CirMgr::checkFEC() {
  vector<IdList*> newGrps;
  set<IdList*> validFECGrp;

  for (size_t i = 0, sizeGrps = _fecGrps.size(); i < sizeGrps; ++i) {
    IdList* idList = _fecGrps.at(i);

    HashMap<mySimValKey, IdList*> *fecHash = new HashMap<mySimValKey, IdList*>;

    for (size_t j = 0, sizeIdList = idList->size(); j < sizeIdList; ++j) {
      CirGate* gate = getGate(idList->at(j) / 2);

      if (gate->getTypeStr() != "AIG" && gate->getTypeStr() != "CONST") {
        // only AIG and CONST 0 can be included in FEC pairs
        continue;
      }

      mySimValKey key(gate->getSimResult());
      IdList* grp;

      if (fecHash->check(key, grp)) {
        int inv = 0;

        /* cout << gate->getId() << ": " << gate->getSimResult() << " ~" << ~gate->getSimResult() << endl; */
        /* cout << gate->getId() << ": " << gate->getSimResult() << endl; */
        /* cout << grp->front() / 2 << ": " << getGate(grp->front() / 2)->getSimResult() << endl; */
        /* cout << grp->front() / 2 << ": " << ~(getGate(grp->front() / 2)->getSimResult()) << endl; */
        if (getGate(grp->front() / 2)->getSimResult() == gate->getSimResult()) {
          inv = 0;
        } else {
          inv = 1;
        }

        grp->push_back(gate->getId() * 2 + inv);
        if (grp->size() > 1) {
          validFECGrp.insert(grp);
        }
      } else {
        grp = new IdList();
        grp->push_back(gate->getId() * 2);
        fecHash->insert(key, grp);
      }
    }

    delete fecHash;
  }


  for (set<IdList*>::const_iterator iter = validFECGrp.begin(); iter != validFECGrp.end(); ++iter) {
    IdList* idList = *iter;
    newGrps.push_back(idList);

    for (size_t i = 0, sizeIdList = idList->size(); i < sizeIdList; ++i) {
      CirGate* gate = getGate(idList->at(i) / 2);
      gate->setFecGrpIdx(newGrps.size() - 1);
    }
  }

  _fecGrps.swap(newGrps);
}
