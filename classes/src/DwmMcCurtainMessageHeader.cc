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
//!  @file DwmMcCurtainMessageHeader.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcCurtainMessageHeader.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::istream & MessageHeader::Read(std::istream & is)
    {
      if (StreamIO::Read(is, _flags)) {
        StreamIO::Read(is, _id);
      }
      return is;
    }
    
    //------------------------------------------------------------------------
    std::ostream & MessageHeader::Write(std::ostream & os) const
    {
      if (StreamIO::Write(os, _flags)) {
        StreamIO::Write(os, _id);
      }
      return os;
    }

    //------------------------------------------------------------------------
    bool MessageHeader::FromJson(const nlohmann::json & j)
    {
      if (j.is_object()) {
        auto  it = j.find("v");
        if ((j.end() != it) && it->is_number()) {
          Version(it->get<uint8_t>());
          it = j.find("ty");
          if ((j.end() != it) && it->is_number()) {
            Type((MsgType)(it->get<uint8_t>()));
            it = j.find("tr");
            if ((j.end() != it) && it->is_boolean()) {
              Truncated(it->get<bool>());
            }
            else {
              Truncated(false);
            }
            return true;
          }
        }
      }
      return false;
    }
    
    //------------------------------------------------------------------------
    nlohmann::json MessageHeader::ToJson() const
    {
      nlohmann::json  j;
      j["v"] = (uint16_t)Version();
      j["ty"] = (uint16_t)Type();
      j["tr"] = Truncated();
      return j;
    }

  }  // namespace McCurtain

}  // namespace Dwm
