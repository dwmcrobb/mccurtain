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
//!  \file TestAS2Ipv4NetDb.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <iostream>
#include <string>

#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"
#include "DwmMcCurtainAS2Ipv4NetDb.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestMakeIpv4ToASDb(Dwm::McCurtain::Ipv4Net2ASDb & db,
                               const std::string & routeViewsPath)
{
  return UnitAssert(db.LoadCAIDARouteViews(routeViewsPath));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestSave(Dwm::McCurtain::AS2Ipv4NetDb & asdb,
                     const std::string & outPath)
{
  return UnitAssert(asdb.Save(outPath));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestLoad(const Dwm::McCurtain::Ipv4Net2ASDb & netdb,
                     Dwm::McCurtain::AS2Ipv4NetDb & asdb)
{
  bool  rc = false;
  if (UnitAssert(asdb.Load(netdb))) {
    rc = UnitAssert(asdb.Size() >= netdb.Entries().Size());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestLoad(const Dwm::McCurtain::AS2Ipv4NetDb & asdb,
                     const string & path)
{
  bool  rc = false;
  Dwm::McCurtain::AS2Ipv4NetDb  asdb2;
  if (UnitAssert(asdb2.Load(path))) {
    rc = UnitAssert(asdb2.Size() == asdb.Size());
  }
  return rc;
}
      
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("TestIpv4Net2ASDb", LOG_PERROR|LOG_PID, "user");

  Dwm::McCurtain::Ipv4Net2ASDb  netdb;
  Dwm::McCurtain::AS2Ipv4NetDb  asdb;

  if (TestMakeIpv4ToASDb(netdb, "inputs/routeviews-rv2-20240406.pfx2as.gz")) {
    if (TestLoad(netdb, asdb)) {
      if (UnitAssert(TestSave(asdb, "as2ipv4.db"))) {
        TestLoad(asdb, "as2ipv4.db");
      }
    }
    // std::remove("as2ipv4.db");
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
