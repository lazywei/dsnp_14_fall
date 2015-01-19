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

      for (size_t pos = 0; pos < 32; ++pos) {

        for (size_t i = 0; i < sizePi; ++i) {
          *_simLog << getBit(inputs.at(i), pos);
        }

        *_simLog << " ";

        for (size_t i = 0; i < sizePo; ++i) {
          *_simLog << getBit(outputs.at(i), pos);
        }

        *_simLog << endl;

      }

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
