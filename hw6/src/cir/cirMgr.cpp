/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>


using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream infile(fileName.c_str());
   istringstream iss;
   string line;
   string dummy; // Dump unused string

   int literal;
   int lhs, rhs_1, rhs_2; // For AND Gate
   int lineNo = 1, colNo = 1;

   int id;
   bool isInverted;

   int id_2;
   bool isInverted_2;

   getline(infile, line);

   iss.str(line);

   iss >> dummy; // aag

   iss >> _nMax;
   iss >> _nInput;

   iss >> dummy; // n of latch

   iss >> _nOutput;
   iss >> _nAnd;

   for (int i = 0; i < _nInput; ++i)
   {
      ++lineNo;
      getline(infile, line);

      iss.clear();
      iss.str(line);
      iss >> literal;

      addPi(literal, lineNo, colNo);
   }

   for (int i = 0; i < _nOutput; ++i)
   {
      ++lineNo;
      getline(infile, line);

      iss.clear();
      iss.str(line);
      iss >> literal;

      addPo(literal, lineNo, colNo, _nMax+i+1);
   }

   for (int i = 0; i < _nAnd; ++i)
   {
      ++lineNo;

      getline(infile, line);

      iss.clear();
      iss.str(line);
      iss >> lhs;
      iss >> rhs_1;
      iss >> rhs_2;

      literal = lhs;

      parseId(literal, id, isInverted);

      CirAndGate* andGate = new CirAndGate(id, lineNo, colNo);

      // -- Parse RHS --
      parseId(rhs_1, id, isInverted);
      parseId(rhs_2, id_2, isInverted_2);
      andGate->addTmpFanin(id, isInverted, id_2, isInverted_2);
      // ---------------

      _andList.insert(pair<int, CirAndGate*>(andGate->getId(), andGate));
   }

   while (getline(infile, line) && line.size() > 0 && line.at(0) != 'c') {
      if (line.at(0) == 'i') {
         _piList.at(_orderedPiList.at(line.at(1) - '0'))->symbol = line.substr(3);
      } else if (line.at(0) == 'o') {
         _poList.at(_orderedPoList.at(line.at(1) - '0'))->symbol = line.substr(3);
      }
   }

   // Now, start connect or set undef gates
   for (map<int, CirPoGate*>::iterator i = _poList.begin(); i != _poList.end(); ++i) {
      realizePoFanin(i->second);
   }

   for (map<int, CirAndGate*>::const_iterator i = _andList.begin(); i != _andList.end(); ++i) {
      realizeAndFanin(i->second);
   }

   /* cout << "----reporting----" << endl; */
   /* cout << "PIs: " << _piList.size() << endl; */
   /* cout << "POs: " << _poList.size() << endl; */
   /* cout << "AIGs: " << _andList.size() << endl; */
   /* cout << "UNDEFs: " << _undefList.size() << endl; */
   /* cout << "CONST:" << _constGate << endl; */

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;

   cout << "  PI";
   cout.width(12);
   cout << right << _piList.size() << endl;

   cout << "  PO";
   cout.width(12);
   cout << right <<  _poList.size() << endl;

   cout << "  AIG";
   cout.width(11);
   cout << right <<  _andList.size() << endl;
   cout << "------------------" << endl;

   cout << "  Total";
   cout.width(9);
   cout << right << (_piList.size() + _poList.size() + _andList.size()) << endl;
}

