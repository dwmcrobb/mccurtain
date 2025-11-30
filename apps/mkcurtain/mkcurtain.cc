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
//!  \file mkcurtain.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

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
  cerr << "Usage: " << argv0 << " [-p] ASesOfInterest_json_file asdbFile\n";
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("mkcurtain", LOG_PERROR|LOG_PID, "user");

  bool  generatePfList = false;
  
  extern int  optind;
  int         optChar;
  while ((optChar = getopt(argc, argv, "p")) != -1) {
    switch (optChar) {
      case 'p':
        generatePfList = true;
        break;
      default:
        Usage(argv[0]);
        return 1;
    }
  }

  if ((argc - optind) != 2) {
    Usage(argv[0]);
    return 1;
  }
  
  Dwm::McCurtain::ASes  ases;
  if (ases.Load(argv[optind], argv[optind+1])) {
    if (! generatePfList) {
      cout << ases.ToJson().dump(4) << '\n' << '\n';
    }
    else {
      std::vector<Dwm::Ipv4Prefix>  pfList;
      ases.MakePfList(pfList, { });
      for (const auto & pfe : pfList) {
        cout << pfe << '\n';
      }
    }
    return 0;
  }
  else {
    cerr << "Failed to load ASes\n";
    return 1;
  }
}
