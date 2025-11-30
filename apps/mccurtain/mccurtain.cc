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

#include <cstdlib>
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
static bool GetPeer(const string & host, uint16_t port, Credence::Peer & peer)
{
  bool  rc = false;
  if (peer.Connect(host, port)) {
    Credence::KeyStash   keyStash;
    Credence::KnownKeys  knownKeys;
    if (peer.Authenticate(keyStash, knownKeys)) {
      rc = true;
    }
    else {
      peer.Disconnect();
      Syslog(LOG_ERR, "Failed to authenticate with %s", host.c_str());
    }
  }
  else {
    Syslog(LOG_ERR, "Failed to connect to %s port %hu", host.c_str(), port);
      peer.Disconnect();
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetPeer(const vector<string> & hosts, uint16_t port,
                    Credence::Peer & peer)
{
  bool  rc = false;
  for (const auto & host : hosts) {
    if (GetPeer(host, port, peer)) {
      rc = true;
      break;
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
PrintASPrefixesResponse(const Dwm::McCurtain::ASPrefixesResponse & resp,
                        bool verbose)
{
  if (verbose) {
    cout << setiosflags(ios::left) << setw(10) << std::get<0>(resp) << ' '
         << setw(2) << std::get<1>(resp).CountryCode() << ' '
         << std::get<1>(resp).Name() << '\n';
  }
  for (const auto & pfx : std::get<2>(resp)) {
    if (verbose) {
      cout << "  ";
    }
    cout << pfx.ToShortString() << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static vector<string> SplitArg(const string & arg)
{
  vector<string>  rc;
  size_t   startIndex = 0;
  size_t   foundIndex;
  while ((foundIndex = arg.find_first_of(',',startIndex)) != string::npos) {
    string  argpiece = arg.substr(startIndex, foundIndex - startIndex);
    if (! argpiece.empty()) {
      rc.push_back(arg.substr(startIndex, foundIndex - startIndex));
    }
    startIndex = foundIndex + 1;
  }
  if (startIndex < arg.size()) {
    string  argpiece = arg.substr(startIndex);
    if (! argpiece.empty()) {
      rc.push_back(arg.substr(startIndex));
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "Usage: " << argv0 << " [-d] [-h mccurtaind_host] [-p port] ipv4addr\n"
       << "       " << argv0 << " [-d] [-v] [-h mccurtaind_host] [-p port] AS_number\n"
       << "       " << argv0 << " -V\n\n"
       << "  Note: MCCURTAIND environment variable will be used if\n"
       << "        '-h mccurtaind_host' option is not specified.\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  extern int        optind;
  int               optChar;
  string            hostList;
  uint16_t          port = 2126;
  bool              verbose = false;
  
  Dwm::SysLogger::Open("mccurtain", LOG_PERROR, LOG_USER);
  Dwm::SysLogger::MinimumPriority(LOG_ERR);
  Dwm::SysLogger::ShowFileLocation(true);

  char *mccurtaindEnv = getenv("MCCURTAIND");
  if (nullptr != mccurtaindEnv) {
    hostList = mccurtaindEnv;
  }
  
  while ((optChar = getopt(argc, argv, "dh:p:vV")) != -1) {
    switch (optChar) {
      case 'd':
        Dwm::SysLogger::MinimumPriority(LOG_DEBUG);
        break;
      case 'h':
        hostList = optarg;
        break;
      case 'p':
        try {
          port = stoul(optarg, nullptr, 10);
        }
        catch (...) {
          cerr << "Invalid port " << optarg << '\n';
          Usage(argv[0]);
          return 1;
        }
        break;
      case 'v':
        verbose = true;
        break;
      case 'V':
        cout << Dwm::McCurtain::version_info.data_view() << '\n';
        return 0;
        break;
      default:
        Usage(argv[0]);
        return 1;
        break;
    }
  }

  if (hostList.empty()) {
    Usage(argv[0]);
    return 1;
  }

  vector<string>  hosts = SplitArg(hostList);
  
  if (optind >= argc) {
    Usage(argv[0]);
    return 1;
  }
  
  Credence::Peer  peer;
  if (GetPeer(hosts, port, peer)) {
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
            PrintASPrefixesResponse(resp, verbose);
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

