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
//!  @file DwmMcCurtainMessage.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::McCurtain::Message class definition
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINMESSAGE_HH_
#define _DWMMCCURTAINMESSAGE_HH_

extern "C" {
  #include <sys/socket.h>
}

#include <variant>

#include "DwmMcCurtainMessageHeader.hh"
#include "DwmMcCurtainOriginResponse.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates request and response messages.  Currently only used
    //!  with UDP requests and responses for IP -> origin queries.
    //------------------------------------------------------------------------
    class Message
    {
    public:
      //----------------------------------------------------------------------
      //!  Returns the encapsulated header (const).
      //----------------------------------------------------------------------
      const MessageHeader & Header() const  { return _header; }

      //----------------------------------------------------------------------
      //!  Returns a mutable reference to the encapsulated header.
      //----------------------------------------------------------------------
      MessageHeader & Header()  { return _header; }
        
      //----------------------------------------------------------------------
      //!  Sets and returns the encapsulated header.
      //----------------------------------------------------------------------
      const MessageHeader & Header(const MessageHeader & header)
      { return _header = header; }

      //----------------------------------------------------------------------
      //!  If the message contains an OriginRequest, returns a pointer to it.
      //!  Else returns nullptr.
      //----------------------------------------------------------------------
      const OriginRequest *OrigRequest() const
      { return std::get_if<OriginRequest>(&_payload); }

      //----------------------------------------------------------------------
      //!  Sets the contained OriginRequest and returns a pointer to it.
      //----------------------------------------------------------------------
      const OriginRequest *OrigRequest(const OriginRequest & req)
      {
        _payload = req;
        _header.Type(MessageHeader::MsgType::e_typeOriginRequest);
        return OrigRequest();
      }
      
      //----------------------------------------------------------------------
      //!  If the message contains an OriginResponse, returns a pointer to it.
      //!  Else returns nullptr.
      //----------------------------------------------------------------------
      const OriginResponse *OrigResponse() const
      { return std::get_if<OriginResponse>(&_payload); }

      //----------------------------------------------------------------------
      //!  Sets the contained OriginResponse and returns a pointer to it.
      //----------------------------------------------------------------------
      const OriginResponse *OrigResponse(const OriginResponse & resp)
      {
        _payload = resp;
        _header.Type(MessageHeader::MsgType::e_typeOriginResponse);
        return OrigResponse();
      }
      
      //----------------------------------------------------------------------
      //!  Reads the Message from an istream.  Returns the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  Writes the Message to an ostream.  Returns the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  Populates the Message from the given json @c j.  Returns true on
      //!  success, false on failure.
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  Returns a json representation of the Message.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;
      
      //----------------------------------------------------------------------
      //!  Sends the Message to the given destination @c dest via the given
      //!  descriptor @c fd.  Returns the number of bytes sent on success, -1
      //!  on failure.
      //----------------------------------------------------------------------
      ssize_t SendTo(int fd, const sockaddr_in *dest) const;

      //----------------------------------------------------------------------
      //!  Sends the Message to the given destination @c dest via the given
      //!  descriptor @c fd.  Returns the number of bytes sent on success, -1
      //!  on failure.
      //----------------------------------------------------------------------
      ssize_t SendTo(int fd, const sockaddr_in6 *dest) const;
      
      //----------------------------------------------------------------------
      //!  Receives the Message via the given descriptor @c fd and populates
      //!  @c src with the address of the sender.  Returns the number of
      //!  bytes read on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t RecvFrom(int fd, sockaddr_in *src);

      //----------------------------------------------------------------------
      //!  Receives the Message via the given descriptor @c fd and populates
      //!  @c src with the address of the sender.  Returns the number of
      //!  bytes read on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t RecvFrom(int fd, sockaddr_in6 *src);

      //----------------------------------------------------------------------
      //!  Sends the Message to @c dest via @c fd in json form.  Returns the
      //!  number of bytes sent on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t SendJsonTo(int fd, const sockaddr_in *dest) const;

      //----------------------------------------------------------------------
      //!  Sends the Message to @c dest via @c fd in json form.  Returns the
      //!  number of bytes sent on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t SendJsonTo(int fd, const sockaddr_in6 *dest) const;
      
      //----------------------------------------------------------------------
      //!  Receives the Message in json form via @c fd and populaes @c src
      //!  with the address of the sender.  Returns the number of bytes
      //!  received on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t RecvJsonFrom(int fd, sockaddr_in *src);

      //----------------------------------------------------------------------
      //!  Receives the Message in json form via @c fd and populaes @c src
      //!  with the address of the sender.  Returns the number of bytes
      //!  received on success, -1 on failure.
      //----------------------------------------------------------------------
      ssize_t RecvJsonFrom(int fd, sockaddr_in6 *src);
      
      //----------------------------------------------------------------------
      //!  equality operator
      //----------------------------------------------------------------------
      bool operator == (const Message &) const = default;
      
    private:
      MessageHeader                               _header;
      std::variant<OriginRequest,OriginResponse>  _payload;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINMESSAGE_HH_
