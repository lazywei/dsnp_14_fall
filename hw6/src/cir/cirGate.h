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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(int id, int lineNo, int colNo, bool isInverted) :
      _id(id), _lineNo(lineNo), _colNo(colNo), _isInverted(isInverted)
   {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return 0; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

private:

protected:
   string           _typeStr;

   vector<CirGate*> _fanins;
   vector<CirGate*> _fanouts;

   int  _id;
   int  _lineNo;
   int  _colNo;

   bool _isInverted;
};

class CirPiGate : public CirGate {
   CirPiGate(int id, int lineNo, int colNo, bool isInverted) :
      CirGate(id, lineNo, colNo, isInverted) {
         _typeStr = "PI";
      }
};

class CirPoGate : public CirGate {
   CirPoGate(int id, int lineNo, int colNo, bool isInverted) :
      CirGate(id, lineNo, colNo, isInverted) {
         _typeStr = "PO"; 
      }
};

class CirAndGate : public CirGate {
   CirAndGate(int id, int lineNo, int colNo, bool isInverted) :
      CirGate(id, lineNo, colNo, isInverted) {
         _typeStr = "AND"; 
      }
};

class CirConstGate : public CirGate {
   CirConstGate(int id, int lineNo, int colNo, bool isInverted) :
      CirGate(id, lineNo, colNo, isInverted) {
         _typeStr = "Const"; 
      }
};

class CirUndefGate : public CirGate {
   CirUndefGate(int id, int lineNo, int colNo, bool isInverted) :
      CirGate(id, lineNo, colNo, isInverted) {
         _typeStr = "UNDEF"; 
      }
};

#endif // CIR_GATE_H
