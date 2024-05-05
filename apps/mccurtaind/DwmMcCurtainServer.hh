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
//!  \file DwmMcCurtainServer.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Server class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINSERVER_HH_
#define _DWMMCCURTAINSERVER_HH_

#include "DwmCredenceKeyStash.hh"
#include "DwmCredenceKnownKeys.hh"
#include "DwmMcCurtainAS2Ipv4NetDb.hh"
#include "DwmMcCurtainIpv4Net2ASDb.hh"
#include "DwmMcCurtainConfig.hh"
#include "DwmMcCurtainResponses.hh"
#include "DwmMcCurtainResponder.hh"
#include "DwmMcCurtainRipeAsnTxt.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Server
    {
    public:
      //----------------------------------------------------------------------
      //!  Construct from a configuration @c config.
      //----------------------------------------------------------------------
      template <typename Ex>
      Server(Ex executor, const Config & config)
          : _config(config), _keyStash(config.Service().KeyDirectory()),
            _knownKeys(config.Service().KeyDirectory()),
            _allowedClients(config.Service().AllowedClients()),
            _acceptors(), _responders()
      {
        namespace ip = boost::asio::ip;
        using boost::asio::ip::tcp;

        InitDatabases();
        
        for (auto & ep : config.Service().Addresses()) {
          ip::tcp::acceptor  acc(executor);
          if (ep.address().is_v6()) {
            acc.open(ip::tcp::v6());
            acc.set_option(ip::v6_only(true));
          }
          else {
            acc.open(ip::tcp::v4());
          }
          ip::tcp::acceptor::reuse_address option(true);
          acc.set_option(option);
          acc.bind(ep);
          auto & a = _acceptors.emplace_back(std::move(acc));
          a.listen();
          AcceptLoop(a);
        }
      }

      void GetIpv4AddrResponse(const Ipv4Address & addr,
                               Ipv4AddrResponse & resp);
      void GetASPrefixesResponse(uint32_t asNum, ASPrefixesResponse & resp);

      const Credence::KeyStash & GetKeyStash() const  { return _keyStash; }
      const Credence::KnownKeys & GetKnownKeys() const  { return _knownKeys; }
      const Config GetConfig() const  { return _config; }

      //----------------------------------------------------------------------
      //!  Stops the server.
      //----------------------------------------------------------------------
      void Stop();

    private:
      Config                                      _config;
      Ipv4Net2ASDb                                _ipv42asdb;
      AS2Ipv4NetDb                                _as2ipv4db;
      RipeAsnTxt                                  _asntxt;
      Credence::KeyStash                          _keyStash;
      Credence::KnownKeys                         _knownKeys;
      std::set<IpPrefix>                          _allowedClients;
      std::deque<boost::asio::ip::tcp::acceptor>  _acceptors;
      std::atomic<bool>                           _run;
      std::vector<std::shared_ptr<Responder>>     _responders;

      bool InitDatabases();
      void AcceptLoop(boost::asio::ip::tcp::acceptor & a);
      bool ClientAllowed(const boost::asio::ip::address & epAddr) const;
      void CleanupResponders();
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINSERVER_HH_
