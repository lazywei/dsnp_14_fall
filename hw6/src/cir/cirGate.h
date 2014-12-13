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
   void addTmpFanin(int, bool);
private:

protected:
   string   _typeStr;

   int      _id;
   int      _lineNo;
   int      _colNo;

   // <fanin, isInverted>
   map<CirGate*, bool> _faninList;

   // <gateId, isInverted>
   map<int, bool> _tmpFaninList;

   /* CirGate* _fanin1; */
   /* bool     _isInverted1; */

   /* CirGate* _fanin2; */
   /* bool     _isInverted2; */
};

class CirPiGate : public CirGate {
public:
   CirPiGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "PI";
      }
   void printGate() const;
};

class CirPoGate : public CirGate {
public:
   CirPoGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "PO";
      }
   void printGate() const;
};

class CirAndGate : public CirGate {
public:
   CirAndGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "AND";
      }
   void printGate() const;
};

class CirConstGate : public CirGate {
public:
   CirConstGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "Const";
      }
   void printGate() const;
};

class CirUndefGate : public CirGate {
public:
   CirUndefGate(int id, int lineNo, int colNo) :
      CirGate(id, lineNo, colNo) {
         _typeStr = "UNDEF";
      }
   void printGate() const;
};

#endif // CIR_GATE_H
