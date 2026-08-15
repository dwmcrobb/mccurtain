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
//!  \file mccurtaindbcat.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <cstdlib>

#include "DwmMcCurtainAS2Ipv4Net.hh"
#include "DwmMcCurtainIpv4Net2AS.hh"

//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  std::cerr << "Usage: " << argv0 << " [-i ipv42as_db] [-a as2ipv4_db]\n";
  return;
}

//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::string  ip2asFile("/usr/local/etc/ipv42as.db");
  std::string  as2ipFile("/usr/local/etc/as2ipv4.db");

  int  optchar;
  
  while ((optchar = getopt(argc, argv, "a:i:")) != -1) {
    switch (optchar) {
      case 'a':
        as2ipFile = optarg;
        break;
      case 'i':
        ip2asFile = optarg;
        break;
      default:
        Usage(argv[0]);
        exit(1);
        break;
    }
  }
  
  Dwm::McCurtain::AS2Ipv4Net  as2ipv4;
  if (as2ipv4.Load(as2ipFile)) {
    for (const auto & entry : as2ipv4.Nets()) {
      std::cout << entry.first << '\n';
      for (const auto & pfx : entry.second) {
        std::cout << "  " << pfx.first << '\n';
      }
    }
  }
  
  Dwm::McCurtain::Ipv4Net2AS  ipv42as;
  if (ipv42as.Load(ip2asFile)) {
    for (const auto & entry : ipv42as) {
      std::cout << entry.first << ' ';
      std::string  sep("");
      for (const auto & as : entry.second) {
        std::cout << sep << as;
        sep = ',';
      }
      std::cout << '\n';
    }
    return 0;
  }
}
