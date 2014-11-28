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


      if (empty()) {
         _head = node;
         _head->_next = dummy;
         _head->_prev = dummy;

         dummy->_prev = _head;
         dummy->_next = _head;
      } else {
         node->_next = dummy;
         node->_prev = dummy->_prev;

         dummy->_prev->_next = node;
         dummy->_prev = node;

/*          std::cout << "node: " << node << std::endl; */
/*          std::cout << "node->_prev: " << node->_prev << std::endl; */
/*          std::cout << "node->_next: " << node->_next << std::endl; */
/*          std::cout << "dummy: " << dummy << std::endl; */
/*          std::cout << "dummy->_prev: " << dummy->_prev << std::endl; */
/*          std::cout << "dummy->_next: " << dummy->_next << std::endl; */
      }
   }

   void pop_front() {
      DListNode<T>* toDelete = _head;

      _head = toDelete->_next;

      _head->_prev = toDelete->_prev; // dummy
      toDelete->_prev->_next = _head;

      delete toDelete;
   }

   void pop_back() {
      DListNode<T>* toDelete = _head->_prev;

      toDelete->_prev->_next = _head;
      _head->_prev = toDelete->_prev;

      delete toDelete;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      for (iterator i = begin(); i != end(); ++i) {
         if (i._node == pos._node) {
            DListNode<T>* toDelete = i._node;

            toDelete->_next->_prev = toDelete->_prev;
            toDelete->_prev->_next = toDelete->_next;
            delete toDelete;
            return true;
         }
      }

      return false;
   }
   bool erase(const T& x) {
      DListNode<T>* node = new DListNode<T>(x);

      for (iterator i = begin(); i != end(); ++i) {
         if (i._node == node) {
            DListNode<T>* toDelete = i._node;
            toDelete->_next->_prev = toDelete->_prev;
            toDelete->_prev->_next = toDelete->_next;
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

   void sort() const { }

private:
   DListNode<T>*  _head;  // = dummy node if list is empty

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
