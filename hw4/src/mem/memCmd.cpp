/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(sizeof(MemTestObj))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   CmdExec::lexOptions(option, options);

   size_t nOpt = options.size(), numObj = 0, arrSize = 0;
   size_t i = 0;
   int tmp;

   if (nOpt > 3) {
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[nOpt-1]);
   } else if (nOpt == 2) {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }

   while (i < nOpt) {

      if (myStrNCmp("-Array", options[i], 2) == 0) {
         if (i+1 >= nOpt) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         }
         if (!myStr2Int(options[i+1], tmp) || tmp <= 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
         } else {
            arrSize = tmp;
         }

         i++;
      } else {
         if (!myStr2Int(options[i], tmp) || tmp <= 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         } else {
            numObj = tmp;
         }
      }

      i++;
   }

   if (numObj == 0) {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }

   try {
      if (arrSize == 0) {
         mtest.newObjs(numObj);
      } else {
         mtest.newArrs(numObj, arrSize);
      }
   } catch(const bad_alloc& e) {
      cout << e.what() << endl;
   }

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO

   vector<string> options;
   CmdExec::lexOptions(option, options);

   size_t nOpt = options.size(), objId = 0, numRandId = 0;
   size_t i = 0;
   int tmp;
   bool isArr = false, isIndex = false, isRandom = false;
   string oldIndex, oldRandom;

   if (nOpt != 2 && nOpt != 3) {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }

   while (i < nOpt) {
      if (myStrNCmp("-Index", options[i], 2) == 0) {

         if (i+1 >= nOpt) {
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
         }

         if (!myStr2Int(options[i+1], tmp) || tmp <= 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
         }

         isIndex = true;
         objId = tmp;
         oldIndex = options[i+1];
         i++;

      } else if (myStrNCmp("-Random", options[i], 2) == 0) {
         if (i+1 >= nOpt) {
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
         }

         if (!myStr2Int(options[i+1], tmp) || tmp <= 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i+1]);
         }

         isRandom = true;
         numRandId = tmp;
         oldRandom = options[i+1];
         i++;

      } else if (myStrNCmp("-Array", options[i], 2) == 0) {
         isArr = true;
      }
      i++;
   }

   if (isIndex) {
      if (isArr) {
         if (objId >= mtest.getArrListSize())
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, oldIndex);
         mtest.deleteArr(objId);
      } else {
         if (objId >= mtest.getObjListSize())
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, oldIndex);
         mtest.deleteObj(objId);
      }

   } else if (isRandom) {
      if (isArr) {
         for (i = 0; i < numRandId; ++i)
         {
            mtest.deleteArr( rnGen(mtest.getArrListSize()) );
         }
      } else {
         for (i = 0; i < numRandId; ++i)
         {
            mtest.deleteObj( rnGen(mtest.getObjListSize()) );
         }
      }

   } else {
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}


