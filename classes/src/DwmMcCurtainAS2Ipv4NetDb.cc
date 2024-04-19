//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmMcCurtainAS2Ipv4NetDb.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>

#include "DwmStreamIO.hh"
#include "DwmMcCurtainAS2Ipv4NetDb.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool AS2Ipv4NetDb::Load(const Ipv4Net2ASDb & net2asdb)
    {
      _asNets.clear();
      const auto & hashMaps = net2asdb.Entries().HashMaps();
      for (uint8_t i = 0; i < 33; ++i) {
        auto  addri = hashMaps[i].begin();
        for ( ; addri != hashMaps[i].end(); ++addri) {
          for (auto asi = addri->second.begin();
               asi != addri->second.end(); ++asi) {
            Ipv4Prefix net(addri->first, i);
            _asNets[*asi][net] = 1;
          }
        }
      }
      return (! _asNets.empty());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool AS2Ipv4NetDb::Load(const std::string & path)
    {
      bool  rc = false;
      ifstream  is(path);
      if (is) {
        if (Read(is)) {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool AS2Ipv4NetDb::Save(const std::string & path) const
    {
      bool  rc = false;
      ofstream  os(path);
      if (os) {
        if (Write(os)) {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::istream & AS2Ipv4NetDb::Read(std::istream & is)
    {
      _asNets.clear();
      uint32_t  numASes;
      if (StreamIO::Read(is, numASes)) {
        for (uint32_t asidx = 0; asidx < numASes; ++asidx) {
          uint32_t  asNum;
          if (StreamIO::Read(is, asNum)) {
            uint8_t  pfxlens;
            if (StreamIO::Read(is, pfxlens)) {
              for (uint8_t pfxnum = 0; pfxnum < pfxlens; ++pfxnum) {
                uint8_t  pfxlen;
                if (StreamIO::Read(is, pfxlen)) {
                  uint32_t  numpfxs;
                  if (StreamIO::Read(is, numpfxs)) {
                    Ipv4Address  addr;
                    for (uint32_t pfxnum = 0; pfxnum < numpfxs; ++pfxnum) {
                      if (StreamIO::Read(is, addr)) {
                        _asNets[asNum][Ipv4Prefix(addr, pfxlen)] = 1;
                      }
                      else {
                        break;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      
      return is;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & AS2Ipv4NetDb::Write(std::ostream & os) const
    {
      uint32_t  numASes = _asNets.size();
      if (StreamIO::Write(os, numASes)) {
        for (auto asi = _asNets.begin(); asi != _asNets.end(); ++asi) {
          uint32_t  asNum = asi->first;
          if (StreamIO::Write(os, asNum)) {
            uint8_t  pfxlens = 0;
            for (uint8_t pfxlen = 0; pfxlen < 33; ++pfxlen) {
              if (! asi->second.HashMaps()[pfxlen].empty()) {
                ++pfxlens;
              }
            }
            if (StreamIO::Write(os, pfxlens)) {
              for (uint8_t pfxlen = 0; pfxlen < 33; ++pfxlen) {
                if (! asi->second.HashMaps()[pfxlen].empty()) {
                  if (StreamIO::Write(os, pfxlen)) {
                    uint32_t  numpfxs = asi->second.HashMaps()[pfxlen].size();
                    if (StreamIO::Write(os, numpfxs)) {
                      for (auto addri = asi->second.HashMaps()[pfxlen].begin();
                           addri != asi->second.HashMaps()[pfxlen].end();
                           ++addri) {
                        StreamIO::Write(os, addri->first);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      return os;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
