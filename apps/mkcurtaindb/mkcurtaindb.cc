//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024, 2025, 2026
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
//!  \brief utility to create binary data file from routeviews data
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "DwmSysLogger.hh"
#include "DwmMcCurtainAS2IPDb.hh"
#include "DwmMcCurtainVersion.hh"

using namespace std;
namespace fs = std::filesystem;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "Usage: " << argv0
       << " [-o outfile] routeViewsIPv4File routeViewsIPv6File\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool PopulateV6Data(const string & caidarvfile,
                           Dwm::McCurtain::AS2Ipv6Net & as2ip)
{
  Dwm::McCurtain::CaidaV6Routeviews  rv;
  if (rv.Load(caidarvfile)) {
    rv.Aggregate();
    as2ip.Load(rv);
    return true;
  }
  else {
    cerr << "Failed to load data from '" << caidarvfile << "'\n";
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool PopulateV4Data(const string & caidarvfile,
                           Dwm::McCurtain::AS2Ipv4Net & as2ip)
{
  Dwm::McCurtain::CaidaV4Routeviews  rv;
  if (rv.Load(caidarvfile)) {
    rv.Aggregate();
    as2ip.Load(rv);
    return true;
  }
  else {
    cerr << "Failed to load data from '" << caidarvfile << "'\n";
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool SaveAS2IpData(const string & outfile,
                          Dwm::McCurtain::AS2Ipv4Net & asip4,
                          Dwm::McCurtain::AS2Ipv6Net & asip6)
{
  return Dwm::McCurtain::AS2IPDb::Save(outfile, asip4, asip6);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("mkcurtaindb", LOG_PERROR|LOG_PID, "user");

  string  mccDbFile = "/usr/local/etc/mccas2ip.db";

  extern int  optind;
  int         optChar;
  while ((optChar = getopt(argc, argv, "o:")) != -1) {
    switch (optChar) {
      case 'o':
        mccDbFile = optarg;
        break;
      default:
        Usage(argv[0]);
        exit(1);
        break;
    }
  }

  if ((optind + 1) >= argc) {
    Usage(argv[0]);
    exit(1);
  }

  Dwm::McCurtain::AS2Ipv4Net  as2ip4;
  Dwm::McCurtain::AS2Ipv6Net  as2ip6;
  
  if (PopulateV4Data(argv[optind], as2ip4)) {
    if (PopulateV6Data(argv[optind+1], as2ip6)) {
      if (SaveAS2IpData(mccDbFile, as2ip4, as2ip6)) {
        return 0;
      }
      else {
        cerr << "Failed to save data to '" << mccDbFile << "'\n";
      }
    }
    else {
      cerr << "Failed to populate IPv6 data\n";
    }
  }
  else {
    cerr << "Failed to populate IPv4 data\n";
  }

  return 1;
}
