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
//!  @file TestCaidaV4Routeviews.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <regex>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "DwmUnitAssert.hh"
#include "DwmMcCurtainAS2Ipv4Net.hh"

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool
LoadRouteviewsAsVector(vector<pair<Dwm::Ipv4Prefix,set<uint32_t>>> & v)
{
  using boost::iostreams::filtering_streambuf;
  using boost::iostreams::gzip_decompressor;
  using boost::iostreams::gzip_compressor;

  v.clear();
  ifstream  is("inputs/routeviews-rv2-20240406.pfx2as.gz");
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
        v.push_back({pfx,GetASNumbers(asnumstr)});
      }
    }
    is.close();
  }
  
  return (! v.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLookups(const Dwm::McCurtain::Ipv4Net2AS & db)
{
  vector<pair<Dwm::Ipv4Prefix,set<uint32_t>>>  v;
  if (UnitAssert(LoadRouteviewsAsVector(v))) {
    auto have_orig_ases = [] (const auto & matches, const auto & origases)
    {
      auto count = origases.size();
      for (const auto & as : origases) {
        for (const auto & match : matches) {
          if (match.second.find(as) != match.second.end()) {
            --count;
            if (! count) {
              return true;
            }
            break;
          }
        }
      }
      return false;
    };
    
    for (const auto & entry : v) {
      std::vector<Dwm::McCurtain::Ipv4Net2AS::value_type>  matches;
      if (UnitAssert(db.find_matches(entry.first, matches))) {
        UnitAssert(have_orig_ases(matches, entry.second));
      }
      else {
        cerr << "failed lookup for " << entry.first << '\n';
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::McCurtain::CaidaV4Routeviews  rv;
  if (UnitAssert(rv.Load("inputs/routeviews-rv2-20240406.pfx2as.gz"))) {
    rv.Aggregate();
    Dwm::McCurtain::Ipv4Net2AS  ipv42as(rv);
    TestLookups(ipv42as);
  }

  if (Dwm::Assertions::Total().Failed())
    Dwm::Assertions::Print(cerr, true);
  else
    cout << Dwm::Assertions::Total() << " passed" << endl;

  exit(0);
  
testFailed:
  
  Dwm::Assertions::Print(cerr, true);
  exit(1);
}
