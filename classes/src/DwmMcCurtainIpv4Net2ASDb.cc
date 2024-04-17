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
//!  \file DwmMcCurtainIpv4Net2ASDb.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <regex>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "DwmMcCurtainIpv4Net2ASDb.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static set<uint32_t> GetASNumbers(string asnumstr)
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
    //!  
    //------------------------------------------------------------------------
    bool Ipv4Net2ASDb::LoadCAIDARouteViews(const std::string & path)
    {
      using boost::iostreams::filtering_streambuf;
      using boost::iostreams::gzip_decompressor;
      using boost::iostreams::gzip_compressor;

      _netASes.Clear();
      ifstream  is(path);
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
            _netASes[pfx] = GetASNumbers(asnumstr);
          }
        }
        is.close();
      }
      bool  rc = (! _netASes.Empty());
      if (rc) {
        _netASes.Coalesce();
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Ipv4Net2ASDb::Load(const std::string & path)
    {
      bool      rc = false;
      ifstream  is(path);
      if (is) {
        if (_netASes.Read(is)) {
          rc = true;
        }
      }
      return rc;
    }
          
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Ipv4Net2ASDb::Save(const std::string & path) const
    {
      bool      rc = false;
      ofstream  os(path);
      if (os) {
        if (_netASes.Write(os)) {
          rc = true;
        }
      }
      return rc;
    }
    
    
  }  // namespace McCurtain

}  // namespace Dwm
