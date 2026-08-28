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
//!  \file DwmMcCurtainOriginServer.hh
//!  \author Daniel W. McRobb
//!  \brief NOT Dwm::McCurtain::OriginServer class declaration
//---------------------------------------------------------------------------

#include <atomic>
#include <variant>

#include "DwmMcCurtainMessage.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates an origin server from the perspective of a client.
    //!  Currently can only be used for IP -> origin queries.  Queries and
    //!  responses are sent via UDP, ununecrypted and unauthenticated.  This
    //!  is the fastest remote lookup machanism, due to a very small header
    //!  and binary transport.
    //------------------------------------------------------------------------
    class OriginServer
    {
    public:
      //----------------------------------------------------------------------
      //!  Construct for a server at @c host on port @c port.  Throws a
      //!  std::invalid_argument if @c host is not an IP address (v4 or v6)
      //!  and cannot be resolved as a host name.  Throws a std::system_error
      //!  if a UDP socket can not be opened.
      //----------------------------------------------------------------------
      OriginServer(const std::string & host, uint16_t port = 8645);
      
      //----------------------------------------------------------------------
      //!  Destrudtor.  Closes the encapsulated socket.
      //----------------------------------------------------------------------
      ~OriginServer();
      
      //----------------------------------------------------------------------
      //!  Requests origin information for @c addr, storing it in  @c orig.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool GetOrigin(const Ipv4Address & addr, OriginResponse & orig) const;
      
    private:
      IpAddress                               _address;
      uint16_t                                _port;
      std::variant<sockaddr_in,sockaddr_in6>  _srvSockAddr;
      int                                     _fd;

      static std::atomic<uint16_t>  _msgid;

      bool SendRequest(const Message & msg) const;
      bool RecvResponse(const Message & request,
                        OriginResponse & resp) const;
      bool SetAddress(const std::string & host);
      void SetUdpSockAddr();
    };
    
  }  // namespace McCurtain

}  // namespace Dwm
