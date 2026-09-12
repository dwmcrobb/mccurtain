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
//!  \file TestAS2Ipv6Net.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::AS2Ipv6Net unit tests
//---------------------------------------------------------------------------

#include <iostream>
#include <string>

#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"
#include "DwmMcCurtainIpv6Net2AS.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestMakeIpv6ToAS(Dwm::McCurtain::AS2Ipv6Net & db,
                               const std::string & routeViewsPath)
{
  McCurtain::CaidaV6Routeviews  rv;
  if (UnitAssert(rv.Load(routeViewsPath))) {
    db.Load(rv);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestSave(Dwm::McCurtain::AS2Ipv6Net & asdb,
                     const std::string & outPath)
{
  return UnitAssert(asdb.Save(outPath));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestLoad(const Dwm::McCurtain::AS2Ipv6Net & asdb,
                     const string & path)
{
  bool  rc = false;
  Dwm::McCurtain::AS2Ipv6Net  asdb2;
  if (UnitAssert(asdb2.Load(path))) {
    rc = UnitAssert(asdb2.Size() == asdb.Size());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(const Dwm::McCurtain::AS2Ipv6Net & asdb)
{
  int  fd = ::open("TestAS2Ipv6Net.fd", O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (UnitAssert(0 <= fd)) {
    UnitAssert(asdb.Write(fd) > 0);
    ::close(fd);
    fd = ::open("TestAS2Ipv6Net.fd", O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      Dwm::McCurtain::AS2Ipv6Net  asdb2;
      if (UnitAssert(asdb2.Read(fd) > 0)) {
        UnitAssert(asdb2.Size() > 0);
        UnitAssert(asdb2.Size() == asdb.Size());
      }
      ::close(fd);
    }
    std::remove("TestAS2Ipv6Net.fd");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(const Dwm::McCurtain::AS2Ipv6Net & asdb)
{
  FILE  *f = fopen("TestAS2Ipv6Net.file", "wb");
  if (UnitAssert(f)) {
    UnitAssert(asdb.Write(f));
    fclose(f);
    f = fopen("TestAS2Ipv6Net.file", "rb");
    if (UnitAssert(f)) {
      Dwm::McCurtain::AS2Ipv6Net  asdb2;
      if (UnitAssert(asdb2.Read(f))) {
        UnitAssert(asdb2.Size() > 0);
        UnitAssert(asdb2.Size() == asdb.Size());
      }
      fclose(f);
    }
    std::remove("TestAS2Ipv6Net.file");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO(const Dwm::McCurtain::AS2Ipv6Net & asdb)
{
  BZFILE  *bzf = BZ2_bzopen("TestAS2Ipv6Net.bz2", "wb");
  if (UnitAssert(bzf)) {
    UnitAssert(asdb.BZWrite(bzf));
    BZ2_bzclose(bzf);
    bzf = BZ2_bzopen("TestAS2Ipv6Net.bz2", "rb");
    if (UnitAssert(bzf)) {
      Dwm::McCurtain::AS2Ipv6Net  asdb2;
      if (UnitAssert(asdb2.BZRead(bzf))) {
        BZ2_bzclose(bzf);
        UnitAssert(asdb2.Size() > 0);
        UnitAssert(asdb2.Size() == asdb.Size());
      }
    }
    std::remove("TestAS2Ipv6Net.bz2");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO(const Dwm::McCurtain::AS2Ipv6Net & asdb)
{
  gzFile  gzf = gzopen("TestAS2Ipv6Net.gz", "wb");
  if (UnitAssert(gzf)) {
    UnitAssert(asdb.Write(gzf));
    gzclose(gzf);
    gzf = gzopen("TestAS2Ipv6Net.gz", "rb");
    if (UnitAssert(gzf)) {
      Dwm::McCurtain::AS2Ipv6Net  asdb2;
      if (UnitAssert(asdb2.Read(gzf))) {
        gzclose(gzf);
        UnitAssert(asdb2.Size() > 0);
        UnitAssert(asdb2.Size() == asdb.Size());
      }
    }
    std::remove("TestAS2Ipv6Net.gz");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("TestIpv6Net2AS", LOG_PERROR|LOG_PID, "user");

  Dwm::McCurtain::AS2Ipv6Net  asdb;

  if (TestMakeIpv6ToAS(asdb, "inputs/routeviews-rv6-20260828.pfx2as.gz")) {
    if (UnitAssert(TestSave(asdb, "as2ipv6.db"))) {
      TestLoad(asdb, "as2ipv6.db");
      TestDescriptorIO(asdb);
      TestFileIO(asdb);
      TestBZ2IO(asdb);
      TestGZIO(asdb);
      if (UnitAssert(TestSave(asdb, "as2ipv6.bz2"))) {
        UnitAssert(TestLoad(asdb, "as2ipv6.bz2"));
        std::remove("as2ipv6.bz2");
      }
      if (UnitAssert(TestSave(asdb, "as2ipv6.gz"))) {
        UnitAssert(TestLoad(asdb, "as2ipv6.gz"));
        std::remove("as2ipv6.gz");
      }
    }
    std::remove("as2ipv6.db");
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
