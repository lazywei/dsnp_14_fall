/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-2014 LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>
#include <iostream>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      // DONE
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }

      // prefix ++
      iterator& operator ++ () {
         _node = _node->_next;
         return *(this);
      }

      // postfix ++
      iterator operator ++ (int) {
         iterator result(*this);
         ++(*this);
         return result;
      }

      iterator& operator -- () {
         _node = _node->_prev;
         return *(this);
      }

      iterator operator -- (int) {
         iterator result(*this);
         --(*this);
         return result;
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return *(this);
      }

      bool operator != (const iterator& i) const {
         return _node != i._node;
      }

      bool operator == (const iterator& i) const {
         return _node == i._node;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {
      return iterator(_head);
   }

   iterator end() const {
      return iterator(_head->_prev);
   }

   bool empty() const {
      return _head->_next == _head;
   }

   size_t size() const {
      size_t cnt = 0;

      for (iterator i = begin(); i != end(); ++i) {
         ++cnt;
      }

      return cnt;
   }


   // dummy -> head -> ... -> x -> dummy
   void push_back(const T& x) {
      DListNode<T>* node = new DListNode<T>(x);
      DListNode<T>* dummy = _head->_prev;
      DListNode<T>* lastNode = dummy->_prev;

      if (empty()) {
         _head = node;
         linkFromTo(_head, dummy);
         linkFromTo(dummy, _head);
      } else {
         linkFromTo(node, dummy);
         linkFromTo(lastNode, node);

         /*
         std::cout << "node: " << node << std::endl;
         std::cout << "node->_prev: " << node->_prev << std::endl;
         std::cout << "node->_next: " << node->_next << std::endl;
         std::cout << "dummy: " << dummy << std::endl;
         std::cout << "dummy->_prev: " << dummy->_prev << std::endl;
         std::cout << "dummy->_next: " << dummy->_next << std::endl;

         swapNodes(_head, _head->_next);

         std::cout << "dummy: " << dummy << std::endl;
         std::cout << "dummy->_prev: " << dummy->_prev << std::endl;
         std::cout << "dummy->_next: " << dummy->_next << std::endl;
         */
      }
   }

   void pop_front() {
      if (empty()) { return; }

      DListNode<T>* toDelete = _head;
      DListNode<T>* next = toDelete->_next;
      DListNode<T>* dummy = _head->_prev;

      linkFromTo(dummy, next);
      _head = next;
      delete toDelete;
   }

   void pop_back() {
      if (empty()) { return; }
      DListNode<T>* dummy = _head->_prev;
      DListNode<T>* toDelete = dummy->_prev;
      DListNode<T>* prev = toDelete->_prev;

      linkFromTo(prev, dummy);
      delete toDelete;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      for (iterator i = begin(); i != end(); ++i) {
         if (i._node == pos._node) {
            DListNode<T>* toDelete = i._node;

            toDelete->_next->_prev = toDelete->_prev;
            toDelete->_prev->_next = toDelete->_next;
            if (toDelete->_next == toDelete->_prev) {
               _head = toDelete->_next;
            }
            delete toDelete;
            return true;
         }
      }

      return false;
   }

   bool erase(const T& x) {
      for (iterator i = begin(); i != end(); ++i) {
         if (i._node->_data == x) {
            DListNode<T>* toDelete = i._node;
            toDelete->_next->_prev = toDelete->_prev;
            toDelete->_prev->_next = toDelete->_next;
            if (toDelete->_next == toDelete->_prev) {
               _head = toDelete->_next;
            }
            delete toDelete;
            return true;
         }
      }

      return false;
   }

   void clear() {
      DListNode<T>* dummy = end()._node;
      for (iterator i = begin(); i != end(); ++i) {
         delete i._node;
      }
      _head = dummy;
      _head->_prev = _head;
      _head->_next = _head;
   }  // delete all nodes except for the dummy node

   void sort() const {
      bool flag = true;

      iterator iter_i;
      iterator iter_j;

      DListNode<T>* cur;
      DListNode<T>* nxt;

      iter_i = begin();
      for (size_t i = 0; i < size() && flag; ++i) {
         flag = false;
         iter_j = begin();
         for (size_t j = 0; j < size() - i - 1; ++j) {
            cur = iter_j._node;
            nxt = cur->_next;
            /* std::cout << "cur: " << cur->_data << " nxt: " << nxt->_data << std::endl; */
            if (nxt->_data < cur->_data) {
               swapData(cur, nxt);
               flag = true;
            }
            ++iter_j;
         }
         ++iter_i;
      }
   }

private:
   DListNode<T>*  _head;  // = dummy node if list is empty

   void linkFromTo(DListNode<T>* front, DListNode<T>* back) {
      front->_next = back;
      back->_prev = front;
   }

   void swapData(DListNode<T>* front, DListNode<T>* back) const {
      T tmp;
      tmp = front->_data;
      front->_data = back->_data;
      back->_data = tmp;
   }

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
