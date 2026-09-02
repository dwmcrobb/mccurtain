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
//!  @file DwmMcCurtainUdpServer.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/select.h>
}

#include "DwmMclogLogger.hh"
#include "DwmMcCurtainMessage.hh"
#include "DwmMcCurtainServer.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    UdpServer::UdpServer(Server & server)
        : _server(server), _binfds(), _stopfds{-1,-1}, _thread(),
          _shouldRun(false)
    {}

    //------------------------------------------------------------------------
    UdpServer::~UdpServer()
    {
      Stop();
    }

    //------------------------------------------------------------------------
    bool UdpServer::Start()
    {
      bool  rc = false;
      if (0 == ::pipe(_stopfds)) {
        if (OpenSockets()) {
          if (BindSockets()) {
            _shouldRun = true;
            _thread = std::thread(&UdpServer::Run, this);
            rc = true;
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    bool UdpServer::Stop()
    {
      bool  rc = false;
      _shouldRun = false;
      char  stop = 's';
      ::write(_stopfds[1], &stop, sizeof(stop));
      if (_thread.joinable()) {
        _thread.join();
        CloseStopPipe();
        CloseSockets();
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    void UdpServer::Run()
    {
      fd_set        fds;
      int           maxfd;
      auto reset_fds = [&] () -> void
      {
        FD_ZERO(&fds);
        for (auto & binfd : _binfds) {
          if (0 <= binfd.second) {
            FD_SET(binfd.second, &fds);
          }
        }
        auto maxit = std::max_element(_binfds.begin(), _binfds.end(),
                                      [] (const auto & a, const auto & b)
                                      { return a.second < b.second; });
        maxfd = maxit->second;
        if (0 <= _stopfds[0])  { FD_SET(_stopfds[0], &fds); }
        maxfd = std::max({maxfd, _stopfds[0]});
      };
      
      while (_shouldRun) {
        reset_fds();
        int  selectrc = select(maxfd+1, &fds, nullptr, nullptr, nullptr);
        if (FD_ISSET(_stopfds[0], &fds))  { break; }
        for (auto & binfd : _binfds) {
          if (FD_ISSET(binfd.second, &fds)) {
            if (binfd.first.addr.IsV4()) {
              RespondBinary(binfd.second);
            }
            else if (binfd.first.addr.IsV6()) {
              RespondBinary6(binfd.second);
            }
          }
        }
      }
      
      return;
    }
    
    //------------------------------------------------------------------------
    bool UdpServer::BindSockets()
    {
      size_t  numbound = 0;
      for (auto & binfd : _binfds) {
        if (0 <= binfd.second) {
          if (binfd.first.addr.IsV4()) {
            sockaddr_in  sockAddr;
            memset(&sockAddr, 0, sizeof(sockAddr));
            sockAddr.sin_family = PF_INET;
            sockAddr.sin_addr.s_addr = binfd.first.addr.Addr<Ipv4Address>()->Raw();
            sockAddr.sin_port = htons(binfd.first.port);
#ifndef __linux__
            sockAddr.sin_len = sizeof(sockAddr);
#endif
            if (0 == ::bind(binfd.second, (sockaddr *)&sockAddr, sizeof(sockAddr))) {
              ++numbound;
            }
            else {
              MCLOG(LOG_ERR, "Failed to bind fd {} to {}: {}",
                    binfd.second, binfd.first.addr, strerror(errno));
            }
          }
          else if (binfd.first.addr.IsV6()) {
            sockaddr_in6  sockAddr6;
            memset(&sockAddr6, 0, sizeof(sockAddr6));
            sockAddr6.sin6_family = PF_INET6;
            sockAddr6.sin6_addr = *(binfd.first.addr.Addr<Ipv6Address>());
            sockAddr6.sin6_port = htons(binfd.first.port);
#ifndef __linux__
            sockAddr6.sin6_len = sizeof(sockAddr6);
#endif
            if (0 == ::bind(binfd.second, (sockaddr *)&sockAddr6, sizeof(sockAddr6))) {
              ++numbound;
            }
            else {
              MCLOG(LOG_ERR, "Failed to bind fd {} to {}: {}",
                    binfd.second, binfd.first.addr, strerror(errno));
            }
          }
        }
      }
      if (_binfds.size() == numbound) {
        return true;
      }
      CloseSockets();
      return false;
    }
    
    //------------------------------------------------------------------------
    bool UdpServer::OpenSockets()
    {
      CloseSockets();
      const auto & addrs = _server.GetConfig().Service().UdpAddresses();
      for (const auto & ba : addrs) {
        int  fd = -1;
        if (ba.addr.IsV4()) { fd = socket(PF_INET, SOCK_DGRAM, 0); }
        else if (ba.addr.IsV6()) { fd = socket(PF_INET6, SOCK_DGRAM, 0); }
        if (0 <= fd) {
          _binfds[ba] = fd;
        }
        else {
          CloseSockets();
          return false;
        }
      }
      return (_binfds.size() == addrs.size());
      return true;
    }
    
    //------------------------------------------------------------------------
    void UdpServer::CloseSockets()
    {
      for (auto & binfd : _binfds) {
        if (0 <= binfd.second) {
          ::close(binfd.second);
        }
      }
      _binfds.clear();
      return;
    }
      
    //------------------------------------------------------------------------
    void UdpServer::CloseStopPipe()
    {
      if (0 <= _stopfds[1]) {  ::close(_stopfds[1]); _stopfds[1] = -1; }
      if (0 <= _stopfds[0]) {  ::close(_stopfds[0]); _stopfds[0] = -1; }
      return;
    }

    //------------------------------------------------------------------------
    bool UdpServer::GetV6AddrResponse(const OriginRequest & req,
                                      OriginResponse & resp)
    {
      bool  rc = false;
      std::vector<Ipv6Net2AS::value_type>  matches;
      if (_server.Ip6ToAS().find_matches(*(req.Address().Addr<Ipv6Address>()),
                                         matches)) {
        MCLOG(LOG_INFO, "Found {} matches for {}",
              matches.size(), *(req.Address().Addr<Ipv6Address>()));
        std::vector<OriginPrefix>  prefixes;
        for (const auto & match : matches) {
          OriginPrefix  prefix;
          prefix.Prefix(match.first);
          for (const auto & as : match.second) {
            OriginAS  origas(as,"--","");
            auto  asnit = _server.AsnTxt().Entries().find(as);
            if (asnit != _server.AsnTxt().Entries().end()) {
              origas.CountryCode(asnit->second.CountryCode());
              origas.Name(asnit->second.Name());
            }
            prefix.ASes().push_back(origas);
          }
          prefixes.push_back(prefix);
        }
        resp.Prefixes(prefixes);
        resp.Request(req);
        rc = true;
      }
      else {
        MCLOG(LOG_ERR, "No matches for {}",
              *(req.Address().Addr<Ipv6Address>()));
      }
      
      return rc;
    }

    //------------------------------------------------------------------------
    bool UdpServer::GetV4AddrResponse(const OriginRequest & req,
                                      OriginResponse & resp)
    {
      bool  rc = false;
      std::vector<Ipv4Net2AS::value_type>  matches;
      if (_server.Ip4ToAS().find_matches(*(req.Address().Addr<Ipv4Address>()),
                                         matches)) {
        std::vector<OriginPrefix>  prefixes;
        for (const auto & match : matches) {
          OriginPrefix  prefix;
          prefix.Prefix(match.first);
          for (const auto & as : match.second) {
            OriginAS  origas(as,"--","");
            auto  asnit = _server.AsnTxt().Entries().find(as);
            if (asnit != _server.AsnTxt().Entries().end()) {
              origas.CountryCode(asnit->second.CountryCode());
              origas.Name(asnit->second.Name());
            }
            prefix.ASes().push_back(origas);
          }
          prefixes.push_back(prefix);
        }
        resp.Prefixes(prefixes);
        resp.Request(req);
        rc = true;
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool UdpServer::GetResponse(const OriginRequest & req,
                                OriginResponse & resp)
    {
      if (req.Address().IsV6()) {
        return GetV6AddrResponse(req, resp);
      }
      else if (req.Address().IsV4()) {
        return GetV4AddrResponse(req, resp);
      }
      return false;
    }
    
    //------------------------------------------------------------------------
    void UdpServer::RespondBinary(int fd)
    {
      return RespondBinary<sockaddr_in>(fd);
    }

    //------------------------------------------------------------------------
    void UdpServer::RespondBinary6(int fd)
    {
      return RespondBinary<sockaddr_in6>(fd);
    }

    //------------------------------------------------------------------------
    void UdpServer::RespondJson(int fd)
    {
      return RespondJson<sockaddr_in>(fd);
    }

    //------------------------------------------------------------------------
    void UdpServer::RespondJson6(int fd)
    {
      return RespondJson<sockaddr_in6>(fd);
    }
    
  
  }  // namespace McCurtain

}  // namespace Dwm
