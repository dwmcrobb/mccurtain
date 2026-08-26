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
//!  @file DwmMcCurtainMessage.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <cerrno>

#if defined(__cpp_lib_spanstream)
#  if (__cpp_lib_spanstream >= 202106L)
#    if __has_include(<spanstream>)
#      include <spanstream>
#      define HAVE_STD_SPANSTREAM 1
#    endif
#  endif
#endif

#ifndef HAVE_STD_SPANSTREAM
#  include "DwmDnsSpanstream.hh"
#endif

#include "DwmFormatters.hh"
#include "DwmMclogLogger.hh"
#include "DwmMcCurtainMessage.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::istream & Message::Read(std::istream & is)
    {
      if (_header.Read(is)) {
        StreamIO::Read(is, _payload);
      }
      return is;
    }

    //------------------------------------------------------------------------
    std::ostream & Message::Write(std::ostream & os) const
    {
      if (_header.Write(os)) {
        StreamIO::Write(os, _payload);
      }
      return os;
    }

    //------------------------------------------------------------------------
    bool Message::FromJson(const nlohmann::json & j)
    {
      if (j.is_object()) {
        auto  it = j.find("hdr");
        if (j.end() != it) {
          if (_header.FromJson(*it)) {
            if (_header.Type()
                == MessageHeader::MsgType::e_typeOriginRequest) {
              it = j.find("req");
              if (j.end() != it) {
                OriginRequest  req;
                if (req.FromJson(*it)) {
                  _payload = req;
                  return true;
                }
              }
            }
            else if (_header.Type()
                     == MessageHeader::MsgType::e_typeOriginResponse) {
              it = j.find("resp");
              if (j.end() != it) {
                OriginResponse  resp;
                if (resp.FromJson(*it)) {
                  _payload = resp;
                  return true;
                }
              }
            }
          }
        }
      }
      return false;
    }

    //------------------------------------------------------------------------
    nlohmann::json Message::ToJson() const
    {
      nlohmann::json j;
      j["hdr"] = _header.ToJson();
      if (_payload.index() == 0) {
        j["req"] = std::get<0>(_payload).ToJson();
      }
      else if (_payload.index() == 1) {
        j["resp"] = std::get<1>(_payload).ToJson();
      }
      return j;
    }

    //------------------------------------------------------------------------
    ssize_t Message::SendTo(int fd, sockaddr_in *dest) const
    {
      ssize_t  rc = -1;
      if ((0 <= fd) && dest) {
        char  pktbuf[4096];
        std::spanstream  ss{std::span{pktbuf, sizeof(pktbuf)}};
        if (Write(ss)) {
          socklen_t  destlen = sizeof(*dest);
          size_t  buflen = ss.tellp();
          rc = sendto(fd, pktbuf, buflen, 0, (sockaddr *)dest, destlen);
          if (0 > rc) {
            MCLOG(LOG_ERR, "sendto({},{},{},0,{},{}) failed: {}",
                  fd, &pktbuf[0], buflen, *dest, destlen, strerror(errno));
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    ssize_t Message::SendTo(int fd, sockaddr_in6 *dest) const
    {
      ssize_t  rc = -1;
      if ((0 <= fd) && dest) {
        char  pktbuf[4096];
        std::spanstream  ss{std::span{pktbuf, sizeof(pktbuf)}};
        if (Write(ss)) {
          socklen_t  destlen = sizeof(*dest);
          size_t  buflen = ss.tellp();
          rc = sendto(fd, pktbuf, buflen, 0, (sockaddr *)dest, destlen);
          if (0 > rc) {
            MCLOG(LOG_ERR, "sendto({},{},{},0,{},{}) failed: {}",
                  fd, &pktbuf[0], buflen, *dest, destlen, strerror(errno));
          }
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    ssize_t Message::RecvFrom(int fd, sockaddr_in *src)
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        char       buf[4096];
        socklen_t  srclen = sizeof(*src);
        rc = recvfrom(fd, buf, sizeof(buf), 0, (sockaddr *)src, &srclen);
        if (0 < rc) {
          std::spanstream  ss{std::span{buf, (size_t)rc}};
          if (! Read(ss)) {
            rc = -1;
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    ssize_t Message::RecvFrom(int fd, sockaddr_in6 *src)
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        char       buf[4096];
        socklen_t  srclen = sizeof(*src);
        rc = recvfrom(fd, buf, sizeof(buf), 0, (sockaddr *)src, &srclen);
        if (0 < rc) {
          std::spanstream  ss{std::span{buf, (size_t)rc}};
          if (! Read(ss)) {
            rc = -1;
          }
        }
      }
      return rc;
    }
    
      
  }  // namespace McCurtain

}  // namespace Dwm
