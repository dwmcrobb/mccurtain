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

#include "DwmMcCurtainMessage.hh"
#include "DwmMcCurtainUdpServer.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    UdpServer::UdpServer(const Ipv4Net2AS & ipv42as, const RipeAsnTxt & asntxt)
        : _ipv42as(ipv42as), _asntxt(asntxt), _binfd(-1), _jsonfd(-1),
          _bin6fd(-1), _json6fd(-1), _stopfds{-1,-1}, _thread(),
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
        maxfd = 0;
        if (0 <= _binfd)       { FD_SET(_binfd, &fds); }
        if (0 <= _jsonfd)      { FD_SET(_jsonfd, &fds); }
        if (0 <= _bin6fd)      { FD_SET(_bin6fd, &fds); }
        if (0 <= _json6fd)     { FD_SET(_json6fd, &fds); }
        if (0 <= _stopfds[0])  { FD_SET(_stopfds[0], &fds); }
        maxfd = std::max({_binfd,_jsonfd,_bin6fd,_json6fd,_stopfds[0]});
      };
      
      while (_shouldRun) {
        reset_fds();
        int  selectrc = select(maxfd+1, &fds, nullptr, nullptr, nullptr);
        if (FD_ISSET(_stopfds[0], &fds))  { break; }
        if (FD_ISSET(_binfd, &fds))       { RespondBinary(_binfd); }
        if (FD_ISSET(_bin6fd, &fds))      { RespondBinary6(_bin6fd); }
        if (FD_ISSET(_jsonfd, &fds))      { RespondJson(_jsonfd); }
        if (FD_ISSET(_json6fd, &fds))     { RespondJson6(_json6fd); }
      }
      
      return;
    }
    
    //------------------------------------------------------------------------
    bool UdpServer::BindSockets()
    {
      if ((0 <= _binfd)
          && (0 <= _jsonfd)
          && (0 <= _bin6fd)
          && (0 <= _json6fd)) {
        sockaddr_in  sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = PF_INET;
        sockAddr.sin_addr.s_addr = INADDR_ANY;
        sockAddr.sin_port = htons(8645);
#ifndef __linux__
        sockAddr.sin_len = sizeof(sockAddr);
#endif
        if (0 == ::bind(_binfd, (sockaddr *)&sockAddr, sizeof(sockAddr))) {
          sockAddr.sin_port = htons(8647);
          if (0 == ::bind(_jsonfd, (sockaddr *)&sockAddr, sizeof(sockAddr))) {
            sockaddr_in6  sockAddr6;
            memset(&sockAddr6, 0, sizeof(sockAddr6));
            sockAddr6.sin6_family = PF_INET6;
            memset(&sockAddr6.sin6_addr, 0, sizeof(sockAddr6.sin6_addr));
            sockAddr6.sin6_port = htons(8645);
#ifndef __linux__
            sockAddr6.sin6_len = sizeof(sockAddr6);
#endif
            if (0 == ::bind(_bin6fd, (sockaddr *)&sockAddr6,
                            sizeof(sockAddr6))) {
              sockAddr6.sin6_port = htons(8647);
              if (0 == ::bind(_json6fd, (sockaddr *)&sockAddr6,
                            sizeof(sockAddr6))) {
                return true;
              }
            }
          }
        }
        CloseSockets();
      }
      return false;
    }
    
    //------------------------------------------------------------------------
    bool UdpServer::OpenSockets()
    {
      CloseSockets();
      _binfd = socket(PF_INET, SOCK_DGRAM, 0);
      if (0 <= _binfd) {
        _jsonfd = socket(PF_INET, SOCK_DGRAM, 0);
        if (0 <= _jsonfd) {
          _bin6fd = socket(PF_INET6, SOCK_DGRAM, 0);
          if (0 <= _bin6fd) {
            _json6fd = socket(PF_INET6, SOCK_DGRAM, 0);
            if (0 <= _json6fd) {
              return true;
            }
          }
        }
      }
      CloseSockets();
      return false;
    }
    
    //------------------------------------------------------------------------
    void UdpServer::CloseSockets()
    {
      if (0 <= _binfd)        { ::close(_binfd);   _binfd   = -1; }
      if (0 <= _jsonfd)       { ::close(_jsonfd);  _jsonfd  = -1; }
      if (0 <= _bin6fd)       { ::close(_bin6fd);  _bin6fd  = -1; }
      if (0 <= _json6fd)      { ::close(_json6fd); _json6fd = -1; }
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
    bool UdpServer::GetResponse(const OriginRequest & req,
                                OriginResponse & resp)
    {
      bool  rc = false;
      std::vector<Ipv4Net2AS::value_type>  matches;
      if (req.Address().IsV4()) {
        if (_ipv42as.find_matches(*(req.Address().Addr<Ipv4Address>()),
                                  matches)) {
          std::vector<OriginPrefix>  prefixes;
          for (const auto & match : matches) {
            OriginPrefix  prefix;
            prefix.Prefix(match.first);
            for (const auto & as : match.second) {
              OriginAS  origas(as,"--","");
              auto  asnit = _asntxt.Entries().find(as);
              if (asnit != _asntxt.Entries().end()) {
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
      }
      return rc;
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
