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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  @file DwmMcCurtainCaidaV4Routeviews.hh
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <regex>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "DwmMcCurtainCaidaV4Routeviews.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;

    //------------------------------------------------------------------------
    bool CaidaV4Routeviews::ASMapValue::HaveWider(const Ipv4Prefix & pfx) const
    {
      Ipv4Prefix  tmppfx(pfx);
      for (auto rit = _prefixSets.rbegin(); rit != _prefixSets.rend(); ++rit) {
        if (rit->first >= pfx.MaskLength()) {
          continue;
        }
        tmppfx.MaskLength(rit->first);
        if (rit->second.find(tmppfx) != rit->second.end()) {
          return true;
        }
      }
      return false;
    }

    //------------------------------------------------------------------------
    void CaidaV4Routeviews::ASMapValue::SwallowSpecifics()
    {
      auto  sit = _prefixSets.rbegin();
      while (sit != _prefixSets.rend()) {
        auto  curit = sit->second.begin();
        while (curit != sit->second.end()) {
          if (HaveWider(*curit)) {
            curit = sit->second.erase(curit);
          }
          else {
            ++curit;
          }
        }
        ++sit;
      }
      return;
    }
    
    //------------------------------------------------------------------------
    void CaidaV4Routeviews::ASMapValue::Aggregate()
    {
      SwallowSpecifics();
      
      auto  sit = _prefixSets.rbegin();
      while (sit != _prefixSets.rend()) {
        if (sit->second.size() > 1) {
          auto  curit = sit->second.begin();
          auto  nxtit = curit; ++nxtit;
          while (nxtit != sit->second.end()) {
            Ipv4Prefix  curnxtpfx(*curit);
            if ((! curit->Bit(curit->MaskLength() - 1))
                && (++curnxtpfx == *nxtit)) {
              Ipv4Prefix  aggpfx(curit->Network(), curit->MaskLength() - 1);
              _prefixSets[aggpfx.MaskLength()].insert(aggpfx);
              sit->second.erase(curit);
              curit = sit->second.erase(nxtit);
              if (curit == sit->second.end()) {
                break;
              }
              nxtit = curit; ++nxtit;
            }
            else {
              ++nxtit;
              ++curit;
            }
          }
          ++sit;
        }
        else {
          ++sit;
        }
      }
      auto  dit = _prefixSets.begin();
      while (dit != _prefixSets.end()) {
        if (dit->second.empty()) {
          dit = _prefixSets.erase(dit);
        }
        else {
          ++dit;
        }
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::set<uint32_t> GetASNumbers(std::string asnumstr)
    {
      static regex  s_rgx("([0-9]+)", regex::optimize|regex::ECMAScript);
      set<uint32_t>  rc;
      for (smatch sm; regex_search(asnumstr, sm, s_rgx); ) {
        rc.insert(stoul(sm.str()));
        asnumstr = sm.suffix();
      }
      return rc;
    }

    //------------------------------------------------------------------------
    bool CaidaV4Routeviews::Load(const std::string & filePath)
    {
      using boost::iostreams::filtering_streambuf;
      using boost::iostreams::gzip_decompressor;
      using boost::iostreams::gzip_compressor;

      _asSets.clear();
      ifstream  is(filePath);
      if (is) {
        filtering_streambuf<boost::iostreams::input>  gzin;
        gzin.push(gzip_decompressor());
        gzin.push(is);
        istream   gzis(&gzin);
        string    addrstr, asnumstr;
        uint16_t  maskLen;
        while (gzis >> addrstr >> maskLen >> asnumstr) {
          if (maskLen < 33) {
            Dwm::Ipv4Prefix  pfx(addrstr, (uint8_t)maskLen);
            _asSets[GetASNumbers(asnumstr)].Insert(pfx);
          }
        }
        is.close();
      }
      return (! _asSets.empty());
    }

    //------------------------------------------------------------------------
    void CaidaV4Routeviews::Aggregate()
    {
      size_t  origTotalPrefixes = TotalPrefixes();

      for (auto & asSet : _asSets) {
        asSet.second.Aggregate();
      }
      std::cerr << "aggregated " << origTotalPrefixes << " -> "
                << TotalPrefixes() << '\n';      
      return;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
