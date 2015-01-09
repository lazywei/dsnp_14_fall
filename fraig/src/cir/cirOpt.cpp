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

      if (gate->getTypeStr() == "PI") {

        ++iter;
        _flFanout.push_back(gate->getId());

      } else {

        cout << "Sweeping " << gate->getTypeStr() <<
          "(" << gate->getId() << ") removed ..." << endl;

        // Ref: http://stackoverflow.com/a/3385251/1371471
        // Update _aigs, _flFanin, _flFanout
        _aigs.erase(remove(_aigs.begin(), _aigs.end(), gate), _aigs.end());
        _flFanin.erase(remove(_flFanin.begin(), _flFanin.end(), gate->getId()), _flFanin.end());
        _flFanout.erase(remove(_flFanout.begin(), _flFanout.end(), gate->getId()), _flFanout.end());

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
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/

