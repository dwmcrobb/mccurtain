//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024, 2025
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
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <iostream>
#include <string>

#include "DwmSysLogger.hh"
#include "DwmMcCurtainASes.hh"
#include "DwmMcCurtainAS2Ipv4NetDb.hh"
#include "DwmMcCurtainVersion.hh"

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "Usage: " << argv0
       << " [-i ipv4ToASdb] [-a asToIpv4db] routeViewsFile\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("mkcurtaindb", LOG_PERROR|LOG_PID, "user");

  string  as2Ipv4DbFile = "as2ipv4.db";
  string  ipv42AsDbFile = "ipv42as.db";

  extern int  optind;
  int         optChar;
  while ((optChar = getopt(argc, argv, "a:i:")) != -1) {
    switch (optChar) {
      case 'a':
        as2Ipv4DbFile = optarg;
        break;
      case 'i':
        ipv42AsDbFile = optarg;
        break;
      default:
        Usage(argv[0]);
        exit(1);
        break;
    }
  }

  if (optind >= argc) {
    Usage(argv[0]);
    exit(1);
  }
  
  Dwm::McCurtain::Ipv4Net2ASDb  netdb;
  if (netdb.LoadCAIDARouteViews(argv[optind])) {
    Dwm::McCurtain::AS2Ipv4NetDb  asdb;
    if (asdb.Load(netdb)) {
      if (netdb.Save(ipv42AsDbFile)) {
        if (asdb.Save(as2Ipv4DbFile)) {
          return 0;
        }
        else {
          cerr << "Failed to save AS to ipv4 net database to '"
               << as2Ipv4DbFile << "'\n";
        }
      }
      else {
        cerr << "Failed to save ipv4 net to AS database to '"                
             << ipv42AsDbFile << "'\n";
      }
    }
    else {
      cerr << "Failed to load AS to ipv4 net database from ipv4 to AS "
           << "database\n";
    }
  }
  else {
    cerr << "Failed to load ipv4 net to AS database from routeviews file '"
         << argv[optind] << "'\n";
  }

  return 1;
}
