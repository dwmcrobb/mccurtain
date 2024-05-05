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
//!  \file DwmMcCurtainServer.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Server class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <thread>

#include "DwmSysLogger.hh"
#include "DwmMcCurtainServer.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::Stop()
    {
      for (auto & a : _acceptors) {
        a.cancel();
      }
      Syslog(LOG_INFO, "Server stopped");
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::AcceptLoop(boost::asio::ip::tcp::acceptor & a)
    {
      using boost::asio::ip::tcp;
      
      a.async_accept(a.get_executor(),
                     [this,&a](error_code ec, tcp::socket && s)
                     {
                       CleanupResponders();
                       if (! ec) {
                         if (ClientAllowed(s.remote_endpoint().address())) {
                           _responders.push_back(std::make_shared<Responder>(std::move(s), *this));
                         }
                         else {
                           s.close();
                         }
                         AcceptLoop(a);
                       }
                       else {
                         Syslog(LOG_ERR, "Exiting AcceptLoop()");
                       }
                     });
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Server::ClientAllowed(const boost::asio::ip::address & epAddr) const
    {
      bool  rc = _allowedClients.empty();
      if (! rc) {
        IpAddress clientAddr(epAddr.to_string());
        auto  it = std::find_if(_allowedClients.begin(),
                                _allowedClients.end(),
                                [&] (const auto & ac)
                                { return ac.Contains(clientAddr); });
        rc = (it != _allowedClients.end());
      }
      return rc;
    }
                                  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::CleanupResponders()
    {
      for (auto ri = _responders.begin(); ri != _responders.end(); ) {
        if ((*ri)->Join()) {
          ri = _responders.erase(ri++);
        }
        else {
          ++ri;
        }
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::GetIpv4AddrResponse(const Ipv4Address & addr,
                                     Ipv4AddrResponse & resp)
    {
      resp.clear();
      std::vector<std::pair<Dwm::Ipv4Prefix,set<uint32_t>>>  matches;
      if (_ipv42asdb.Entries().Find(addr, matches)) {
        for (const auto & match : matches) {
          Ipv4AddrResponseEntry  entry;
          get<0>(entry) = match.first;
          for (const auto & as : match.second) {
            auto  asit = _asntxt.Entries().find(as);
            if (asit != _asntxt.Entries().end()) {
              get<1>(entry).push_back(*asit);
            }
            else {
              pair<uint32_t,RipeAsnTxt::Entry>
                asnoinfo(as, RipeAsnTxt::Entry("",""));
              get<1>(entry).push_back(asnoinfo);
            }
          }
          resp.push_back(entry);
        }
      }
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Server::GetASPrefixesResponse(uint32_t asNum,
                                       ASPrefixesResponse & resp)
    {
      std::get<0>(resp) = asNum;
      std::get<1>(resp).Clear();
      std::get<2>(resp).clear();

      auto  asnit = _asntxt.Entries().find(asNum);
      if (asnit != _asntxt.Entries().end()) {
        std::get<1>(resp) = asnit->second;
      }
      auto  asit = _as2ipv4db.Nets().find(asNum);
      if (asit != _as2ipv4db.Nets().end()) {
        auto  netsCopy = asit->second;
        netsCopy.Coalesce();
        std::vector<std::pair<Dwm::Ipv4Prefix,uint8_t>>  prefixes;
        netsCopy.SortByKey(prefixes);
        for (const auto & pfx : prefixes) {
          std::get<2>(resp).push_back(pfx.first);
        }
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Server::InitDatabases()
    {
      bool  rc = false;
      if (_ipv42asdb.Load("/usr/local/etc/ipv42as.db")) {
        if (_as2ipv4db.Load("/usr/local/etc/as2ipv4.db")) {
          if (_asntxt.Load("/usr/local/etc/asn.txt")) {
            rc = true;
          }
          else {
            Syslog(LOG_ERR, "Failed to load asn.txt");
          }
        }
        else {
          Syslog(LOG_ERR, "Failed to load as2ipv4.db");
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to load ipv42as.db");
      }
      return rc;
    }
    
    
  }  // namespace McCurtain

}  // namespace Dwm
