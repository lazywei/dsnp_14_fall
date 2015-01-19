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
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

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
      cout << "PI#1: " << tmp << endl;
      inputs.push_back(tmp);
    }

    for (size_t i = 0, sizeDfs = _dfsOrder.size(); i < sizeDfs; ++i) {
      CirGate* gate = _dfsOrder.at(i);
      gate->simulate();

      if (gate->getTypeStr() == "PO") {
        cout << "PO#1: " << gate->getSimResult() << endl;
        outputs.push_back(gate->getSimResult());
      }
    }

    if (_simLog) {

      for (size_t i = 0; i < 32; ++i) {
        unsigned mask = 1 << i;

        for (size_t j = 0; j < sizePi; ++j) {
          *_simLog << ((inputs.at(j) & mask) >> i);
        }

        *_simLog << " ";

        for (size_t j = 0; j < sizePo; ++j) {
          *_simLog << ((outputs.at(j) & mask) >> i);
        }

        *_simLog << endl;

      }

    }
  }
}

void
CirMgr::fileSim(ifstream& patternFile)
{
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

