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
//!  \file mccurtaincgi.cc
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

#include "DwmCgi.hh"
#include "DwmMclogLogger.hh"
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
    Credence::KeyStash   keyStash("/home/www/.credence");
    Credence::KnownKeys  knownKeys("/home/www/.credence");
    if (peer.Authenticate(keyStash, knownKeys)) {
      rc = true;
    }
    else {
      peer.Disconnect();
      MCLOG(LOG_ERR, "Failed to authenticate with {}", host);
    }
  }
  else {
    MCLOG(LOG_ERR, "Failed to connect to {} port {}", host, port);
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
static std::string GetMyURI(Dwm::Cgi & cgi)
{
  auto & env = cgi.EnvValues();
  return std::string("https://") + env["SERVER_NAME"] + env["SCRIPT_NAME"];
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void
PrintIpv4AddrResponse(const string & myuri,
                      const Dwm::McCurtain::Ipv4AddrResponse & resp)
{
  cout << "<code>\n";
  for (const auto & entry : resp) {
    cout << std::get<0>(entry) << ":<br/>";
    for (const auto & ase : std::get<1>(entry)) {
      cout << "&nbsp;&nbsp;<a href=\"" << myuri << "?query=" << ase.first << "\">"
           << ase.first << "</a> " << ase.second.CountryCode() << ' '
           << ase.second.Name() << "<br/>";
    }
  }
  cout << "</code>\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void
PrintASPrefixesResponse(const string & myuri,
                        const Dwm::McCurtain::ASPrefixesResponse & resp)
{
  cout << "<code>\n";
  cout << std::get<0>(resp) << ' '
       << std::get<1>(resp).CountryCode() << ' '
       << std::get<1>(resp).Name() << "<br/>";
  for (const auto & pfx : std::get<2>(resp)) {
    cout << "&nbsp;&nbsp;<a href=\"" << myuri << "?query=" << pfx.Network()
         << "\">" << pfx.ToShortString() << "</a><br/>";
  }
  cout << "</code>\n";
  return;
}

//----------------------------------------------------------------------------
static void PrintPreamble()
{
  cout << "Content-type: text/html\n\n"
       << "<html>\n"
       << "<head>\n"
       << "  <style>\n"
       << "    th, td {\n"
       << "      padding-right: 20px;\n"
       << "    }\n"
       << "    .tblsubhdr {\n"
       << "      background-color: #FFFFA0;\n"
       << "    }\n"
       << "    .tblhosthdr {\n"
       << "      font-weight: bold;\n"
       << "      font-size: x-large;\n"
       << "      background-color: #B0D0FF;\n"
       << "    }\n"
       << "  </style>\n"
       << "</head>\n"
       << "<body>\n";
}

//----------------------------------------------------------------------------
static void PrintTrailer()
{
  cout << "</body>\n</html>\n";
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  uint16_t        port = 2126;
  Dwm::Cgi        cgi;
  Credence::Peer  peer;

  Dwm::Mclog::logger.Open("user");
  Dwm::Mclog::logger.LogLocations(true);

  MCLOG(LOG_INFO, "SERVER_PROTOCOL: {}", cgi.EnvValues()["SERVER_PROTOCOL"]);
  MCLOG(LOG_INFO, "SERVER_NAME: {}", cgi.EnvValues()["SERVER_NAME"]);
  MCLOG(LOG_INFO, "SCRIPT_NAME: {}", cgi.EnvValues()["SCRIPT_NAME"]);
  // MCLOG(LOG_INFO, "PATH_TRANSLATED: {}", cgi.EnvValues()["PATH_TRANSLATED"]);
  
  auto  queryit = cgi.QueryValues().find("query");
  if (queryit != cgi.QueryValues().end()) {
    if (GetPeer("ria", port, peer)) {
      std::string  query = queryit->second;
      if (query.find_first_of('.') != string::npos) {
        Dwm::McCurtain::Request  req{Dwm::Ipv4Address(query)};
        if (peer.Send(req)) {
          Dwm::McCurtain::Ipv4AddrResponse  resp;
          if (peer.Receive(resp)) {
            PrintPreamble();
            auto  myuri = GetMyURI(cgi);
            PrintIpv4AddrResponse(GetMyURI(cgi), resp);
            PrintTrailer();
            return 0;
          }
        }
      }
      else {
        try {
          uint32_t  asNum = stoul(query);
          Dwm::McCurtain::Request  req{asNum};
          if (peer.Send(req)) {
            Dwm::McCurtain::ASPrefixesResponse  resp;
            if (peer.Receive(resp)) {
              PrintPreamble();
              PrintASPrefixesResponse(GetMyURI(cgi), resp);
              PrintTrailer();
              return 0;
            }
          }
        }
        catch (...) {
        }
      }
    }
  }
  return 1;
}

