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
//!  @file DwmMcCurtainMessageHeader.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::McCurtain::MessageHeader class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINMESSAGEHEADER_HH_
#define _DWMMCCURTAINMESSAGEHEADER_HH_

#include <iostream>
#include <nlohmann/json.hpp>

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates the Message header used for IP -> origin requests and
    //!  responses.  Currently only used for UDP messaging.
    //------------------------------------------------------------------------
    class MessageHeader
    {
    public:
      static constexpr uint16_t  k_versionMask   = 0xF800;  //  5 bits
      static constexpr uint16_t  k_typeMask      = 0x0700;  //  3 bits
      static constexpr uint16_t  k_truncatedMask = 0x0080;  //  1 bit
      static constexpr uint16_t  k_unusedMask    = 0x007F;  //  7 bits

      //----------------------------------------------------------------------
      //!  Valid message types.
      //----------------------------------------------------------------------
      enum class MsgType : uint8_t {
        e_typeNone            = 0,
        e_typeOriginRequest   = 1,   //  IP address origin request
        e_typeOriginResponse  = 2    //  Response to IP address origin request
      };
      
      //----------------------------------------------------------------------
      //!  Constructor
      //----------------------------------------------------------------------
      MessageHeader(uint8_t version = 1)
          : _id(0)
      {
        Version(version);
        Type(MsgType::e_typeNone);
        Truncated(false);
      }

      //----------------------------------------------------------------------
      //!  Returns the version.
      //----------------------------------------------------------------------
      uint8_t Version() const
      {
        return (_flags & k_versionMask) >> 11;
      }

      //----------------------------------------------------------------------
      //!  Sets and returns the version.
      //----------------------------------------------------------------------
      uint8_t Version(const uint8_t version)
      {
        _flags &= ~k_versionMask;
        _flags |= ((uint16_t)version) << 11;
        return Version();
      }

      //----------------------------------------------------------------------
      //!  Returns the message type.
      //----------------------------------------------------------------------
      MsgType Type() const
      {
        return (MsgType)((_flags & k_typeMask) >> 8);
      }

      //----------------------------------------------------------------------
      //!  Sets and returns the message type.
      //----------------------------------------------------------------------
      MsgType Type(MsgType msgType)
      {
        _flags &= ~k_typeMask;
        _flags |= ((uint16_t)msgType) << 8;
        return Type();
      }

      //----------------------------------------------------------------------
      //!  Returns true if the message is truncated.
      //----------------------------------------------------------------------
      bool Truncated() const
      {
        return (0 != (_flags & k_truncatedMask));
      }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the truncation flag.
      //----------------------------------------------------------------------
      bool Truncated(bool truncated)
      {
        _flags &= ~k_truncatedMask;
        if (truncated) {
          _flags |= k_truncatedMask;
        }
        return Truncated();
      }

      //----------------------------------------------------------------------
      //!  Returns the message ID.  This can be used by a client to match
      //!  queries to requests.
      //----------------------------------------------------------------------
      uint16_t Id() const
      {
        return _id;
      }

      //----------------------------------------------------------------------
      //!  Sets and returns the message ID.
      //----------------------------------------------------------------------
      uint16_t Id(uint16_t id)
      {
        _id = id;
        return _id;
      }

      //----------------------------------------------------------------------
      //!  Reads the header from an istream.  Returns the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  Writes the header to an ostream.  Returns the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  Populates the header from the given json @c j.
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);
      
      //----------------------------------------------------------------------
      //!  Returns a json representation of the header.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;
      
      //----------------------------------------------------------------------
      //!  Equality operator
      //----------------------------------------------------------------------
      bool operator == (const MessageHeader &) const = default;
      
    private:
      uint16_t   _flags;
      uint16_t  _id;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINMESSAGEHEADER_HH_
