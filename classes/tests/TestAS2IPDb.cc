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
//!  @file TestAS2IPDb.cc
//!  @author Daniel W. McRobb
//!  @brief Dwm::McCurtain::AS2IPDb unit tests
//---------------------------------------------------------------------------

#include <fstream>

#include "DwmUnitAssert.hh"
#include "DwmMcCurtainAS2IPDb.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestMakeASToIpv4(Dwm::McCurtain::AS2Ipv4Net & as2ip4,
                             const std::string & routeViewsPath)
{
  McCurtain::CaidaV4Routeviews  rv;
  if (UnitAssert(rv.Load(routeViewsPath))) {
    rv.Aggregate();
    return as2ip4.Load(rv);
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestMakeASToIpv6(Dwm::McCurtain::AS2Ipv6Net & as2ip6,
                             const std::string & routeViewsPath)
{
  McCurtain::CaidaV6Routeviews  rv;
  if (UnitAssert(rv.Load(routeViewsPath))) {
    rv.Aggregate();
    as2ip6.Load(rv);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSaveLoad(const Dwm::McCurtain::AS2Ipv4Net & as2ip4,
                         const Dwm::McCurtain::AS2Ipv6Net & as2ip6,
                         const string & filename)
{
  using  as2ipdb = McCurtain::AS2IPDb;
  
  if (UnitAssert(as2ipdb::Save(filename, as2ip4, as2ip6))) {
    Dwm::McCurtain::AS2Ipv4Net  as2ip4_2;
    Dwm::McCurtain::AS2Ipv6Net  as2ip6_2;
    if (UnitAssert(as2ipdb::Load(filename, as2ip4_2, as2ip6_2))) {
      UnitAssert(as2ip4_2.Size() == as2ip4.Size());
      UnitAssert(as2ip6_2.Size() == as2ip6.Size());
    }
    std::remove(filename.c_str());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  const string  rv4file("inputs/routeviews-rv2-20240406.pfx2as.gz");
  const string  rv6file("inputs/routeviews-rv6-20260828.pfx2as.gz");
  Dwm::McCurtain::AS2Ipv4Net  as2ip4;
  Dwm::McCurtain::AS2Ipv6Net  as2ip6;
  
  if (UnitAssert(TestMakeASToIpv4(as2ip4, rv4file))) {
    if (UnitAssert(TestMakeASToIpv6(as2ip6, rv6file))) {
      TestSaveLoad(as2ip4, as2ip6, "TestAS2IPDb.db");
      TestSaveLoad(as2ip4, as2ip6, "TestAS2IPDb.bz2");
      TestSaveLoad(as2ip4, as2ip6, "TestAS2IPDb.gz");
    }
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
