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
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINMESSAGEHEADER_HH_
#define _DWMMCCURTAINMESSAGEHEADER_HH_

#include <iostream>
#include <nlohmann/json.hpp>

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class MessageHeader
    {
    public:
      static constexpr uint16_t  k_versionMask   = 0xF800;  //  5 bits
      static constexpr uint16_t  k_typeMask      = 0x0700;  //  3 bits
      static constexpr uint16_t  k_truncatedMask = 0x0080;  //  1 bit
      static constexpr uint16_t  k_unusedMask    = 0x007F;  //  7 bits

      enum class MsgType : uint8_t {
        e_typeNone            = 0,
        e_typeOriginRequest   = 1,
        e_typeOriginResponse  = 2
      };
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      MessageHeader(uint8_t version = 1)
          : _id(0)
      {
        Version(version);
        Type(MsgType::e_typeNone);
        Truncated(false);
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint8_t Version() const
      {
        return (_flags & k_versionMask) >> 11;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint8_t Version(const uint8_t version)
      {
        _flags &= ~k_versionMask;
        _flags |= ((uint16_t)version) << 11;
        return Version();
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      MsgType Type() const
      {
        return (MsgType)((_flags & k_typeMask) >> 8);
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      MsgType Type(MsgType msgType)
      {
        _flags &= ~k_typeMask;
        _flags |= ((uint16_t)msgType) << 8;
        return Type();
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool Truncated() const
      {
        return (0 != (_flags & k_truncatedMask));
      }
      
      //----------------------------------------------------------------------
      //!  
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
      //!  
      //----------------------------------------------------------------------
      uint16_t Id() const
      {
        return _id;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint16_t Id(uint16_t id)
      {
        _id = id;
        return _id;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      bool FromJson(const nlohmann::json & j);
      
      nlohmann::json ToJson() const;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator == (const MessageHeader &) const = default;
      
    private:
      uint16_t   _flags;
      uint16_t  _id;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINMESSAGEHEADER_HH_
