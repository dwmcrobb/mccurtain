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
//!  @file DwmMcCurtainUdpServer.hh
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINUDPSERVER_HH_
#define _DWMMCCURTAINUDPSERVER_HH_

#include "DwmMcCurtainAS2Ipv4Net.hh"
#include "DwmMcCurtainRipeAsnTxt.hh"

namespace Dwm {

  namespace McCurtain {

    class UdpServer
    {
    public:
      UdpServer() = delete;
      UdpServer(const Ipv4Net2AS & ipv42as, const RipeAsnTxt & asntxt);
      ~UdpServer();
      bool Start();
      bool Stop();
      
    private:
      const Ipv4Net2AS  & _ipv42as;
      const RipeAsnTxt  & _asntxt;
      int                 _binfd;
      int                 _jsonfd;
      int                 _bin6fd;
      int                 _json6fd;
      int                 _stopfds[2];

      bool BindSockets();
      bool OpenSockets();
      void CloseSockets();
      void CloseStopPipe();

      bool GetResponse(const OriginRequest & req, OriginResponse & resp);

      template <typename AT>
      void RespondBinary(int fd)
      {
        if (0 <= fd) {
          Message  msg;
          AT       sockAddr;
          if (msg.RecvFrom(fd, &sockAddr)) {
            if (msg.Header().Type()
                == MessageHeader::MsgType::e_typeOriginRequest) {
              auto  *req = msg.OrigRequest();
              if (req) {
                OriginResponse  resp;
                if (GetResponse(*req, resp)) {
                  msg.OrigResponse(resp);
                  msg.Header().Type(MessageHeader::MsgType::e_typeOriginResponse);
                  msg.SendTo(fd, &sockAddr);
                }
              }
            }
          }
        }
        return;
      }

      template <typename AT>
      void RespondJson(int fd)
      {
        if (0 <= fd) {
          Message  msg;
          AT       sockAddr;
          if (msg.RecvFrom(fd, &sockAddr)) {
            if (msg.Header().Type()
                == MessageHeader::MsgType::e_typeOriginRequest) {
              auto  *req = msg.OrigRequest();
              if (req) {
                OriginResponse  resp;
                if (GetResponse(*req, resp)) {
                  msg.OrigResponse(resp);
                  msg.Header().Type(MessageHeader::MsgType::e_typeOriginResponse);
                  msg.SendTo(fd, &sockAddr);
                }
              }
            }
          }
        }
        return;
      }
      
      void RespondBinary(int fd);
      void RespondJson(int fd);
      void RespondBinary6(int fd);
      void RespondJson6(int fd);
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINUDPSERVER_HH_
