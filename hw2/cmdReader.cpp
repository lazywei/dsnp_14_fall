/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar checkChar(char, istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      char ch = mygetc(istr);
      ParseChar pch = checkChar(ch, istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY :
                               if (moveBufPtr(_readBufPtr - 1))
                               {
                                  deleteChar();
                               }
         break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        :
                               size_t spaces_to_tab;
                               spaces_to_tab = _readBufPtr - _readBuf;
                               spaces_to_tab %= TAB_POSITION;
                               spaces_to_tab = TAB_POSITION - spaces_to_tab;
                               insertChar(' ', spaces_to_tab);
         break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY  : mybeep(); break;
         default:  // printable character
                               insertChar(char(pch));
         break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   int diff;
   string movement;

   if (greater_equal<char*>()(ptr, _readBuf)
       && less_equal<char*>()(ptr, _readBufEnd))
   {
      diff = ptrdiff_t(ptr - _readBufPtr);

      if (diff > 0)
      {
         movement = "\x1b[1C";
      }
      else if (diff < 0)
      {
         movement = "\b";
         diff = -diff;
      }

      for (int i = 0; i < diff; ++i)
      {
         cout << movement;
      }

      _readBufPtr = ptr;
   }
   else
   {
      mybeep();
      return false;
   }

   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   size_t tail_length = _readBufEnd - _readBufPtr;

   for (size_t i = 0; i < tail_length; ++i)
   {
      *(_readBufPtr + i) = *(_readBufPtr + i + 1);
   }

   // tail_length != 0 means we actually delete something
   if (tail_length != 0)
   {
      _readBufEnd = _readBufEnd - 1;

      for (size_t i = 0; i < tail_length; ++i)
      {
         cout << *(_readBufPtr + i);
      }

      cout << " ";

      for (size_t i = 0; i < tail_length; ++i)
      {
         cout << "\b";
      }
   }
   else
   {
      mybeep();
      return false;
   }

   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);

   size_t tail_length = _readBufEnd - _readBufPtr;

   _readBufEnd = _readBufEnd + repeat;

   for (size_t i = 0; i < tail_length; ++i)
   {
      *(_readBufEnd - i - 1) = *(_readBufEnd - i - repeat - 1);
   }

   for (int i = 0; i < repeat; ++i)
   {
      *(_readBufPtr + i) = ch;
   }

   for (size_t i = 0; i < repeat + tail_length; ++i)
   {
      cout << *(_readBufPtr + i);
   }

   for (size_t i = 0; i < tail_length; ++i)
   {
      cout << "\b";
   }

   _readBufPtr = _readBufPtr + repeat;
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   while(_readBufPtr > _readBuf) {
      cout << "\b";
      _readBufPtr--;
   }

   while(_readBufPtr < _readBufEnd) {
      cout << " ";
      _readBufPtr++;
   }

   while(_readBufPtr > _readBuf) {
      cout << "\b";
      _readBufPtr--;
   }

   _readBufPtr = _readBufEnd = _readBuf;
   *(_readBufPtr) = 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   bool reachEnd = false;
   if (index == _historyIdx)
   {
      return;
   }

   if (index < _historyIdx)
   {
      if (_historyIdx == 0)
      {
         mybeep();
         return;
      }

      if (_historyIdx == _history.size())
      {
         string str;
         str.assign(_readBuf, size_t(_readBufEnd - _readBuf));
         _history.push_back(str);
         _tempCmdStored = true;
      }

      if (index < 0)
      {
         index = 0;
      }
   }
   else
   {
      if (_historyIdx == _history.size())
      {
         mybeep();
         return;
      }

      if (index >= _history.size())
      {
         index = _history.size() - 1;
         reachEnd = true;
      }
   }

   _historyIdx = index;
   retrieveHistory();
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   // Clean tmp recorded string

   if (_tempCmdStored == true)
   {
      _tempCmdStored = false;
      _history.pop_back();
   }

   // Trim leading and trailing whitespaces

   char* nonEmptyStart;
   char* nonEmptyEnd;
   nonEmptyStart = nonEmptyEnd = _readBufEnd;

   for (size_t i = 0; i < size_t(_readBufEnd - _readBuf); ++i)
   {
      if (*(_readBuf + i) != ' ') {
         nonEmptyStart = _readBuf + i;
         break;
      }
   }

   for (size_t i = 0; i < size_t(_readBufEnd - nonEmptyStart); ++i)
   {
      if (*(_readBufEnd - i - 1) != ' ') {
         nonEmptyEnd = _readBufEnd - i;
         break;
      }
   }

   // Return if string is empty
   if (nonEmptyStart == nonEmptyEnd) {
      return;
   }

   string str;
   str.assign(nonEmptyStart, size_t(nonEmptyEnd - nonEmptyStart));
   _history.push_back(str);
   _historyIdx = _history.size();
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();

   if(_historyIdx == (_history.size()-1) && _tempCmdStored)
   {
      _tempCmdStored = false;
      _history.pop_back();
   }
}
