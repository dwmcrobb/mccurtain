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
//!  \file mccurtain.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <iomanip>
#include <iostream>
#include <regex>
#include <thread>

#include "DwmSysLogger.hh"
#include "DwmCredencePeer.hh"
#include "DwmMcCurtainRequests.hh"
#include "DwmMcCurtainResponses.hh"
#include "DwmMcCurtainVersion.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetPeer(const string & host, Credence::Peer & peer)
{
  bool  rc = false;
  if (peer.Connect(host, 2126)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      rc = true;
    }
    else {
      peer.Disconnect();
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void
PrintIpv4AddrResponse(const Dwm::McCurtain::Ipv4AddrResponse & resp)
{
  for (const auto & entry : resp) {
    cout << std::get<0>(entry) << ":\n";
    for (const auto & ase : std::get<1>(entry)) {
      cout << "  " << setiosflags(ios::left) << setw(10) << ase.first << ' '
           << setw(2) << ase.second.CountryCode() << ' '
           << ase.second.Name() << '\n';
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void
PrintASPrefixesResponse(const Dwm::McCurtain::ASPrefixesResponse & resp)
{
  cout << setiosflags(ios::left) << setw(10) << std::get<0>(resp) << ' '
       << setw(2) << std::get<1>(resp).CountryCode() << ' '
       << std::get<1>(resp).Name() << '\n';
  for (const auto & pfx : std::get<2>(resp)) {
    cout << "  " << pfx.ToShortString() << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "Usage: " << argv0 << " [-h mccurtaind_host] ipv4addr\n"
       << "       " << argv0 << " [-h mccurtaind_host] AS_number\n"
       << "       " << argv0 << " -V\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  extern int        optind;
  int               optChar;
  string            host;
  
  Dwm::SysLogger::Open("mccurtain", LOG_PERROR, LOG_USER);
  Dwm::SysLogger::MinimumPriority(LOG_ERR);
  Dwm::SysLogger::ShowFileLocation(true);

  while ((optChar = getopt(argc, argv, "h:V")) != -1) {
    switch (optChar) {
      case 'h':
        host = optarg;
        break;
      case 'V':
        cout << Dwm::McCurtain::Version.Version() << '\n';
        return 0;
        break;
      default:
        Usage(argv[0]);
        return 1;
        break;
    }
  }

  if (host.empty()) {
    host = "kiva.mcplex.net";
  }

  if (optind >= argc) {
    Usage(argv[0]);
    return 1;
  }
  
  Credence::Peer  peer;
  if (GetPeer(host, peer)) {
    string  arg(argv[optind]);
    if (arg.find_first_of('.') != string::npos) {
      Dwm::McCurtain::Request  req{Dwm::Ipv4Address(arg)};
      if (peer.Send(req)) {
        Dwm::McCurtain::Ipv4AddrResponse  resp;
        if (peer.Receive(resp)) {
          PrintIpv4AddrResponse(resp);
          return 0;
        }
      }
    }
    else {
      try {
        uint32_t  asNum = stoul(arg);
        Dwm::McCurtain::Request  req{asNum};
        if (peer.Send(req)) {
          Dwm::McCurtain::ASPrefixesResponse  resp;
          if (peer.Receive(resp)) {
            PrintASPrefixesResponse(resp);
            return 0;
          }
        }
      }
      catch (...) {
      }
    }
  }
  return 1;
}

