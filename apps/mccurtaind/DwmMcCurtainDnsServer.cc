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
//!  @file DwmMcCurtainDnsServer.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/socket.h>
  #include <unistd.h>
}

#include <cstring>

#include "DwmMclogLogger.hh"
#include "DwmDnsUtils.hh"
#include "DwmMcCurtainDnsServer.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    static void SetRcvBuf(int fd)
    {
      if (0 <= fd) {
        std::vector<int>  bufsizes {
          1048576, 786432, 524288, 393216, 262144, 196608, 131072
        };
        int        bufsz;
        socklen_t  bufszlen = sizeof(bufsz);
        if (0 == getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsz, &bufszlen)) {
          for (int sz : bufsizes) {
            if (0 == setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(bufsz))) {
              bufsz = sz;
              break;
            }
          }
          setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufsz, sizeof(bufsz));
          MCLOG(Mclog::Severity::info, "DnsServer fd {} rcvbuf {}", fd, bufsz);
        }
        else {
          MCLOG(Mclog::Severity::err, "DnsServer getsockopt({},SOL_SOCKET,"
                " SO_RCVBUF) failed: {}", fd, strerror(errno));
        }
      }
      else {
        MCLOG(Mclog::Severity::err, "SetRcvBuf() called with"
              " invalid fd {}", fd);
      }
      return;
    }

    //------------------------------------------------------------------------
    DnsServer::DnsServer(const Ipv4Net2AS & ipv42as)
        : _v4fd(-1), _stopfds{-1,-1}, _thread(), _shouldRun(false),
          /* _running(false), */ _ipv42as(ipv42as)
    { }

    //------------------------------------------------------------------------
    DnsServer::~DnsServer()
    {
      Stop();
      if (0 <= _v4fd) {
        close(_v4fd);
        _v4fd = -1;
      }
    }

    //------------------------------------------------------------------------
    bool DnsServer::Start()
    {
      _shouldRun = true;
      if (0 == pipe(_stopfds)) {
        _thread = std::thread(&DnsServer::Run, this);
#if (defined(__FreeBSD__) || defined(__linux__))
        pthread_setname_np(_thread.native_handle(), "DnsServer");
#endif
        MCLOG(Mclog::Severity::info, "DnsServer started");
        return true;
      }
      else {
        MCLOG(Mclog::Severity::err,
              "DnsServer not started: pipe() failed ({})", strerror(errno));
      }
      return false;
    }

    //------------------------------------------------------------------------
    bool DnsServer::Stop()
    {
      _shouldRun = false;
      char  stop = 's';
      ::write(_stopfds[1], &stop, sizeof(stop));
      if (_thread.joinable()) {
        _thread.join();
        if (0 <= _stopfds[1]) { ::close(_stopfds[1]); _stopfds[1] = -1; }
        if (0 <= _stopfds[0]) { ::close(_stopfds[0]); _stopfds[0] = -1; }
        MCLOG(Mclog::Severity::info, "DnsServer stopped");
        return true;
      }
      return false;
    }
    
    //------------------------------------------------------------------------
    void DnsServer::Run()
    {
      _v4fd = OpenV4Socket();
      if (0 <= _v4fd) {
        fd_set        fds;
        int           maxfd;
        sockaddr_in   fromAddr;

        auto  reset_fds = [&] () -> void
        {
          FD_ZERO(&fds);
          maxfd = 0;
          if (0 <= _v4fd)  {
            FD_SET(_v4fd, &fds); maxfd = std::max({_v4fd, maxfd});
          }
          FD_SET(_stopfds[0], &fds);
          maxfd = std::max({_stopfds[0], maxfd}) + 1;
        };

        while (_shouldRun) {
          reset_fds();
          int  selectrc = select(maxfd, &fds, nullptr, nullptr, nullptr);
          if (0 < selectrc) {
            if ((0 <= _v4fd) && FD_ISSET(_v4fd, &fds)) {
              Dns::Message  message;
              sockaddr_in   fromAddr;
              socklen_t     fromLen;
              ssize_t  recvrc =
                message.RecvFrom(_v4fd, 0, (sockaddr *)&fromAddr, &fromLen);
              if (0 < recvrc) {
                SendResponse(_v4fd, message, fromAddr);
              }
            }
            else if (FD_ISSET(_stopfds[0], &fds)) {
              break;
            }
          }
        }
        if (0 <= _v4fd)  { ::close(_v4fd);  _v4fd = -1; }
      }
      return;
    }
    
    //------------------------------------------------------------------------
    int DnsServer::OpenV4Socket()
    {
      int  fd = socket(PF_INET, SOCK_DGRAM, 0);
      if (0 <= fd) {
        struct sockaddr_in  sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_addr.s_addr = INADDR_ANY;
        sockAddr.sin_port = htons(5353);
#ifndef __linux__
        sockAddr.sin_len = sizeof(sockAddr);
#endif
        if (0 == bind(fd, (sockaddr *)&sockAddr, sizeof(sockAddr))) {
          SetRcvBuf(fd);
        }
        else {
          close(fd);
          fd = -1;
        }
      }
      return fd;
    }

    //------------------------------------------------------------------------
    static void PopulateAnswer(Dns::Message & reply,
                               std::vector<Ipv4Net2AS::value_type> & matches)
    {
      std::vector<Dns::RRDataORIGIN::PrefixEntry>  pfxEntries;
      for (const auto & match : matches) {
        Dns::RRDataORIGIN::PrefixEntry  pfxEntry;
        pfxEntry.prefix = match.first;
        for (const auto & AS : match.second) {
          Dns::RRDataORIGIN::ASEntry  asentry{AS,{"--",""}};
          pfxEntry.asentries.push_back(asentry);
        }
        pfxEntries.push_back(pfxEntry);
      }
      Dns::RRDataORIGIN  origin(pfxEntries);
      Dns::ResourceRecord  origin_rr;
      origin_rr.Name(reply.Questions().front().QName());
      origin_rr.Data<Dns::RRDataORIGIN>(origin);
      origin_rr.TTL(4 * 3600);
      reply.Answers().push_back(origin_rr);
      return;
    }
    
    //------------------------------------------------------------------------
    void DnsServer::SendResponse(int fd, const Dns::Message & msg,
                                 sockaddr_in & addr)
    {
      if (0 > fd) { return; }
      if (msg.Header().IsResponse()
          || (1 != msg.Header().QuestionCount())
          || (1 != msg.Questions().size())) {
        return;
      }
      const auto & question = msg.Questions().front();
      if ((Dns::MessageQuestion::k_classIN == question.QClass())
          && (Dns::MessageQuestion::k_typeORIGIN == question.QType())) {
        std::string  ipAddrStr;
        if (Dns::FromArpa(question.QName(), ipAddrStr)) {
          Ipv4Address  ipv4Addr(ipAddrStr);
          std::vector<Ipv4Net2AS::value_type>  matches;
          if (_ipv42as.find_matches(ipv4Addr, matches)) {
            Dns::Message  reply = msg;
            reply.Header().IsResponse(true);
            PopulateAnswer(reply, matches);
            reply.SendTo(fd, 0, (const sockaddr *)&addr, sizeof(addr));
          }
          else {
            //  Send... NXDOMAIN?
          }
        }
        else {
          //  Send... ???
        }
      }
      else {
        //  Send... ???
      }
      
      return;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
