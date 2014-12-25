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
      _data.push_back(d);

      int idx = _data.size();
      bool flag = true;

      while (flag && idx > 0) {
         if (_data[idx/2] < _data[idx]) {
            flag = false;
         } else {
            iter_swap(_data.begin() + idx, _data.begin() + idx/2);
            idx = idx/2;
         }
      }
   }

   void siftDown(int nodeIndex) {
      int leftChildIndex, rightChildIndex, minIndex;
      Data tmp;
      leftChildIndex = nodeIndex * 2;
      rightChildIndex = nodeIndex * 2 + 1;

      if (rightChildIndex >= _data.size()) {
         if (leftChildIndex >= _data.size())
            return;
         else
            minIndex = leftChildIndex;
      } else {
         if (_data[leftChildIndex] < _data[rightChildIndex])
            minIndex = leftChildIndex;
         else
            minIndex = rightChildIndex;
      }

      if (_data[minIndex] < _data[nodeIndex]) {
         tmp = _data[minIndex];
         _data[minIndex] = _data[nodeIndex];
         _data[nodeIndex] = tmp;
         siftDown(minIndex);
      }
   }

   void delMin() {
      for (typename std::vector<Data>::const_iterator i = _data.begin(); i != _data.end(); ++i) {
         std::cout << *i << std::endl;
      }

      if (_data.size() == 0)
         return;

      _data[0] = _data.back();
      _data.pop_back();

      if (_data.size() > 0) {
         siftDown(0);
      }
   }

   void delData(size_t i) {
      int idx = i;

      while (idx > 0) {
         iter_swap(_data.begin() + idx, _data.begin() + idx/2);
         idx = idx/2;
      }

      delMin();
   } // remove _data[i]

private:
   // DO NOT add or change data members
   std::vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
