/****************************************************************************
  FileName     [ calcCmd.h ]
  PackageName  [ calc ]
  Synopsis     [ Define calculator command classes ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef CALC_CMD_H
#define CALC_CMD_H

#include "cmdParser.h"

CmdClass(GNSetCmd);
CmdClass(GNVarCmd);
CmdClass(GNAddCmd);
CmdClass(GNSubCmd);
CmdClass(GNMultCmd);
CmdClass(GNCmpCmd);
CmdClass(GNPrintCmd);

#endif // CALC_CMD_H
