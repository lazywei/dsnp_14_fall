/****************************************************************************
  FileName     [ p3.amgr.h ]
  PackageName  [ HW1 ]
  Synopsis     [ For problem 3 of HW1 in DSnP class ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef P3_AMGR_H
#define P3_AMGR_H

#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class A;
typedef map<string, A*>          AMap;
typedef vector<A*>               AList;
typedef pair<unsigned, unsigned> Occurence;

class A 
{
public:
   A();
   A(const string&);
   ~A();

   void addOccurence(unsigned l, unsigned w) {
      _occurence.push_back(Occurence(l, w)); }

   static unsigned getCount() { return _count; }

   friend ostream& operator<< (ostream&, const A&);

private:
   static unsigned _count;

   string             _data;
   vector<Occurence>  _occurence;
};

class AMgr
{
public:
   void cleanAll();
   bool checkEmpty() const { return _alist.empty(); }
   void import(ifstream& inf);

   bool validate() const { return (_alist.size() == A::getCount()); }

   friend ostream& operator<< (ostream&, const AMgr&);

private:
   AMap   _amap;
   AList  _alist;

   void newEntry(const string&, unsigned, unsigned);
};

#endif
