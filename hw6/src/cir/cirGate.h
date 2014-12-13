/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

#include <map>

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(int id, int lineNo, int colNo) :
      _id(id), _lineNo(lineNo), _colNo(colNo)
   {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return _typeStr; }
   unsigned getLineNo() const { return _lineNo; }
   unsigned getId() const { return _id; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   // My helper functions
   void addFanin(CirGate*, bool);
private:

protected:
   string   _typeStr;

   int      _id;
   int      _lineNo;
   int      _colNo;

   // <fanin, isInverted>
   map<CirGate*, bool> _faninList;
};

class CirPiGate : public CirGate {
public:
   CirPiGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "PI";
      }
   void printGate() const {};
};

class CirPoGate : public CirGate {
public:
   CirPoGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "PO";
         _tmpFaninId = -1;
         _tmpFaninInverted = false;
      }
   void printGate() const {};

   void addTmpFanin(int id, bool isInverted) {
      _tmpFaninId = id;
      _tmpFaninInverted = isInverted;
   }

   void getTmpFanin(int& id, bool& isInverted) {
      id = _tmpFaninId;
      isInverted = _tmpFaninInverted;
   }

private:
   int  _tmpFaninId;
   bool _tmpFaninInverted;
};

class CirAndGate : public CirGate {
public:
   CirAndGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "AND";
         _tmpFaninId_1 = -1;
         _tmpFaninId_2 = -1;
         _tmpFaninInverted_1 = false;
         _tmpFaninInverted_2 = false;
      }
   void printGate() const {};

   void addTmpFanin(int id_1, bool isInverted_1, int id_2, bool isInverted_2) {
      _tmpFaninId_1 = id_1;
      _tmpFaninInverted_1 = isInverted_1;

      _tmpFaninId_2 = id_2;
      _tmpFaninInverted_2 = isInverted_2;
   }

   void getTmpFanin(int& id_1, bool& isInverted_1, int& id_2, bool& isInverted_2) {
      id_1 = _tmpFaninId_1;
      isInverted_1 = _tmpFaninInverted_1;

      id_2 = _tmpFaninId_2;
      isInverted_2 = _tmpFaninInverted_2;
   }

private:
   int  _tmpFaninId_1;
   bool _tmpFaninInverted_1;

   int  _tmpFaninId_2;
   bool _tmpFaninInverted_2;
};

class CirUndefGate : public CirGate {
public:
   CirUndefGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "UNDEF";
      }
   void printGate() const {};
};

class CirConstGate : public CirGate {
public:
   CirConstGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "CONST";
      }
   void printGate() const {};
};


#endif // CIR_GATE_H
