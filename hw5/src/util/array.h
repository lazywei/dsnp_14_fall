/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>
#include <iostream>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   Array() : _data(0), _size(0), _capacity(0) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }

      // prefix ++
      iterator& operator ++ () {
         _node = _node + 1;
         return (*this);
      }

      // postfix ++
      iterator operator ++ (int) {
         iterator result(*this);
         ++(*this);
         return result;
      }

      iterator& operator -- () {
         _node = _node - 1;
         return (*this);
      }

      iterator operator -- (int) {
         iterator result(*this);
         --(*this);
         return result;
      }

      iterator operator + (int i) const {
         iterator result(_node+i);
         return result;
      }

      iterator& operator += (int i) {
         _node = _node + i;
         return (*this);
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return (*this);
      }

      bool operator != (const iterator& i) const {
         return _node != i._node;
      }

      bool operator == (const iterator& i) const {
         return _node == i._node;
      }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data+_size); }

   bool empty() const { return _size == 0; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if (_size + 1 > _capacity) {
         if (_capacity == 0) {
            _capacity = 1;
         } else {
            _capacity = _capacity * 2;
         }

         T* oldData = _data;
         _data = new T[_capacity];

         for (size_t idx = 0; idx < _size; ++idx) {
            _data[idx] = oldData[idx];
         }
      }
      _data[_size] = x;
      ++_size;
   }

   void pop_front() {
      for (size_t idx = 0; idx < _size-1; ++idx) {
         _data[idx] = _data[idx+1];
      }
      --_size;
   }

   void pop_back() {
      --_size;
   }

   bool erase(iterator pos) {
      if (empty()) { return false; }

      for (iterator iter = pos; iter+1 != end(); ++iter) {
         *(iter._node) = *((iter+1)._node);
      }
      --_size;
      return true;
   }

   bool erase(const T& x) {
      if (empty()) { return false; }

      for (iterator iter = begin(); iter != end(); ++iter) {
         if (*(iter._node) == x) {
            return erase(iter);
         }
      }
      return false;
   }

   void clear() {
      _capacity = 0;
      _size = 0;
   }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*           _data;
   size_t       _size;       // number of valid elements
   size_t       _capacity;   // max number of elements

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
