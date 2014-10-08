/****************************************************************************
  FileName     [ p3.amgr.cpp ]
  PackageName  [ HW1 ]
  Synopsis     [ For problem 3 of HW1 in DSnP class ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>
#include "p3.amgr.h"
#include "iterator"

using namespace std;

// TODO: Initialize static data variable in class A

unsigned int A::_count = 0;

// TODO: Implement class A's constructors and desctructor

A::A()
{
   ++_count;
}

A::A(const string& str) 
{
   _data = str;
   ++_count;
}

A::~A()
{
   cout << "destructor called" << endl;
   --_count;
}


void AMgr::cleanAll()
{
   // TODO: clean up all the data in AMgr::_amap and AMgr::_alist
   AList().swap(_alist);
   AMap().swap(_amap);
}

// If _amap or _alist is not empty, it will be cleaned anyway.
void AMgr::import(ifstream& inf)
{
   if (!checkEmpty()) {
      cout << "Warning: Database is not empty. Erasing..."; cout.flush();
      cleanAll();
      cout << " done." << endl;
   }

   #define BUF_LEN 65536
   char buf[BUF_LEN];
   unsigned lineNo = 0;
   do {
      // Read one line at a time, and parse this line into string tokens
      // The parsed string tokens are stored in vector<string> strs
      inf.getline(buf, BUF_LEN);
      stringstream ss((string(buf)));
      istream_iterator<string> bb(ss);
      istream_iterator<string> ee;
      vector<string> strs(bb, ee);

      // Store the parsed string tokens to both _amap and _alist
      for (size_t i = 0, s = strs.size(); i < s; ++i) {
         newEntry(strs[i], lineNo, i);
      }

      lineNo++;
   } while (!inf.eof());
}

// TODO:
// 1. Check is there is already an entry (str, A*) in _amap
// 2. If not, create a new A*, map (str, A*), and store this A* to _alist
// 3. Record the (lineNo, wordNo) to A::_occurence
void AMgr::newEntry(const string& str, unsigned lineNo, unsigned wordNo)
{
   if (!_amap.count(str))
   {
      A* a = new A(str);
      _amap[str] = a;
      _alist.push_back(a);
   }
   _amap[str]->addOccurence(lineNo, wordNo);
}

// TODO: Implement the ostream& operator<< for AMgr and A.
// A sample input/output can be found at p3.in and p3.out.ref
// 
ostream& operator << (ostream& os, const AMgr& am)
{
   os << "Total distinct words: " << am._alist.size() << endl;
   for(size_t i = 0; i < am._alist.size(); i++) {
      os << "[" << i << "] " << *(am._alist[i]) << endl;
   }
   return os;
}

ostream& operator << (ostream& os, const A& a)
{
   os << a._data << endl;
   for(size_t i = 0; i < a._occurence.size(); i++) {
      os << "( " << a._occurence[i].first << ", "
         << a._occurence[i].second << " )";
      os << "  ";
      if (i != a._occurence.size() - 1)
      {
      } else {
         os << endl;
      }
   }
   return os;
}
