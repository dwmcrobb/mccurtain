//===========================================================================
//  Copyright (c) Daniel W. McRobb 2026
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//     endorse or promote products derived from this software without
//     specific prior written permission.
//
//  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
//  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
//  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
//  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
//  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
//  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER 
//  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY 
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmMcCurtainIpv4Net2AS.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Ipv4Net2AS class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINIPV4NET2AS_HH_
#define _DWMMCCURTAINIPV4NET2AS_HH_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <list>
#include <optional>
#include <set>
#include <utility>
#include <vector>

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmMcCurtainCaidaV4Routeviews.hh"

static inline std::ostream &
operator << (std::ostream & os, const std::set<uint32_t> & ases)
{
  std::string  sep("");
  for (const auto & as : ases) {
    os << sep << as;
    sep = ',';
  }
  return os;
}

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  A Patricia trie (Practical Algorithm To Retrieve Information Coded
    //!  In Alphanumeric) for Ipv4Prefix keys, with path compression.
    //!
    //!  Path compression (also called "radix tree" compression) ensures that
    //!  internal nodes with exactly one child are eliminated.  The edge
    //!  between a parent and its descendant can therefore span multiple bits
    //!  -- those bits are encoded in the descendant's prefix rather than
    //!  requiring intermediate nodes.
    //!
    //!  Node splitting during insertion:
    //!  When a new prefix shares some initial bits with an existing node's
    //!  prefix but diverges at a later bit position (within the shorter
    //!  of the two mask lengths), a new branching node is created.  This
    //!  branching node stores the common prefix (which is shorter than
    //!  either original) and has no value of its own.
    //------------------------------------------------------------------------
    class Ipv4Net2AS
    {
      //----------------------------------------------------------------------
      //!  Internal trie node.
      //!
      //!  Each node stores a pair of (prefix, value), a flag indicating
      //!  whether a value is present, and two child pointers for the
      //!  0-bit and 1-bit branches.  The prefix stored at a node is
      //!  always at least as long as the prefix of its parent (or
      //!  strictly longer for value-holding leaf nodes).
      //!
      //!  The pair uses const Key to match std::map semantics, preventing
      //!  modification of the key through iterator access.
      //----------------------------------------------------------------------
      struct Node
      {
        std::pair<const Ipv4Prefix, std::set<uint32_t>>  _pair;
        std::array<Node *, 2>                            _children;
        Node                                            *_parent;
        bool                                             _hasValue;

        //--------------------------------------------------------------------
        //!  Construct from the given @c prefix, @c value, @c hasValue and
        //!  @c parent.
        //--------------------------------------------------------------------
        Node(const Ipv4Prefix & prefix, const std::set<uint32_t> & value,
             bool hasValue = true, Node *parent = nullptr)
            : _pair{prefix, value}, _hasValue(hasValue),
              _children{nullptr, nullptr}, _parent(parent)
        {}

        //--------------------------------------------------------------------
        //!  ostream output operator
        //--------------------------------------------------------------------
        friend std::ostream & operator << (std::ostream & os,
                                           const Node & node)
        {
          if (node._hasValue) {
            os << node._pair.first << ' ';
            std::string  sep = "";
            for (const auto & as : node._pair.second) {
              os << sep << as;
              sep = ",";
            }
            os << '\n';
          }
          if (node._children[0]) {
            os << *(node._children[0]);
          }
          if (node._children[1]) {
            os << *(node._children[1]);
          }
          return os;
        }
      };

    public:
      //----------------------------------------------------------------------
      //!  Type aliases matching std::map convention.
      //----------------------------------------------------------------------
      using key_type        = Ipv4Prefix;
      using mapped_type     = std::set<uint32_t>;
      using value_type      = std::pair<const Ipv4Prefix, mapped_type>;
      using size_type       = std::size_t;
      using difference_type = std::ptrdiff_t;

      //------------------------------------------------------------------------
      //!  Forward declarations for iterator types.
      //------------------------------------------------------------------------
      class iterator;
      class const_iterator;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      //----------------------------------------------------------------------
      //!  Default constructor.  Creates an empty trie.
      //----------------------------------------------------------------------
      Ipv4Net2AS()
          : _root(nullptr), _size(0)
      {}

      Ipv4Net2AS(const CaidaV4Routeviews & rv);
      
      //----------------------------------------------------------------------
      //!  Destructor.  Deletes all nodes.
      //----------------------------------------------------------------------
      ~Ipv4Net2AS()
      { clear(_root); }

      //----------------------------------------------------------------------
      //!  Copy constructor.
      //----------------------------------------------------------------------
      Ipv4Net2AS(const Ipv4Net2AS & other)
          : _root(copyNode(other._root)), _size(other._size)
      {}

      //----------------------------------------------------------------------
      //!  Copy assignment operator.
      //----------------------------------------------------------------------
      Ipv4Net2AS & operator = (const Ipv4Net2AS & other)
      {
        if (this != &other) {
          clear(_root);
          _root = copyNode(other._root);
          _size = other._size;
        }
        return *this;
      }

      //----------------------------------------------------------------------
      //!  Move constructor.
      //----------------------------------------------------------------------
      Ipv4Net2AS(Ipv4Net2AS && other) noexcept
          : _root(other._root), _size(other._size)
      {
        other._root = nullptr;
        other._size = 0;
      }

      //----------------------------------------------------------------------
      //!  Move assignment operator.
      //----------------------------------------------------------------------
      Ipv4Net2AS & operator = (Ipv4Net2AS && other) noexcept
      {
        if (this != &other) {
          clear(_root);
          _root       = other._root;
          _size       = other._size;
          other._root = nullptr;
          other._size = 0;
        }
        return *this;
      }

      //----------------------------------------------------------------------
      //!  Insert a new element into the trie.  This is modeled after standard
      //!  associative container insertion.
      //!
      //!  If the prefix already exists in the trie, no insertion takes place
      //!  and the node remains unmodified.
      //!
      //!  @param value  The pair of prefix and value to insert.
      //!  @return  A pair containing an iterator to the element and a boolean
      //!           indicating whether an insertion took place.
      //----------------------------------------------------------------------
      std::pair<iterator, bool> insert(const value_type & value)
      {
        Node  *result = nullptr;
        bool   inserted = false;
        _root = addNode(_root, value.first, value.second, result, inserted,
                        false, nullptr);
        return { iterator(_root, result), inserted };
      }

      //----------------------------------------------------------------------
      //!  Access the value associated with @c key.  If the key does not exist,
      //!  it is inserted with a default-constructed value.
      //!
      //!  @param key  The Ipv4Prefix to look up.
      //!  @return  A reference to the value associated with the key.
      //----------------------------------------------------------------------
      std::set<uint32_t> & operator [] (const Ipv4Prefix & key)
      {
        Node  *result = nullptr;
        bool   inserted = false;
        _root = addNode(_root, key, mapped_type{}, result, inserted, false,
                        nullptr);
        return result->_pair.second;
      }

      //----------------------------------------------------------------------
      //!  Returns true if the trie is empty.
      //----------------------------------------------------------------------
      bool empty() const
      { return (0 == _size); }
    
      
      //----------------------------------------------------------------------
      //!  Clears all nodes from the trie.
      //----------------------------------------------------------------------
      void clear()
      {
        clear(_root);
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool CombinableAdjacents(const_iterator it1, const_iterator it2)
      {
        if (! it1->first.Bit(it1->first.MaskLength() - 1)) {
          Ipv4Prefix  nextpfx(it1->first);  ++nextpfx;
          return ((nextpfx == it2->first)
                  && (it1->second == it2->second));
        }
        return false;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void CombineAdjacents()
      {
        if (_size > 1) {
          bool  combining = true;
          while (combining) {
            auto it = begin();
            auto nit = it; ++nit;
            combining = false;
            while (nit != end()) {
#if 0
              std::cerr << "it: " << it->first << ' ' << it->second
                        << " nit: " << nit->first << ' ' << nit->second
                        << '\n';
#endif
              if (CombinableAdjacents(it, nit)) {
                Ipv4Prefix  aggpfx(it->first.Network(),
                                   it->first.MaskLength() - 1);
                auto [insit, inserted] = insert({aggpfx, it->second});
                assert(insit->first == aggpfx);
                if (! inserted) {
                  for (const auto & as : it->second) {
                    insit->second.insert(as);
                  }
                }
#if 0
                std::cerr << "aggregated " << it->first << ' ' << it->second
                          << " and " << nit->first << ' ' << nit->second
                          << " into " << insit->first << ' ' << insit->second
                          << '\n';
#endif
                assert(std::ranges::includes(insit->second, it->second));
                erase(it);
                it = insit;
                erase(nit);
                nit = it; ++nit;
                combining = true;
              }
              else {
                ++nit;
                ++it;
              }
            }
          }
        }
        return;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const_iterator find_combinable_adjacent(const_iterator it) 
      {
        if (! it->first.Bit(it->first.MaskLength() - 1)) {
          Ipv4Prefix  pfx(it->first); ++pfx;
          mapped_type  ases{it->second};
          for (++it; it != cend(); ++it) {
            if (it->first > pfx) {
              break;
            }
            else if ((it->first == pfx) && (it->second == ases)) {
              return it;
            }
          }
        }
        return cend();
      }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void CombineAdjacents2()
      {
        if (_size > 1) {
          bool  combining = true;
          while (combining) {
            auto it = cbegin();
            combining = false;
            while (it != cend()) {
              auto  nxtit = find_combinable_adjacent(it);
              if (nxtit != cend()) {
                erase(nxtit);
                value_type  agg{Ipv4Prefix(it->first.Network(),
                                           it->first.MaskLength() - 1),
                  it->second};
                erase(it);
                auto [iit, inserted] = insert(agg);
                if (! inserted) {
                  iit->second.insert_range(agg.second);
                }
                it = iit;
                combining = true;
              }
              else {
                ++it;
              }
            }
          }
        }
      }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void Aggregate()
      {
        iterator  it = begin();
        while (it != end()) {
          auto  wit = wider_covering(it);
          if (wit != end()) {
            it = erase(it);
          }
          else {
            ++it;
          }
        }
        CombineAdjacents2();
        return;
      }
          
      //----------------------------------------------------------------------
      //!  Remove a prefix from the trie.
      //!
      //!  After removal, nodes that no longer carry a value and have no
      //!  children are deleted (compaction).  Internal nodes that become
      //!  single-child nodes with no value are also bypassed, maintaining
      //!  the path-compressed structure.
      //!
      //!  @param prefix  The Ipv4Prefix to remove.
      //!  @return  true if the prefix was found and removed, false otherwise.
      //----------------------------------------------------------------------
      bool Remove(const Ipv4Prefix & prefix)
      {
        bool  removed = false;
        _root = removeNode(_root, prefix, removed);
        if (_root) {
          _root->_parent = nullptr;
        }
        return removed;
      }

      //----------------------------------------------------------------------
      //!  Erases the node at @c it and returns an iterator referencing the
      //!  next node in the trie.
      //----------------------------------------------------------------------
      iterator erase(iterator it)
      {
        iterator  rc = it;
        if (rc != end()) {
          ++rc;
          Remove(it._current->_pair.first);
        }
        return rc;
      }
    
      //----------------------------------------------------------------------
      //!  Erases the node at @c it and returns an iterator referencing the
      //!  next node in the trie.
      //----------------------------------------------------------------------
      iterator erase(const_iterator it)
      {
        iterator  rc((Node *)it._root, (Node *)it._current);
        if (rc != end()) {
          ++rc;
          Remove(it._current->_pair.first);
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Erases the node for prefix @c pfx in the trie and returns 1 if a
      //!  node was removed, 0 if @c pfx was not found in the trie.
      //----------------------------------------------------------------------
      size_type erase(const Ipv4Prefix & pfx)
      {
        return (Remove(pfx) ? 1 : 0);
      }
    
      //----------------------------------------------------------------------
      //!  Find the node whose key exactly matches @c key.
      //!  Returns an iterator to the matching node, or end() if not found.
      //----------------------------------------------------------------------
      iterator find(const Ipv4Prefix & key)
      {
        Node  *node = _root;

        while (node) {
          if (node->_pair.first == key) {
            if (node->_hasValue) {
              return iterator(_root, node);
            }
            // Exact prefix match but no value: not a stored entry
            break;
          }
          if (node->_pair.first.Contains(key)) {
            uint8_t  b = key.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return end();
      }

      //----------------------------------------------------------------------
      //!  Find the node whose prefix exactly matches @c pfx.  Returns a
      //!  const_iterator to the matching node, or end() if not found.
      //----------------------------------------------------------------------
      const_iterator find(const Ipv4Prefix & pfx) const
      {
        const Node  *node = _root;

        while (node) {
          if (node->_pair.first == pfx) {
            if (node->_hasValue) {
              return const_iterator(_root, node);
            }
            // Exact prefix match but no value: not a stored entry
            break;
          }
          if (node->_pair.first.Contains(pfx)) {
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return cend();
      }

      //----------------------------------------------------------------------
      //!  Finds the node with the longest match to the given prefix @c pfx.
      //!  Returns end() if no match is found.
      //----------------------------------------------------------------------
      iterator find_longest(const Ipv4Prefix & pfx)
      {
        auto      *node = _root;
        iterator   it = end();
        while (node) {
          if (node->_pair.first.Contains(pfx)) {
            if (node->_hasValue) {
              it = iterator(_root, node);
            }
            if (node->_pair.first.MaskLength() >= pfx.MaskLength()) {
              break;
            }
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return it;
      }

      //----------------------------------------------------------------------
      //!  Finds the node with the longest match to the given address @c addr.
      //!  Returns end() if no match is found.
      //----------------------------------------------------------------------
      iterator find_longest(const Ipv4Address & addr)
      { return find_longest(Ipv4Prefix(addr, 32)); }
      
      //----------------------------------------------------------------------
      //!  Finds the node with the longest match to the given prefix @c pfx.
      //!  Returns end() if no match is found.
      //----------------------------------------------------------------------
      const_iterator find_longest(const Ipv4Prefix & pfx) const
      {
        auto            *node = _root;
        const_iterator   it = cend();
        while (node) {
          if (node->_pair.first.Contains(pfx)) {
            if (node->_hasValue) {
              it = const_iterator(_root, node);
            }
            if (node->_pair.first.MaskLength() >= pfx.MaskLength()) {
              break;
            }
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return it;
      }

      //----------------------------------------------------------------------
      //!  Finds the node with the longest match to the given address @c addr.
      //!  Returns end() if no match is found.
      //----------------------------------------------------------------------
      const_iterator find_longest(const Ipv4Address & addr) const
      { return find_longest(Ipv4Prefix(addr, 32)); }
    
      //----------------------------------------------------------------------
      //!  Finds all nodes that match prefix @c pfx, placing them in @c
      //!  matches.  Returns true if matches were found, else returns false.
      //----------------------------------------------------------------------
      bool find_matches(const Ipv4Prefix & pfx,
                        std::vector<value_type> & matches) const
      {
        matches.clear();
        auto      *node = _root;
        while (node) {
          if (node->_pair.first.Contains(pfx)) {
            if (node->_hasValue) {
              matches.push_back(node->_pair);
            }
            if (node->_pair.first.MaskLength() >= pfx.MaskLength()) {
              break;
            }
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return (! matches.empty());
      }

      //----------------------------------------------------------------------
      //!  Finds nodes that contain prefix @c pfx and have a wider netmask
      //!  than @c pfx, placing them in @c matches.  Returns true if matches
      //!  were found, else returns false.
      //----------------------------------------------------------------------
      bool find_wider(const Ipv4Prefix & pfx,
                      std::vector<value_type> & matches) const
      {
        matches.clear();
        auto      *node = _root;
        while (node) {
          if (node->_pair.first.Contains(pfx)) {
            if (node->_pair.first.MaskLength() < pfx.MaskLength()) {
              if (node->_hasValue) {
                matches.push_back(node->_pair);
              }
            }
            else {
              break;
            }
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return (! matches.empty());
      }
    
      //----------------------------------------------------------------------
      //!  Finds all nodes that match address @c addr, placing them in
      //!  @c matches.  Returns true if matches were found, else returns
      //!  false.
      //----------------------------------------------------------------------
      bool find_matches(const Ipv4Address & addr,
                        std::vector<value_type> & matches) const
      { return find_matches(Ipv4Prefix(addr, 32), matches); }
    
      //----------------------------------------------------------------------
      //!  Returns the total number of nodes in the trie.
      //----------------------------------------------------------------------
      size_t size() const
      { return _size; }

      //----------------------------------------------------------------------
      //!  Return an iterator to the first value-holding node.
      //!  Traversal visits nodes in pre-order (current, left, right),
      //!  producing prefixes sorted by network address.
      //----------------------------------------------------------------------
      iterator begin()
      { return iterator(_root); }

      //----------------------------------------------------------------------
      //!  Return the past-the-end iterator.
      //----------------------------------------------------------------------
      iterator end()
      { return iterator(_root, nullptr); }

      //----------------------------------------------------------------------
      //!  Return a const_iterator to the first value-holding node.
      //----------------------------------------------------------------------
      const_iterator begin() const
      { return const_iterator(_root); }

      //----------------------------------------------------------------------
      //!  Return the past-the-end const_iterator.
      //----------------------------------------------------------------------
      const_iterator end() const
      { return const_iterator(_root, nullptr); }

      //----------------------------------------------------------------------
      //!  Returns a const_iterator for the first value-holding node.
      //----------------------------------------------------------------------
      const_iterator cbegin() const { return begin(); }

      //----------------------------------------------------------------------
      //!  Return the past-the-end const_iterator.
      //----------------------------------------------------------------------
      const_iterator cend() const   { return end(); }

      //----------------------------------------------------------------------
      //!  Returns a reverse_iterator to the first value-holding element of
      //!  the reversed trie.
      //----------------------------------------------------------------------
      reverse_iterator rbegin() { return reverse_iterator(end()); }

      //----------------------------------------------------------------------
      //!  Returns a reverse_iterator to the element following the last
      //!  value-holding element of the reversed trie.
      //----------------------------------------------------------------------
      reverse_iterator rend() { return reverse_iterator(begin()); }

      //----------------------------------------------------------------------
      //!  Returns a const_reverse_iterator to the first value-holding element
      //!  of the reversed trie.
      //----------------------------------------------------------------------
      const_reverse_iterator rbegin() const
      { return const_reverse_iterator(end()); }
    
      //----------------------------------------------------------------------
      //!  Returns a const_reverse_iterator to the element following the last
      //!  value-holding element of the reversed trie.
      //----------------------------------------------------------------------
      const_reverse_iterator rend() const
      { return const_reverse_iterator(begin()); }
    
      //----------------------------------------------------------------------
      //!  Returns a const_reverse_iterator to the first value-holding element
      //!  of the reversed trie.
      //----------------------------------------------------------------------
      const_reverse_iterator crbegin() const
      { return const_reverse_iterator(cend()); }
    
      //----------------------------------------------------------------------
      //!  Returns a const_reverse_iterator to the element following the last
      //!  value-holding element of the reversed trie.
      //----------------------------------------------------------------------
      const_reverse_iterator crend() const
      { return const_reverse_iterator(cbegin()); }

      //----------------------------------------------------------------------
      //!  ostream output operator
      //----------------------------------------------------------------------
      friend std::ostream &
      operator << (std::ostream & os, const Ipv4Net2AS & n2as)
      {
        if (n2as._root) {
          os << *(n2as._root);
        }
        return os;
      }

      //----------------------------------------------------------------------
      //!  Writes the trie to the given ostream @c os.  Returns @c os.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const
      {
        EncodedU64  numEntries = _size;
        if (numEntries.Write(os)) {
          if (numEntries) {
            for (const_iterator it = cbegin(); it != cend(); ++it) {
              if (! StreamIO::Write(os, *it)) {
                break;
              }
            }
          }
        }
        return os;
      }
    
      //----------------------------------------------------------------------
      //!  Reads the trie from the given istream @c is.  Returns @c is.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is)
      {
        clear();
        EncodedU64  numEntries;
        if (numEntries.Read(is)) {
          std::pair<key_type, mapped_type>  entry;
          for (uint64_t i = 0; i < numEntries; ++i) {
            if (StreamIO::Read(is, entry)) {
              insert(entry);
            }
            else {
              break;
            }
          }
        }
        return is;
      }

      //----------------------------------------------------------------------
      //!  Writes the trie to the given descriptor @c fd.  Returns the number
      //!  of bytes written on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t Write(int fd) const
      {
        ssize_t  rc = -1;
        EncodedU64  numEntries = _size;
        ssize_t  bytesWritten = numEntries.Write(fd);
        if (bytesWritten > 0) {
          rc = bytesWritten;
          if (numEntries) {
            for (const_iterator it = cbegin(); it != cend(); ++it) {
              bytesWritten = DescriptorIO::Write(fd, *it);
              if (bytesWritten > 0) {
                rc += bytesWritten;
              }
              else {
                rc = -1;
                break;
              }
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Reads the trie from the given descriptor @c fd.  Returns the number
      //!  of bytes read on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t Read(int fd)
      {
        ssize_t  rc = -1;
        clear();
        EncodedU64  numEntries;
        ssize_t  bytesRead = numEntries.Read(fd);
        if (bytesRead > 0) {
          rc = bytesRead;
          std::pair<key_type, mapped_type>  entry;
          for (uint64_t i = 0; i < numEntries; ++i) {
            bytesRead = DescriptorIO::Read(fd, entry);
            if (bytesRead > 0) {
              insert(entry);
              rc += bytesRead;
            }
            else {
              rc = -1;
              break;
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Writes the trie to the given FILE @c f.  Returns 1 on success, 0 on
      //!  failure.
      //----------------------------------------------------------------------
      size_t Write(FILE *f) const
      {
        size_t  rc = 0;
        if (f) {
          EncodedU64  numEntries = _size;
          if (numEntries.Write(f)) {
            rc = 1;
            if (numEntries) {
              for (const_iterator it = cbegin(); it != cend(); ++it) {
                if (! FileIO::Write(f, *it)) {
                  rc = 0;
                  break;
                }
              }
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Reads the trie from the given FILE @c f.  Returns 1 on success,
      //!  0 on failure.
      //----------------------------------------------------------------------
      size_t Read(FILE *f)
      {
        size_t  rc = 0;
        clear();
        if (f) {
          EncodedU64  numEntries;
          if (numEntries.Read(f)) {
            rc = 1;
            if (numEntries) {
              std::pair<key_type, mapped_type>  entry;
              for (size_t i = 0; i < numEntries; ++i) {
                if (FileIO::Read(f, entry)) {
                  insert(entry);
                }
                else {
                  rc = 0;
                  break;
                }
              }
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Writes the trie to the given gzFiule @c gzf.  Returns the number of
      //!  bytes written on success, -1 on failure.
      //----------------------------------------------------------------------
      int Write(gzFile gzf) const
      {
        int  rc = -1;
        if (gzf) {
          EncodedU64  numEntries = _size;
          int  bytesWritten = numEntries.Write(gzf);
          if (bytesWritten > 0) {
            rc = bytesWritten;
            if (_size) {
              for (const_iterator it = cbegin(); it != cend(); ++it) {
                bytesWritten = GZIO::Write(gzf, *it);
                if (bytesWritten > 0) {
                  rc += bytesWritten;
                }
                else {
                  rc = -1;
                  break;
                }
              }
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Reads the trie from the given gzFile @c gzf.  Returns the number of
      //!  bytes read on success, -1 on failure.
      //----------------------------------------------------------------------
      int Read(gzFile gzf)
      {
        int  rc = -1;
        clear();
        EncodedU64  numEntries;
        ssize_t  bytesRead = numEntries.Read(gzf);
        if (bytesRead > 0) {
          rc = bytesRead;
          std::pair<key_type, mapped_type>  entry;
          for (uint64_t i = 0; i < numEntries; ++i) {
            bytesRead = GZIO::Read(gzf, entry);
            if (bytesRead > 0) {
              insert(entry);
              rc += bytesRead;
            }
            else {
              rc = -1;
              break;
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Writes the trie to the given BZFILE @c bzf.  Returns the number of
      //!  bytes written on success, -1 on failure.
      //----------------------------------------------------------------------
      int BZWrite(BZFILE *bzf) const
      {
        int  rc = -1;
        if (bzf) {
          EncodedU64  numEntries = _size;
          int  bytesWritten = numEntries.BZWrite(bzf);
          if (bytesWritten > 0) {
            rc = bytesWritten;
            for (const_iterator it = cbegin(); it != cend(); ++it) {
              bytesWritten = BZ2IO::BZWrite(bzf, *it);
              if (bytesWritten > 0) {
                rc += bytesWritten;
              }
              else {
                rc = -1;
                break;
              }
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Reads the trie from the given BZFILE @c bzf.  Returns the number of
      //!  bytes read on success, -1 on failure.
      //----------------------------------------------------------------------
      int BZRead(BZFILE *bzf)
      {
        int  rc = -1;
        clear();
        if (bzf) {
          EncodedU64  numEntries;
          ssize_t  bytesRead = numEntries.BZRead(bzf);
          if (bytesRead > 0) {
            rc = bytesRead;
            std::pair<key_type, mapped_type>  entry;
            for (uint64_t i = 0; i < numEntries; ++i) {
              bytesRead = BZ2IO::BZRead(bzf, entry);
              if (bytesRead > 0) {
                insert(entry);
                rc += bytesRead;
              }
              else {
                rc = -1;
                break;
              }
            }
          }
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Loads the contents from a gzip'ed routeviews file from CAIDA.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool LoadCAIDARouteViews(const std::string & path);
      
      //----------------------------------------------------------------------
      //!  Loads the contents from the native binary file located at @c path.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Load(const std::string & path);
      
      //----------------------------------------------------------------------
      //!  Saves the contents in native binary form to the file at @c path.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Save(const std::string & path) const;
      
      //----------------------------------------------------------------------
      //!  Forward iterator over value-holding nodes in pre-order traversal
      //!  (current, left, right), which produces sorted-by-address output
      //!  for a Patricia trie.  Models std::forward_iterator.  Dereferences
      //!  to a reference to std::pair<const Ipv4Prefix, std::set<uint32_t>>,
      //!  matching  std::map::iterator semantics.
      //----------------------------------------------------------------------
      class iterator
      {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::pair<const Ipv4Prefix, std::set<uint32_t>>;
        using difference_type   = std::ptrdiff_t;
        using reference         = value_type &;
        using pointer           = value_type *;

        iterator() = default;

        //------------------------------------------------------------------
        reference operator * () const
        { return _current->_pair; }

        //------------------------------------------------------------------
        pointer operator -> () const
        { return &(_current->_pair); }

        //------------------------------------------------------------------
        iterator & operator ++ ()
        {
          _current = _findSuccessor(_current);
          return *this;
        }

        //------------------------------------------------------------------
        iterator operator ++ (int)
        {
          iterator tmp = *this;
          ++(*this);
          return tmp;
        }

        //------------------------------------------------------------------
        iterator & operator -- ()
        {
          if (_current == nullptr) {
            _current = _findLastValueNode(_root);
          } else {
            _current = _findPredecessor(_current);
          }
          return *this;
        }

        //------------------------------------------------------------------
        iterator operator -- (int)
        {
          iterator tmp = *this;
          --(*this);
          return tmp;
        }

        //------------------------------------------------------------------
        bool operator == (const iterator & other) const
        { return _current == other._current; }

        //------------------------------------------------------------------
        bool operator != (const iterator & other) const
        { return !(*this == other); }

      private:
        Node               *_current   = nullptr;
        Node               *_root      = nullptr;

        friend class Ipv4Net2AS;
        friend class const_iterator;

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        explicit iterator(Node *root)
            : _root(root)
        {
          if (root) {
            _current = _findFirstValueNode(root);
          }
        }

        //--------------------------------------------------------------------
        //! Construct an iterator pointing directly to @c currentNode.
        //! Used internally by find().
        //--------------------------------------------------------------------
        iterator(Node *root, Node *currentNode)
            : _current(currentNode), _root(root)
        {}

        //--------------------------------------------------------------------
        //! Find the first value-holding node in the subtree rooted at @c n.
        //--------------------------------------------------------------------
        Node *_findFirstValueNode(Node *n)
        {
          if (! n) {
            return nullptr;
          }
          if (n->_hasValue) {
            return n;
          }
          if (n->_children[0]) {
            Node  *res = _findFirstValueNode(n->_children[0]);
            if (res) {
              return res;
            }
          }
          if (n->_children[1]) {
            Node  *res = _findFirstValueNode(n->_children[1]);
            if (res) {
              return res;
            }
          }
          return nullptr;
        }

        //--------------------------------------------------------------------
        //! Find the pre-order successor of the given node.
        //--------------------------------------------------------------------
        Node *_findSuccessor(Node *n)
        {
          if (! n) {
            return nullptr;
          }
          if (n->_children[0]) {
            Node  *res = _findFirstValueNode(n->_children[0]);
            if (res) {
              return res;
            }
          }
          if (n->_children[1]) {
            Node  *res = _findFirstValueNode(n->_children[1]);
            if (res) {
              return res;
            }
          }
          Node  *curr = n;
          while (curr->_parent) {
            Node  *p = curr->_parent;
            if (p->_children[0] == curr) {
              if (p->_children[1]) {
                Node  *res = _findFirstValueNode(p->_children[1]);
                if (res) {
                  return res;
                }
              }
            }
            curr = p;
          }
          return nullptr;
        }

        //--------------------------------------------------------------------
        //! Find the pre-order predecessor of the given node.
        //--------------------------------------------------------------------
        Node *_findPredecessor(Node *n)
        {
          if (! n) {
            return nullptr;
          }
          Node  *p = n->_parent;
          if (! p) {
            return nullptr;
          }

          if (p->_children[1] == n) {
            Node  *left = p->_children[0];
            if (left) {
              return _findLastValueNode(left);
            }
            if (p->_hasValue) return p;
            return _findPredecessor(p);
          }
          else {
            if (p->_hasValue) return p;
            return _findPredecessor(p);
          }
        }

        //--------------------------------------------------------------------
        //! Find the rightmost value-holding node in the subtree rooted at
        //!  @c n.
        //--------------------------------------------------------------------
        Node *_findLastValueNode(Node *n)
        {
          if (! n) {
            return nullptr;
          }
          Node  *last = _findLastValueNode(n->_children[1]);
          if (last) {
            return last;
          }
          last = _findLastValueNode(n->_children[0]);
          if (last) {
            return last;
          }
          if (n->_hasValue) {
            return n;
          }
          return nullptr;
        }

      };

      //----------------------------------------------------------------------
      //!  Forward const_iterator over value-holding nodes in pre-order
      //!  traversal (current, left, right), which produces sorted-by-address
      //!  output for a Patricia trie.  Models std::forward_iterator.
      //!  Dereferences to a reference to
      //!  std::pair<const Ipv4Prefix, std::set<uint32_t>>, matching
      //!  std::map::const_iterator semantics.
      //----------------------------------------------------------------------
      class const_iterator
      {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::pair<const Ipv4Prefix, std::set<uint32_t>>;
        using difference_type   = std::ptrdiff_t;
        using reference         = const value_type &;
        using pointer           = const value_type *;

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        const_iterator() = default;

        //--------------------------------------------------------------------
        //! Allow construction from a mutable iterator.
        //--------------------------------------------------------------------
        const_iterator(const iterator & it)
            : _current(it._current), _root(it._root)
        {}

        //--------------------------------------------------------------------
        reference operator * () const
        { return _current->_pair; }

        //--------------------------------------------------------------------
        pointer operator -> () const
        { return &(_current->_pair); }

        //--------------------------------------------------------------------
        const_iterator & operator ++ ()
        {
          _current = _findSuccessor(_current);
          return *this;
        }

        //--------------------------------------------------------------------
        const_iterator operator ++ (int)
        {
          const_iterator tmp = *this;
          ++(*this);
          return tmp;
        }

        //--------------------------------------------------------------------
        const_iterator & operator -- ()
        {
          if (_current == nullptr) {
            _current = _findLastValueNode(_root);
          } else {
            _current = _findPredecessor(_current);
          }
          return *this;
        }

        //--------------------------------------------------------------------
        const_iterator operator -- (int)
        {
          const_iterator tmp = *this;
          --(*this);
          return tmp;
        }

        //--------------------------------------------------------------------
        bool operator == (const const_iterator & other) const
        { return _current == other._current; }

        //--------------------------------------------------------------------
        bool operator != (const const_iterator & other) const
        { return !(*this == other); }

      private:
        const Node                *_current = nullptr;
        const Node                *_root    = nullptr;

        friend class Ipv4Net2AS;

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        explicit const_iterator(const Node *root)
            : _root(root)
        {
          if (root) {
            _current = _findFirstValueNode(root);
          }
        }

        //--------------------------------------------------------------------
        //! Construct a const_iterator pointing directly to @c currentNode.
        //! Used internally by find().
        //--------------------------------------------------------------------
        const_iterator(const Node *root, const Node *currentNode)
            : _current(currentNode), _root(root)
        {}

        //--------------------------------------------------------------------
        //! Find the first value-holding node in the subtree rooted at @c n.
        //--------------------------------------------------------------------
        const Node *_findFirstValueNode(const Node *n)
        {
          if (! n) {
            return nullptr;
          }
          if (n->_hasValue) {
            return n;
          }
          if (n->_children[0]) {
            const Node  *res = _findFirstValueNode(n->_children[0]);
            if (res) {
              return res;
            }
          }
          if (n->_children[1]) {
            const Node *res = _findFirstValueNode(n->_children[1]);
            if (res) {
              return res;
            }
          }
          return nullptr;
        }

        //--------------------------------------------------------------------
        //! Find the pre-order successor of the given node.
        //--------------------------------------------------------------------
        const Node *_findSuccessor(const Node *n)
        {
          if (! n) {
            return nullptr;
          }
          if (n->_children[0]) {
            const Node  *res = _findFirstValueNode(n->_children[0]);
            if (res) {
              return res;
            }
          }
          if (n->_children[1]) {
            const Node *res = _findFirstValueNode(n->_children[1]);
            if (res) {
              return res;
            }
          }
          const Node  *curr = n;
          while (curr->_parent) {
            const Node  *p = curr->_parent;
            if (p->_children[0] == curr) {
              if (p->_children[1]) {
                const Node  *res = _findFirstValueNode(p->_children[1]);
                if (res) {
                  return res;
                }
              }
            }
            curr = p;
          }
          return nullptr;
        }

        //--------------------------------------------------------------------
        //! Find the pre-order predecessor of the given node.
        //--------------------------------------------------------------------
        const Node *_findPredecessor(const Node *n)
        {
          if (! n) {
            return nullptr;
          }
          const Node * p = n->_parent;
          if (! p) {
            return nullptr;
          }

          if (p->_children[1] == n) {
            const Node  *left = p->_children[0];
            if (left) {
              return _findLastValueNode(left);
            }
            if (p->_hasValue) {
              return p;
            }
            return _findPredecessor(p);
          }
          else {
            if (p->_hasValue) {
              return p;
            }
            return _findPredecessor(p);
          }
        }

        //--------------------------------------------------------------------
        //!  Find the rightmost value-holding node in the subtree rooted at
        //!  @c n.
        //--------------------------------------------------------------------
        const Node *_findLastValueNode(const Node *n)
        {
          if (! n) {
            return nullptr;
          }
          const Node  *last = _findLastValueNode(n->_children[1]);
          if (last) {
            return last;
          }
          last = _findLastValueNode(n->_children[0]);
          if (last) {
            return last;
          }
          if (n->_hasValue) {
            return n;
          }
          return nullptr;
        }

      };

    private:
      Node    *_root;
      size_t   _size;

      //----------------------------------------------------------------------
      //!  Recursively delete all nodes under @c n and @c n itself.
      //----------------------------------------------------------------------
      void clear(Node * & n)
      {
        if (n) {
          clear(n->_children[0]);
          clear(n->_children[1]);
          if (n->_hasValue) {
            --_size;
          }
          delete n;
          n = nullptr;
        }
      }

      //----------------------------------------------------------------------
      //!  Recursively deep-copy a subtree.
      //----------------------------------------------------------------------
      Node *copyNode(Node *n)
      {
        if (! n) {
          return nullptr;
        }
        Node  *c = new Node(n->_pair.first, n->_pair.second, n->_hasValue);
        if (n->_hasValue) {
          ++_size;
        }
        c->_children[0] = copyNode(n->_children[0]);
        c->_children[1] = copyNode(n->_children[1]);
        return c;
      }

      //----------------------------------------------------------------------
      //!  Recursive insertion helper.
      //!
      //!  Cases handled:
      //!    1. Empty subtree -> create a new leaf.
      //!    2. Exact prefix match -> update the value (if updateExisting is
      //!       true).
      //!    3. New prefix is longer (extends an existing prefix) ->
      //!       recurse into the appropriate child.
      //!    4. New prefix is shorter (existing node extends beyond it) ->
      //!       create a new node above the existing one (path compression).
      //!    5. Prefixes diverge at some bit position within both mask
      //!       lengths -> split: create a branching node that holds the
      //!       common prefix, and place both the existing node and the
      //!       new node as its children (node splitting).
      //!----------------------------------------------------------------------
      Node *addNode(Node *node, const Ipv4Prefix & prefix,
                    const std::set<uint32_t> & value,
                    Node * & resultNode, bool & inserted, bool updateExisting,
                    Node *parent = nullptr)
      {
        if (! node) {
          inserted = true;
          Node * newNode = new Node(prefix, value, true, parent);
          resultNode = newNode;
          ++_size;
          return newNode;
        }

        int diffBit = firstDiffBit(node->_pair.first, prefix,
                                   std::min(node->_pair.first.MaskLength(),
                                            prefix.MaskLength()));

        if (diffBit < 0) {
          if (node->_pair.first == prefix) {
            if (node->_hasValue) {
              inserted = false;
              resultNode = node;
              if (updateExisting) {
                node->_pair.second = value;
              }
              return node;
            }
            else {
              // Node exists but has no value (branching node)
              inserted = true;
              node->_pair.second = value;
              node->_hasValue = true;
              resultNode = node;
              ++_size;
              return node;
            }
          }

          if (prefix.MaskLength() > node->_pair.first.MaskLength()) {
            uint8_t bit = prefix.Bit(node->_pair.first.MaskLength());
            node->_children[bit] = addNode(node->_children[bit], prefix, value,
                                           resultNode, inserted, updateExisting,
                                           node);
            return node;
          }
          else {
            inserted = true;
            Node     *parentNode = new Node(prefix, value, true, parent);
            resultNode = parentNode;
            uint8_t   bit   = node->_pair.first.Bit(prefix.MaskLength());
            parentNode->_children[bit] = node;
            node->_parent = parentNode;
            ++_size;
            return parentNode;
          }
        }

        // Prefixes diverge at diffBit.  Create a branching node that
        // holds the common prefix (mask length = diffBit).  The
        // existing node and the new node become its two children.
        Ipv4Prefix common(Ipv4Address(node->_pair.first.NetworkRaw()), diffBit);
        Node  *branch = new Node(common, std::set<uint32_t>{}, false, parent);

        uint8_t bitForExisting = node->_pair.first.Bit(diffBit);
        uint8_t bitForNew      = prefix.Bit(diffBit);

        inserted = true;
        Node * newLeaf = new Node(prefix, value, true, branch);
        resultNode = newLeaf;
        ++_size;
        branch->_children[bitForExisting] = node;
        node->_parent = branch;
        branch->_children[bitForNew] = newLeaf;
        return branch;
      }

      //----------------------------------------------------------------------
      //!  Recursive removal helper.
      //!
      //!  Returns the (possibly null) replacement node.  Post-removal
      //!  compaction: if a node has no value and zero children it is
      //!  deleted; if it has no value and exactly one child it is
      //!  bypassed (path compression maintained).
      //!----------------------------------------------------------------------
      Node *removeNode(Node *node, const Ipv4Prefix & prefix, bool & removed)
      {
        if (! node) {
          return nullptr;
        }
      
        if ((node->_hasValue) && (node->_pair.first == prefix)) {
          removed = true;
          node->_hasValue = false;
          --_size;

          if ((! node->_children[0]) && (! node->_children[1])) {
            delete node;
            return nullptr;
          }
          if ((! node->_children[0]) || (! node->_children[1])) {
            Node  *kid =
              node->_children[0] ? node->_children[0] : node->_children[1];
            delete node;
            return kid;
          }
          return node;
        }

        if (prefix.MaskLength() <= node->_pair.first.MaskLength()) {
          return node;
        }

        uint8_t   bit = prefix.Bit(node->_pair.first.MaskLength());
        Node     *child = node->_children[bit];

        if (child && child->_pair.first.Contains(prefix)) {
          node->_children[bit] = removeNode(child, prefix, removed);
          if (node->_children[bit]) {
            node->_children[bit]->_parent = node;
          }
        }

        if (removed) {
          if (! node->_hasValue) {
            if ((! node->_children[0]) && (! node->_children[1])) {
              delete node;
              return nullptr;
            }
            if ((! node->_children[0]) || (! node->_children[1])) {
              Node  *kid = node->_children[0]
                ? node->_children[0] : node->_children[1];
              delete node;
              return kid;
            }
          }
        }
        return node;
      }

      //----------------------------------------------------------------------
      //!  Return the bit index (0 = MSB) of the first bit where two
      //!  prefixes differ, considering only the first @c maxBits bits.
      //!  Returns -1 if they are identical within that range.
      //----------------------------------------------------------------------
      static int firstDiffBit(const Ipv4Prefix & a, const Ipv4Prefix & b,
                              uint8_t maxBits)
      {
        uint32_t  x  = ntohl(a.NetworkRaw()) ^ ntohl(b.NetworkRaw());
        int  ipBit = std::countl_zero(x);
        return (ipBit < maxBits) ? ipBit : -1;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void CombineAdjacents(std::list<Ipv4Prefix> & prefixes)
      {
        bool notDone = false;
        do {
          notDone = false;
          for (auto it = prefixes.begin(); it != prefixes.end(); ++it) {
            auto nit = it;  ++nit;
            if (nit != prefixes.end()) {
              Ipv4Prefix  pfx(*it);
              if (! pfx.Bit(pfx.MaskLength() - 1)) {
                //  Last bit is not set, so changing it to zero will yield the
                //  same network.
                if (pfx.MaskLength() == nit->MaskLength()) {
                  pfx.MaskLength(pfx.MaskLength() - 1);
                  if (pfx.Contains(*nit)) {
                    auto  existing = find(pfx);
                    if (existing == end()) {
                      *it = pfx;
                      prefixes.erase(nit);
                      notDone = true;
                    }
                    else {
                      // ???
                    }
                  }
                }
              }
            }
          }
        } while (notDone);
        return;
      }
    
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void CombinePrefixes(std::list<Ipv4Prefix> & prefixes,
                           const mapped_type & mt)
      {
        prefixes.sort();
        CombineAdjacents(prefixes);
#if 1
        for (auto it = prefixes.begin(); it != prefixes.end(); ++it) {
          auto nit = it; ++nit;
          while (nit != prefixes.end()) {
            if (it->Contains(*nit)) {
              std::vector<value_type>  matches;
              find_wider(*nit, matches);
              auto  widerit = std::find_if(matches.begin(), matches.end(),
                                           [&] (const auto & match)
                                           { return (match.second != mt); });
              if (widerit == matches.end()) {
                nit = prefixes.erase(nit);
              }
              else {
                ++nit;
              }
            }
            else {
              ++nit;
            }
          }
        }
#endif
        return;
      }

#if 0
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool have_wider_covering(const_iterator it) const
      {
        auto  *node = _root;
        auto   pfx = it->first;
        while (node) {
          if (node->_pair.first.Contains(pfx)) {
            if (node->_pair.first.MaskLength() < pfx.MaskLength()) {
              if (node->_hasValue) {
                if (std::ranges::includes(node->_pair.second, it->second)) {
                  return true;
                }
              }
            }
            else {
              break;
            }
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        return false;
      }
#endif

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const_iterator wider_covering(const_iterator it) const
      {
        const_iterator  rc = cend();
        auto       pfx = it->first;
        auto      *node = _root;
        while (node) {
          if (node->_pair.first.Contains(pfx)) {
            if (node->_pair.first.MaskLength() < pfx.MaskLength()) {
              if (node->_hasValue) {
                rc = const_iterator(_root, node);
              }
            }
            else {
              break;
            }
            uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
            node = node->_children[b];
          }
          else {
            break;
          }
        }
        if (rc != cend()) {
          if (! std::includes(rc->second.begin(),
                              rc->second.end(),
                              it->second.begin(),
                              it->second.end())) {
            rc = cend();
          }
        }
        return rc;
      }

    };

  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINIPV4NET2AS_HH_
