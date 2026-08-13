//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024, 2026
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
//!  \file mkcurtain.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Ipv4Net2AS unit tests
//---------------------------------------------------------------------------

#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"
#include "DwmMcCurtainIpv4Net2AS.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestMakeIpv4ToAS(Dwm::McCurtain::Ipv4Net2AS & db,
                             const std::string & routeViewsPath)
{
  if (UnitAssert(db.LoadCAIDARouteViews(routeViewsPath))) {
    size_t preaggsize = db.size();
    db.Aggregate();
    cerr << "db.size(): " << preaggsize << " -> " << db.size() << '\n';
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLookups(const Dwm::McCurtain::Ipv4Net2AS & db,
                        const std::string & routeViewsPath)
{
  using boost::iostreams::filtering_streambuf;
  using boost::iostreams::gzip_decompressor;
  using boost::iostreams::gzip_compressor;

  vector<pair<Ipv4Prefix,string>>  pfxVec;
  
  ifstream  is(routeViewsPath);
  if (UnitAssert(is)) {
    filtering_streambuf<boost::iostreams::input>  gzin;
    gzin.push(gzip_decompressor());
    gzin.push(is);
    istream   gzis(&gzin);
    uint16_t  maskLen;
    string    addrstr, asstr;
    while (gzis >> addrstr >> maskLen >> asstr) {
      Ipv4Prefix  pfx(Ipv4Address(addrstr),(uint8_t)maskLen);
      pfxVec.push_back({pfx,asstr});
    }
    is.close();
  }
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
    cerr << "count: " << count << '\n';
    return false;
  };
    
  for (const auto & entry : pfxVec) {
    std::vector<Dwm::McCurtain::Ipv4Net2AS::value_type>  matches;
    if (UnitAssert(db.find_matches(entry.first, matches))) {
      UnitAssert(have_orig_ases(matches, GetASNumbers(entry.second)));
    }
    else {
      cerr << "failed lookup for " << entry.first << '\n';
    }
  }
  return;
}
        
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestSave(Dwm::McCurtain::Ipv4Net2AS & db,
                     const std::string & outPath)
{
  return UnitAssert(db.Save(outPath));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestLoad(const Dwm::McCurtain::Ipv4Net2AS & db,
                     const std::string & inPath)
{
  bool  rc = false;
  Dwm::McCurtain::Ipv4Net2AS  db2;
  if (UnitAssert(db2.Load(inPath))) {
    rc = UnitAssert(db.size() == db2.size());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("TestIpv4Net2AS", LOG_PERROR|LOG_PID, "user");

  Dwm::McCurtain::Ipv4Net2AS  db;
  if (TestMakeIpv4ToAS(db, "inputs/routeviews-rv2-20240406.pfx2as.gz")) {
    TestLookups(db, "inputs/routeviews-rv2-20240406.pfx2as.gz");
    if (TestSave(db, "ipv42as.db")) {
      TestLoad(db, "ipv42as.db");
    }
    // std::remove("ipv42as.db");
  }
  for (auto it = db.cbegin(); it != db.cend(); ++it) {
    cout << it->first << ' ' << it->second << '\n';
  }
  
  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;

  exit(0);
  
testFailed:
  
  Assertions::Print(cerr, true);
  exit(1);
}
