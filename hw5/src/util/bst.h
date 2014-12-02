/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* parent = 0):
      _data(d), _parent(parent), _left(NULL), _right(NULL) {}

   T               _data;
   BSTreeNode<T>*  _parent;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   BSTree(): _root(NULL), _size(0) { }
   ~BSTree() { } // clear(); delete _head; }

   class iterator {
      friend class BSTree;

   public:
      iterator(T* n= 0, BSTree<T>* tree= NULL): _node(n), _tree(tree) {}
      iterator(const iterator& i): _node(i._node), _tree(i._tree) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }

      // prefix ++
      iterator& operator ++ () {
         _node = _tree->_successor(_node);
         if (_node == NULL) {
            return NULL;
         }
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
      BSTreeNode<T>*   _node;
      const BSTree<T>* _tree;
   };
public:

   // TODO: implement these functions
   iterator begin() const { return iterator(*this); }
   iterator end() const { return iterator(*this); }

   bool empty() const { return _size == 0; }
   size_t size() const { return _size; }

   void push_back(const T& x) {
   }

   void pop_front() {
   }

   void pop_back() {
   }

   bool erase(iterator pos) {
   }

   bool erase(const T& x) {
   }

   void clear() {
   }

   // This is done. DO NOT change this one.
   void sort() const { }

private:
   BSTreeNode<T>* _root;
   size_t         _size;

   BSTreeNode<T>* _successor(BSTreeNode<T>* node) {
      BSTreeNode<T>* suc = NULL;
      BSTreeNode<T>* cur = _root;

      if (!_root) {
         return NULL;
      }

      while(cur->_data != node->_data) {
         if (node->_data < cur->_data) {
            suc = cur;
            cur = cur->_left;
         } else {
            cur = cur->_right;
         }
      }

      if (cur && cur->_right) {
         suc = _min(cur->_right);
      }

      return suc;
   }

   BSTreeNode<T>* _predecessor(BSTreeNode<T>* node) {
      BSTreeNode<T>* pre = NULL;
      BSTreeNode<T>* cur = _root;

      if (!_root) {
         return NULL;
      }

      while(cur->_data != node->_data) {
         if (node->_data < cur->_data) {
            cur = cur->_left;
         } else {
            pre = cur;
            cur = cur->_right;
         }
      }

      if (cur && cur->_left) {
         pre = _max(cur->_left);
      }

      return pre;
   }

   BSTreeNode<T>* _min(BSTreeNode<T>* node) {
      while (node->_left != NULL) {
         node = node->_left;
      }
      return node;
   }

   BSTreeNode<T>* _max(BSTreeNode<T>* node) {
      while (node->_right != NULL) {
         node = node->_right;
      }
      return node;
   }
};

#endif // BST_H
