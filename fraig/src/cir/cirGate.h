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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class AigGateV;
class CirGate;
extern unsigned long long dfsFlag;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class AigGateV
{
   #define NEG 0x1
public:
   AigGateV(CirGate* g = 0, size_t phase = 0): _gateV(size_t(g) + phase) {}
   CirGate* gate() const { return (CirGate*)(_gateV & ~size_t(NEG)); }
   bool isInv() const { return (_gateV & NEG); }

private:
   size_t _gateV;
};

class CirGate
{
public:
   CirGate() {}
   virtual ~CirGate() {

      for (vector<AigGateV>::const_iterator i = _faninList.begin(); i != _faninList.end(); ++i) {
         (*i).gate()->removeFromList("fanout", this, (*i).isInv());
      }

      for (vector<AigGateV>::const_iterator i = _fanoutList.begin(); i != _fanoutList.end(); ++i) {
         (*i).gate()->removeFromList("fanin", this, (*i).isInv());
      }

      _faninList.clear();
      _fanoutList.clear();
      _dfsFlag = 0;
   }

   // Basic access methods
   virtual string getTypeStr() const { return "0"; }
   unsigned getLineNo() const { return _lineNo; }
   string getSymbol() const { return _symbol; }
   void setSymbol(const string& s) { _symbol = s; }
   void setId(const int& id) { _id = id; }
   int  getId() const { return _id; }
   void mark() const { _dfsFlag = dfsFlag; }
   bool isMark() const { return (_dfsFlag == dfsFlag);}
   size_t getInListSize() const { return _faninList.size(); }
   size_t getOutListSize() const { return _fanoutList.size(); }
   AigGateV getFanin(const size_t& idx) { return _faninList[idx];}
   AigGateV getFanout(const size_t& idx) { return _fanoutList[idx];}

   // Printing functions
   virtual void printGate() const = 0; // called in dfsFan (class CirGate)
   virtual void pGateNet() const = 0; // called in printNetList (class cirMgr)
   void reportGate() const;
   void reportFanin(const int& level) const;
   void reportFanout(const int& level) const;

   // Helper function
   bool isUndef() const { return (getTypeStr() == "UNDEF");}
   void setLineNo(const int& n) { _lineNo = n; }
   void resetDfsFlag() { _dfsFlag = 0; }
   void addFanin(const AigGateV& in){ _faninList.push_back(in); }
   void addFanout(const AigGateV& out){ _fanoutList.push_back(out); }
   void dfsFan(const int&, const int&, const string&) const;

   void connectFaninToEachFanout(AigGateV fanin) {
      for (vector<AigGateV>::iterator i = _fanoutList.begin(); i != _fanoutList.end(); ++i) {

         AigGateV from(fanin.gate(), fanin.isInv() ^ (*i).isInv());
         (*i).gate()->addFanin(from);

         AigGateV to((*i).gate(), from.isInv());
         from.gate()->addFanout(to);
      }
   }

private:
   int _id;
   unsigned _lineNo;
   mutable unsigned _dfsFlag;
   string _symbol;

protected:
   vector<AigGateV> _faninList;
   vector<AigGateV> _fanoutList;

   void removeFromList(string listName, CirGate* gate, bool isInv) {
      /* cout << listName << " " << gate->getId() << " " << isInv << endl; */

      if (listName == "fanin") {
         for (vector<AigGateV>::iterator i = _faninList.begin(); i != _faninList.end();) {
            /* cout << (*i).gate()->getId() << " " << (*i).isInv() << endl; */
            if (((*i).gate() == gate) && ((*i).isInv() == isInv)) {
               /* cout << "Erase " << listName << " " << (*i).gate()->getId() << " from " << getId() << endl; */
               i = _faninList.erase(i);
            } else {
               ++i;
            }
         }

      } else {
         for (vector<AigGateV>::iterator i = _fanoutList.begin(); i != _fanoutList.end();) {
            if (((*i).gate() == gate) && ((*i).isInv() == isInv)) {
               i = _fanoutList.erase(i);
            } else {
               ++i;
            }
         }

      }
   }
};

class Aig: public CirGate
{
public:
   Aig(int id = 0, int in1 = 0, int in2 = 0, int n = 0) {
      setId(id);
      setLineNo(n);
      _in[0] = in1;
      _in[1] = in2;
      resetDfsFlag();
   }
   ~Aig() {}
   virtual string getTypeStr() const { return "AIG"; }
   virtual void printGate() const { cout << getTypeStr() << " " << getId(); }
   virtual void pGateNet() const;
   int getIn(const int& n) const { if (n < 2) return _in[n]; return 0;}
private:
   int _in[2];
};

class PI: public CirGate
{
public:
   PI(int id = 0, int n = 0) { setId(id); setLineNo(n); resetDfsFlag(); }
   ~PI() {}
   virtual string getTypeStr() const { if(!getId()) return "CONST"; return "PI"; }
   virtual void printGate() const { cout << getTypeStr() << " " << getId();  }
   virtual void pGateNet() const;
private:
};

class PO: public CirGate
{
public:
   PO(int id = 0, int n = 0, int i = 0) { setId(id); setLineNo(n); _in = i; resetDfsFlag(); }
   ~PO() {}
   virtual string getTypeStr() const { return "PO"; }
   virtual void printGate() const { cout << getTypeStr() << " " << getId(); }
   virtual void pGateNet() const;
   int getIn() const { return _in; }
private:
   int _in;
};

class Undefined: public CirGate
{
public:
   Undefined(int id = 0) { setId(id); resetDfsFlag(); }
   ~Undefined() {}
   virtual string getTypeStr() const { return "UNDEF"; }
   virtual void printGate() const { cout << getTypeStr() << " " << getId();}
   virtual void pGateNet() const {};
private:
};

#endif // CIR_GATE_H
