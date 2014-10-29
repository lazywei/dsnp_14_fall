/****************************************************************************
  FileName     [ calcGNum.h ]
  PackageName  [ calc ]
  Synopsis     [ Define class GNum ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef CALC_G_NUM_H
#define CALC_G_NUM_H

#include <iostream>
#include <vector>
#include <map>

#include "util.h"

using namespace std;

class GNum;

typedef map<string, GNum> CalcMap;

class GNum
{
public:
   // Default constructor
   // TODO: Implement the default constructor;
   // DONE
   GNum(int i = 0) : _num(i) {}
   // Get the GNum from the _varMap based on the string "str".
   // If found, copy the value.
   // If not found, create a default one and insert to the _varMap.
   GNum(const string& str) { _num = _varMap[str]._num; }

   // Operator overload
   // TODO: overload the following operators (+, +=, -, -=, *, *=, ==, !=, =)
   // DONE
   //
   GNum operator + (const GNum& n) const
   {
      return GNum(_num + n._num);
   }

   GNum& operator += (const GNum& n)
   {
      _num += n._num;
      return (*this);
   }

   GNum operator - (const GNum& n) const
   {
      return GNum(_num - n._num);
   }

   GNum& operator -= (const GNum& n)
   {
      _num -= n._num;
      return (*this);
   }

   GNum operator * (const GNum& n) const
   {
      return GNum(_num * n._num);
   }

   GNum& operator *= (const GNum& n)
   {
      _num *= n._num;
      return (*this);
   }

   bool operator == (const GNum& n) const
   {
      return _num == n._num;
   }

   bool operator != (const GNum& n) const
   {
      return _num != n._num;
   }

   GNum& operator = (const GNum& n)
   {
      _num = n._num;
      return (*this);
   }

   // static methods
   static void setBase(int b) { _base = b; }
   static int getBase() { return _base; }
   //
   // [TODO] Set the variable 's' in the _varMap to value 'n',
   // no matter the variable 's' exists in _varMap or not
   // DONE
   static void setVarVal(const string& s, const GNum& n)
   {
      _varMap[s] = n;
   }

   //
   // [TODO] Get the value of variable 's'.
   // If 's' can be found, store the value in 'n' and return true.
   // Otherwise ('s' not found), return false.
   // DONE
   static bool getVarVal(const string& s, GNum& n)
   {
      if (_varMap.count(s))
      {
         n = _varMap[s];
         return true;
      }
      else
      {
         return false;
      }
   }

   //
   // [TODO] If 's' is a valid variable name, return "getVarVal(s, n)";
   // else if 's' is a valid number, convert it to GNum and assign to 'n'
   static bool getStrVal(const string& s, GNum& n)
   {
      if (isValidVarName(s))
      {
         return getVarVal(s, n);
      }
      else
      {
         // [TODO] How to check if it is a valid number
         return true;
      }
   }

   //
   // [TODO] Print out all the variables in _varMap, one variable per line,
   // in the following format (assume _base = 16) ---
   // a = #9
   // b = #1a
   // kkk = #f1c
   static void printVars() { }
   static void resetVapMap() { }

   // friend functions
   friend ostream& operator << (ostream&, const GNum&);  // TODO

private:
   // Data members
   // DO NOT add/delete/modify data members
   int                _num;

   static int         _base;
   static CalcMap     _varMap;
};

#endif // CALC_MOD_NUM_H
