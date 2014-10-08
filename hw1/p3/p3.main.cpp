/****************************************************************************
  FileName     [ p3.main.cpp ]
  PackageName  [ HW1 ]
  Synopsis     [ For problem 3 of HW1 in DSnP class ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include "p3.amgr.h"
#include "cstdlib"

using namespace std;

int main(int argc, char **argv)
{
   if (argc != 2) {
      cerr << "Error: use p3 <fileName>!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);
   if (!inf) {
      cerr << "Error: cannot open \"" << argv[1] << "\"!" << endl;
      exit(-1);
   }

   AMgr amgr;
   amgr.import(inf);

   if (!amgr.validate()) {
      cerr << "Error: data validation fails..." << endl;
      exit(-1);
   }

   cout << amgr << endl;
}