void
CirMgr::printNetlist() const
{
   CirGate::setGlobalRef();
   _dfsIdList.clear();

   CirGate* toPrint;
   int counter = 0;

   cout << endl;

   for (vector<int>::const_iterator i = _orderedPoList.begin(); i != _orderedPoList.end(); ++i) {
      getGate(*i)->dfsTraversal(_dfsIdList);
   }

   for (vector<int>::const_iterator i = _dfsIdList.begin(); i != _dfsIdList.end(); ++i) {
      toPrint = getGate(*i);
      cout << "[" << counter++ << "] ";
      toPrint->printGate();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit: ";

   for (vector<int>::const_iterator i = _orderedPiList.begin(); i != _orderedPiList.end(); ++i) {
      cout << *i;
      if (i+1 != _orderedPiList.end()) {
         cout << " ";
      }
   }

   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit: ";

   for (vector<int>::const_iterator i = _orderedPoList.begin(); i != _orderedPoList.end(); ++i) {
      cout << *i;
      if (i+1 != _orderedPoList.end()) {
         cout << " ";
      }
   }

   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   vector<int> ids;

   // floating -- fanin not define

   for (vector<int>::const_iterator i = _orderedPoList.begin(); i != _orderedPoList.end(); ++i) {
      if (getGate(*i)->isFloating()) {
         ids.push_back(*i);
      }
   }

   for (map<int, CirAndGate*>::const_iterator i = _andList.begin(); i != _andList.end(); ++i) {
      if (i->second->isFloating()) {
         ids.push_back(i->first);
      }
   }

   if (!ids.empty()) {
      sort(ids.begin(), ids.end());
      cout << "Gates with floating fanin(s): ";
      for (vector<int>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
         cout << *i;
         if (i+1 != ids.end()) {
            cout << " ";
         }
      }
      cout << endl;
   }

   ids.clear();

   // define but not used -- no fanouts

   for (vector<int>::const_iterator i = _orderedPiList.begin(); i != _orderedPiList.end(); ++i) {
      if (getGate(*i)->getFanoutList().empty()) {
         ids.push_back(*i);
      }
   }

   for (map<int, CirAndGate*>::const_iterator i = _andList.begin(); i != _andList.end(); ++i) {
      if (i->second->getFanoutList().empty()) {
         ids.push_back(i->first);
      }
   }

   if (!ids.empty()) {
      sort(ids.begin(), ids.end());
      cout << "Gates defined but not used  : ";
      for (vector<int>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
         cout << *i;
         if (i+1 != ids.end()) {
            cout << " ";
         }
      }
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   CirGate* targetGate;
   vector<GateIdInv>::const_iterator targetFanin;

   if (_dfsIdList.empty()) {
      CirGate::setGlobalRef();

      for (vector<int>::const_iterator i = _orderedPoList.begin(); i != _orderedPoList.end(); ++i) {
         getGate(*i)->dfsTraversal(_dfsIdList);
      }
   }

   outfile << "aag "
      << _nMax << " "
      << _nInput << " 0 "
      << _nOutput << " "
      << _nAnd << endl;

   for (vector<int>::const_iterator i = _orderedPiList.begin(); i != _orderedPiList.end(); ++i) {
      outfile << 2*(*i) << endl;
   }

   for (vector<int>::const_iterator i = _orderedPoList.begin(); i != _orderedPoList.end(); ++i) {
      targetFanin = getGate(*i)->getFaninList().begin();

      if (targetFanin->second) {
         outfile << 2*(targetFanin->first) + 1 << endl;
      } else {
         outfile << 2*(targetFanin->first) << endl;
      }
   }

   for (vector<int>::const_iterator i = _dfsIdList.begin(); i != _dfsIdList.end(); ++i) {
      if (_andList.count(*i)) {
         targetGate = _andList.at(*i);

         vector<GateIdInv>::const_iterator fanin = targetGate->getFaninList().begin();

         outfile << 2*(*i) << " ";

         if (fanin->second) {
            outfile << 2*(fanin->first)+1;
         } else {
            outfile << 2*(fanin->first);
         }
         outfile << " ";

         ++fanin;

         if (fanin->second) {
            outfile << 2*(fanin->first)+1;
         } else {
            outfile << 2*(fanin->first);
         }
         outfile << endl;
      }
   }
}

/**********************************************************/
/*   class CirMgr helper member functions                 */
/**********************************************************/
void
CirMgr::parseId(const int& literal, int& id, bool& isInverted)
{
   assert (literal >= 0);

   if (literal % 2 == 0) {
      id = literal / 2;
      isInverted = false;
   } else {
      id = (literal - 1) / 2;
      isInverted = true;
   }
}

void
CirMgr::realizePoFanin(CirPoGate* poGate)
{
   int id;
   bool isInverted;

   CirGate* fanin;

   poGate->getTmpFanin(id, isInverted);

   if (id != -1) {
      if (id == 0 || id == 1) {
         createConstGate();
      }

      fanin = getGateInAll(id);
      if (fanin == 0) {
         fanin = new CirUndefGate(id, -1, -1);
         _undefList.insert(pair<int, CirUndefGate*>(fanin->getId(), (CirUndefGate*) fanin));
      }

      poGate->addFanin(fanin, isInverted);
   }
}

void
CirMgr::realizeAndFanin(CirAndGate* andGate)
{
   int id_1;
   bool isInverted_1;

   int id_2;
   bool isInverted_2;

   CirGate* fanin;

   andGate->getTmpFanin(id_1, isInverted_1, id_2, isInverted_2);

   if (id_1 != -1) {
      if (id_1 == 0 || id_1 == 1) {
         createConstGate();
      }

      fanin = getGateInAll(id_1);
      if (fanin == 0) {
         fanin = new CirUndefGate(id_1, -1, -1);
         _undefList.insert(pair<int, CirUndefGate*>(fanin->getId(), (CirUndefGate*) fanin));
      }

      andGate->addFanin(fanin, isInverted_1);
   }

   if (id_2 != -1) {
      if (id_2 == 0 || id_2 == 1) {
         createConstGate();
      }

      fanin = getGateInAll(id_2);
      if (fanin == 0) {
         fanin = new CirUndefGate(id_2, -1, -1);
         _undefList.insert(pair<int, CirUndefGate*>(fanin->getId(), (CirUndefGate*) fanin));
      }

      andGate->addFanin(getGateInAll(id_2), isInverted_2);
   }
}

CirGate*
CirMgr::getGateInAll(const int& id) const
{
   if (_piList.count(id)) {
      return _piList.at(id);
   } else if (_poList.count(id)) {
      return _poList.at(id);
   } else if (_andList.count(id)) {
      return _andList.at(id);
   } else if (_undefList.count(id)) {
      return _undefList.at(id);
   } else if (id == 0) {
      return getConstGate();
   } else {
      return 0;
   }
}
