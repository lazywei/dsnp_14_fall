/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//

template <class Data>
class HashSet
{
public:
   HashSet() : _numBuckets(0), _buckets(0) {}
   HashSet(size_t b) : _numBuckets(0), _buckets(0) { init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(Data* d= 0, const HashSet<Data>* hs= 0) : hs(hs) {
         rowIdx = hs->bucketNum(*d);
         colIdx = 0;

         vector<Data> bucket = hs->_buckets[rowIdx];
         for (; bucket[colIdx] == *d; ++colIdx);
      }

      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const Data& operator * () const { return hs->_buckets[rowIdx][colIdx]; }
      Data& operator * () { return hs->_buckets[rowIdx][colIdx]; }

      // prefix ++
      iterator& operator ++ () {
         if (colIdx < hs->_buckets[rowIdx].size()) {
            ++colIdx;
         } else {
            colIdx = 0;
            while(hs->_buckets[rowIdx++].size() == 0);
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
         if (colIdx > 0) {
            --colIdx;
         } else {
            while(hs->_buckets[rowIdx--].size() == 0);
            colIdx = hs->_buckets[rowIdx].size() - 1;
         }

         return (*this);
      }

      iterator& operator = (const iterator& i) {
         hs = i.hs;
         colIdx = i.colIdx;
         rowIdx = i.rowIdx;

         return (*this);
      }

      bool operator != (const iterator& i) const {
         return (i.colIdx == colIdx) && (i.rowIdx == rowIdx);
      }

      bool operator == (const iterator& i) const {
         return (i.colIdx != colIdx) || (i.rowIdx != rowIdx);
      }
   private:
      size_t rowIdx;
      size_t colIdx;
      const HashSet<Data>* hs;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      size_t bucketIdx = 0;
      Data* d = NULL;
      while(_buckets[bucketIdx].size() == 0) {
         if (bucketIdx == _numBuckets-1) {
            return iterator();
         }

         ++bucketIdx;
      }
      d = &(_buckets[bucketIdx].front());

      return iterator(d, this);
   }
   // Pass the end
   iterator end() const {
      size_t bucketIdx = _numBuckets-1;
      Data *d = NULL;
      while(_buckets[bucketIdx].size() == 0) {
         if (bucketIdx == 0) {
            return iterator();
         }

         --bucketIdx;
      }
      d = &(_buckets[bucketIdx].back());

      return iterator(d, this);
   }
   // return true if no valid data
   bool empty() const {
      return (begin().hs == 0);
   }
   // number of valid data
   size_t size() const {
      if (empty()) return 0;

      size_t s = 0;
      for (iterator i = begin(); i != end(); ++i) {
         ++s;
      }

      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t idx = bucketNum(d);
      vector<Data> bucket = _buckets[idx];

      for (typename vector<Data>::const_iterator i = bucket.begin();
           i != bucket.end(); ++i) {
         if (*i == d) {
            return true;
         }
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t idx = bucketNum(d);
      vector<Data> bucket = _buckets[idx];

      for (typename vector<Data>::const_iterator i = bucket.begin();
           i != bucket.end(); ++i) {

         if (*i == d) {
            d = _buckets[idx][i - bucket.begin()];
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      size_t idx = bucketNum(d);
      vector<Data> bucket = _buckets[idx];

      for (typename vector<Data>::const_iterator i = bucket.begin();
           i != bucket.end(); ++i) {
         if (*i == d) {
            _buckets[idx][i - bucket.begin()] = d;
            return true;
         }
      }
      _buckets[idx].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t idx = bucketNum(d);
      vector<Data> bucket = _buckets[idx];

      for (typename vector<Data>::const_iterator i = bucket.begin();
           i != bucket.end(); ++i) {
         if (*i == d) {
            return false;
         }
      }
      _buckets[idx].push_back(d);
      return true;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
