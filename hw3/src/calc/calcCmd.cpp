/****************************************************************************
  FileName     [ calcCmd.cpp ]
  PackageName  [ calc ]
  Synopsis     [ Define calculator commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "util.h"
#include "calcCmd.h"
#include "calcGNum.h"

bool
initCalcCmd()
{
   // TODO...
   if (!(cmdMgr->regCmd("GNADD", 5, new GNAddCmd) &&
         cmdMgr->regCmd("GNPrint", 3, new GNPrintCmd)
         /* cmdMgr->regCmd("HELp", 3, new HelpCmd) && */
         /* cmdMgr->regCmd("DOfile", 2, new DofileCmd) */
      )) {
      cerr << "Registering \"init\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

//----------------------------------------------------------------------
//    GNSET <(int base)>
//----------------------------------------------------------------------
CmdExecStatus
GNSetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;

   int m;
   if (!myStr2Int(token, m) || (m <= 1) || (m >= 37))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);

   if (GNum::getBase() != m) {
      GNum::setBase(m);
   }

   return CMD_EXEC_DONE;
}

void
GNSetCmd::usage(ostream& os) const
{
   os << "Usage: GNSET <(int base)>" << endl;
}

void
GNSetCmd::help() const
{
   cout << setw(15) << left << "GNSET: "
        << "set the base of the number system" << endl;
}


//----------------------------------------------------------------------
//    GNVARiable <(string var)> <(string var) | #(int val)>
//----------------------------------------------------------------------
CmdExecStatus
GNVarCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 2))
      return CMD_EXEC_ERROR;

   // check option 1
   if (!isValidVarName(options[0]))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   // check option 2
   GNum v;
   if (!GNum::getStrVal(options[1], v))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);

   GNum::setVarVal(options[0], v);
   cout << options[0] << " = " << v << endl;

   return CMD_EXEC_DONE;
}

void
GNVarCmd::usage(ostream& os) const
{
   os << "Usage: GNVARiable <(string var)> <(string var) | #(int val)>"
      << endl;
}

void
GNVarCmd::help() const
{
   cout << setw(15) << left << "GNVARiable: "
        << "perform assignment on the variable stored in the calculator"
        << endl;
}


//----------------------------------------------------------------------
//    GNADD <(string y)> <(string a) | #(int va)> <(string b) | #(int vb)>
//----------------------------------------------------------------------
CmdExecStatus
GNAddCmd::exec(const string& option)
{
   // TODO...
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 3))
      return CMD_EXEC_ERROR;

   // check option 1
   if (!isValidVarName(options[0]))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   // check option 2
   GNum v1;
   if (!GNum::getStrVal(options[1], v1))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);

   // check option 3
   GNum v2;
   if (!GNum::getStrVal(options[2], v2))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);

   GNum::setVarVal(options[0], v1+v2);

   return CMD_EXEC_DONE;
}

void
GNAddCmd::usage(ostream& os) const
{
   os << "Usage: GNADD "
      << "<(string y)> <(string a) | #(int va)> <(string b) | #(int vb)>"
      << endl;
}

void
GNAddCmd::help() const
{
   cout << setw(15) << left << "GNADD: "
        << "perform addition" << endl;
}


//----------------------------------------------------------------------
//    GNSUBtract <(string y)> <(string a) | #(int va)> <(string b) | #(int vb)>
//----------------------------------------------------------------------
CmdExecStatus
GNSubCmd::exec(const string& option)
{
   // TODO...

   return CMD_EXEC_DONE;
}

void
GNSubCmd::usage(ostream& os) const
{
   os << "Usage: GNSUBtract "
      << "<(string y)> <(string a) | #(int va)> <(string b) | #(int vb)>"
      << endl;
}

void
GNSubCmd::help() const
{
   cout << setw(15) << left << "GNSUBtract: "
        << "perform subtraction" << endl;
}


//----------------------------------------------------------------------
//    GNMULTiply <(string y)> <(string a) | #(int va)> <(string b) | #(int vb)>
//----------------------------------------------------------------------
CmdExecStatus
GNMultCmd::exec(const string& option)
{
   // TODO...
   
   return CMD_EXEC_DONE;
}

void
GNMultCmd::usage(ostream& os) const
{
   os << "Usage: GNMULTiply "
      << "<(string y)> <(string a) | #(int va)> <(string b) | #(int vb)>"
      << endl;
}

void
GNMultCmd::help() const
{
   cout << setw(15) << left << "GNMULTiply: "
        << "perform multiplication" << endl;
}


//----------------------------------------------------------------------
//    GNCOMPare <(string var1) | #(int val1)> <(string var2) | #(int val2)>
//----------------------------------------------------------------------
CmdExecStatus
GNCmpCmd::exec(const string& option)
{
   // TODO...

   return CMD_EXEC_DONE;
}

void
GNCmpCmd::usage(ostream& os) const
{
   os << "Usage: GNCOMPare "
      << "<(string var1) | #(int val1)> <(string var2) | #(int val2)>"
      << endl;
}

void
GNCmpCmd::help() const
{
   cout << setw(15) << left << "GNCOMPare: "
        << "compare if two variables or values are equal"
        << endl;
}


//----------------------------------------------------------------------
//    GNPrint [(string var)...]
//----------------------------------------------------------------------
CmdExecStatus
GNPrintCmd::exec(const string& option)
{
   vector<string> options;
   CmdExec::lexOptions(option, options);
   size_t n = options.size();
   if (n) {
      for (size_t i = 0; i < n; ++i) {
         GNum val;
         if (isValidVarName(options[i]) && GNum::getVarVal(options[i], val))
            cout << options[i] << " = " << val << endl;
         else
            CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      }
   }
   else
      GNum::printVars();
   return CMD_EXEC_DONE;
}

void
GNPrintCmd::usage(ostream& os) const
{
   os << "Usage: GNPrint [(string var)...]" << endl;
}

void
GNPrintCmd::help() const
{
   cout << setw(15) << left << "GNPrint: "
        << "print the variables stored in the calculator"
        << endl;
}


