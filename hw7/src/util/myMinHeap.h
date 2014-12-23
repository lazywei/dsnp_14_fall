/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>
#include <algorithm>

template <class Data>
class MinHeap
{
public:
   MinHeap() : _data(0) {}
   MinHeap(size_t s) {
      _data.reserve(s);
      for (size_t i = 0; i < s; ++i) _data.push_back(Data());
      sort(_data.begin(), _data.end());
   }
   ~MinHeap() {}

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      _data.push_bask(d);

      int idx = _data.size();
      bool flag = true;

      while (flag && idx > 0) {
         if (_data[idx] > _data[idx/2]) {
            flag = false;
         } else {
            iter_swap(_data.begin() + idx, _data.begin() + idx/2);
            idx = idx/2;
         }
      }
   }

   void delMin() {
      _data[0] = _data.back();
      _data.pop_back();

      int idx = 0;

      int minVal, minIdx;

      while (idx * 2 < _data.size()) {
         if (idx * 2 + 1 < _data.size()) {
            // Has two children
            if (_data[idx] > _data[idx*2] || _data[idx*2+1]) {
               // Order break
               minVal = _data[idx];
               minIdx = idx;

               if (_data[idx*2] < minVal) {
                  minVal = _data[idx*2];
                  minIdx = idx*2;
               }

               if (_data[idx*2+1] < minVal) {
                  minVal = _data[idx*2+1];
                  minIdx = idx*2+1;
               }

               iter_swap(_data.begin() + idx, _data.begin() + minIdx);
               idx = minIdx;
            }
         } else {
            // Has one child
            if (_data[idx] > _data[idx*2]) {
               iter_swap(_data.begin() + idx, _data.begin() + idx*2);
               idx = idx*2;
            }
         }
      }
   }

   void delData(size_t i) {
      _data[i] = _data[0] - 1;

      int idx = i;
      bool flag = true;

      while (flag && idx > 0) {
         if (_data[idx] > _data[idx/2]) {
            flag = false;
         } else {
            iter_swap(_data.begin() + idx, _data.begin() + idx/2);
            idx = idx/2;
         }
      }

      delMin();
   } // remove _data[i]

private:
   // DO NOT add or change data members
   std::vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
