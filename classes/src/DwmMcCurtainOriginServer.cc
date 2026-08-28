//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmMcCurtainOriginServer.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <stdexcept>
#include <system_error>

#include "DwmDnsResolver.hh"
#include "DwmMcCurtainOriginServer.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::atomic<uint16_t>  OriginServer::_msgid = 0;
    
    //------------------------------------------------------------------------
    OriginServer::OriginServer(const std::string & host, uint16_t port)
        : _port(port), _fd(-1)
    {
      if (! SetAddress(host)) {
        std::string  what("Invalid server host '" + host + "'");
        throw std::invalid_argument(what);
      }
      if (_address.IsV4()) {
        _fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (0 > _fd) {
          throw std::system_error(errno, std::generic_category(),
                                  strerror(errno));
        }
      }
      else if (_address.IsV6()) {
        _fd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (0 > _fd) {
          throw std::system_error(errno, std::generic_category(),
                                  strerror(errno));
        }
      }
    }

    //------------------------------------------------------------------------
    OriginServer::~OriginServer()
    {
      if (0 <= _fd) {
        ::close(_fd);
        _fd = -1;
      }
    }

    //------------------------------------------------------------------------
    bool OriginServer::GetOrigin(const Ipv4Address & addr,
                                 OriginResponse & orig) const
    {
      if (0 <= _fd) {
        Message  msg;
        msg.Header().Type(MessageHeader::MsgType::e_typeOriginRequest);
        msg.Header().Id(_msgid++);
        msg.OrigRequest(OriginRequest(addr));
        if (SendRequest(msg)) {
          if (RecvResponse(msg, orig)) {
            return true;
          }
        }
      }
      return false;
    }

    //------------------------------------------------------------------------
    void OriginServer::SetUdpSockAddr()
    {
      if (_address.IsV4()) {
        sockaddr_in  inAddr;
        memset(&inAddr, 0, sizeof(inAddr));
        inAddr.sin_addr.s_addr = _address.Addr<Ipv4Address>()->Raw();
        inAddr.sin_port = htons(_port);
        inAddr.sin_family = AF_INET;
#ifndef __linux__
        inAddr.sin_len = sizeof(inAddr);
#endif
        _srvSockAddr = inAddr;
      }
      else if (_address.IsV6()) {
        sockaddr_in6  in6Addr;
        memset(&in6Addr, 0, sizeof(in6Addr));
        in6Addr.sin6_addr = *(_address.Addr<Ipv6Address>());
        in6Addr.sin6_port = htons(_port);
        in6Addr.sin6_family = AF_INET;
#ifndef __linux__
        in6Addr.sin6_len = sizeof(in6Addr);
#endif
        _srvSockAddr = in6Addr;
      }
      
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool OriginServer::SendRequest(const Message & msg) const
    {
      if (_address.IsV4()) {
        return msg.SendTo(_fd, std::get_if<sockaddr_in>(&_srvSockAddr));
      }
      else {
        return msg.SendTo(_fd, std::get_if<sockaddr_in6>(&_srvSockAddr));
      }
    }

    //------------------------------------------------------------------------
    bool OriginServer::RecvResponse(const Message & request,
                                    OriginResponse & resp) const
    {
      if (0 <= _fd) {
        fd_set  fds;
        FD_ZERO(&fds);
        FD_SET(_fd, &fds);
        timeval  timeout{2, 0};
        if (select(_fd+1, &fds, nullptr, nullptr, &timeout) > 0) {
          Message  msg;
          ssize_t  recvrc = -1;
          if (_address.IsV4()) {
            sockaddr_in  inAddr;
            recvrc = msg.RecvFrom(_fd, &inAddr);
#if 0
            if (0 < recvrc) {
              //  TODO: should check source address and port
              //  if (inAddr is invalid source) {
              //    return false;
              //  }
            }
#endif
          }
          else if (_address.IsV6()) {
            sockaddr_in6  fromAddr;
            recvrc = msg.RecvFrom(_fd, &fromAddr);
#if 0
            if (0 < recvrc) {
              //  TODO: should check source address and port
              //  if (fromAddr is invalid source) {
              //    return false;
              //  }
            }
#endif
          }
          if (0 < recvrc) {
            if ((msg.Header().Type()
                 == MessageHeader::MsgType::e_typeOriginResponse)
                && (msg.Header().Id() == request.Header().Id())) {
              resp = *(msg.OrigResponse());
              return true;
            }
          }
        }
      }
      return false;
    }
  
    //------------------------------------------------------------------------
    bool OriginServer::SetAddress(const std::string & host)
    {
      bool  rc = false;
      
      in_addr  inAddr;
      if (inet_pton(AF_INET, host.c_str(), &inAddr) == 1) {
        _address = Ipv4Address(inAddr.s_addr);
        SetUdpSockAddr();
        rc = true;
      }
      else {
        in6_addr  inAddr6;
        if (inet_pton(AF_INET6, host.c_str(), &inAddr6) == 1) {
          _address = Ipv6Address(inAddr6);
          SetUdpSockAddr();
          rc = true;
        }
        else {
          Dns::Resolver  resolver;
          std::vector<in6_addr>  in6Addrs;
          std::vector<in_addr>   inAddrs;
          if (resolver.GetHostByName(host, in6Addrs, inAddrs)) {
            if (! inAddrs.empty()) {
              _address = Ipv4Address(inAddrs.front().s_addr);
              SetUdpSockAddr();
              rc = true;
            }
            else if (! in6Addrs.empty()) {
              _address = Ipv6Address(in6Addrs.front());
              SetUdpSockAddr();
              rc = true;
            }
          }
        }
      }
      return rc;
    }

    
      
  }  // namespace McCurtain

}  // namespace Dwm
