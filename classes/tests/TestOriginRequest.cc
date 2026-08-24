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
//!  @file TestOriginRequest.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <sstream>

#include "DwmUnitAssert.hh"
#include "DwmMcCurtainOriginRequest.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestJson()
{
  ifstream  is("inputs/origin_request.json");
  if (UnitAssert(is)) {
    nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
    if (UnitAssert(! j.is_discarded())) {
      McCurtain::OriginRequest  req;
      if (UnitAssert(req.FromJson(j))) {
        const Ipv4Address  *addr = req.Address().Addr<Ipv4Address>();
        if (UnitAssert(addr)) {
          UnitAssert(Ipv4Address("38.19.214.1") == *addr);
        }
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIO()
{
  ifstream  is("inputs/origin_request.json");
  if (UnitAssert(is)) {
    nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
    if (UnitAssert(! j.is_discarded())) {
      McCurtain::OriginRequest  req;
      if (UnitAssert(req.FromJson(j))) {
        stringstream  ss;
        if (UnitAssert(req.Write(ss))) {
          McCurtain::OriginRequest  req2;
          if (UnitAssert(req2.Read(ss))) {
            UnitAssert(req2 == req);
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
