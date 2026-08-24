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
//!  @file TestOriginPrefix.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcCurtainOriginPrefix.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestJson()
{
  ifstream  is("inputs/origin_prefix.json");
  if (UnitAssert(is)) {
    nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
    if (UnitAssert(! j.is_discarded())) {
      McCurtain::OriginPrefix  origpfx;
      if (UnitAssert(origpfx.FromJson(j))) {
        const Ipv4Prefix  *v4pfx = origpfx.Prefix().Prefix<Ipv4Prefix>();
        if (UnitAssert(v4pfx)) {
          UnitAssert(*v4pfx == Ipv4Prefix("38.19.214.0/24"));
          if (UnitAssert(origpfx.ASes().size() == 2)) {
            UnitAssert(origpfx.ASes()[0].Number() == 174);
            UnitAssert(origpfx.ASes()[0].CountryCode() == "US");
            UnitAssert(origpfx.ASes()[0].Name()
                       == "Cogent Communications, LLC");
            
            UnitAssert(origpfx.ASes()[1].Number() == 53107);
            UnitAssert(origpfx.ASes()[1].CountryCode() == "BR");
            UnitAssert(origpfx.ASes()[1].Name() == "EVEO S.A.");
          }
        }
        nlohmann::json  j2 = origpfx.ToJson();
        if (UnitAssert(! j2.is_discarded())) {
          McCurtain::OriginPrefix  origpfx2;
          if (UnitAssert(origpfx2.FromJson(j2))) {
            UnitAssert(origpfx2 == origpfx);
          }
        }
      }
    }
    is.close();
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIO()
{
  ifstream  is("inputs/origin_prefix.json");
  if (UnitAssert(is)) {
    nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
    if (UnitAssert(! j.is_discarded())) {
      McCurtain::OriginPrefix  origpfx;
      if (UnitAssert(origpfx.FromJson(j))) {
        stringstream  ss;
        if (UnitAssert(origpfx.Write(ss))) {
          McCurtain::OriginPrefix  origpfx2;
          if (UnitAssert(origpfx2.Read(ss))) {
            UnitAssert(origpfx2 == origpfx);
          }
        }
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
  TestJson();
  TestIO();
  
  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;

  exit(0);
  
testsFailed:
  
  Assertions::Print(cerr, true);
  exit(1);
}
