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

#include "DwmMcCurtainMessage.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::istream & Message::Read(std::istream & is)
    {
      if (_header.Read(is)) {
        if (_header.Format() == MessageFormat::e_binary) {
          if (_header.IsResponse()) {
            OriginResponse  resp;
            if (resp.Read(is)) {
              _payload = resp;
            }
          }
          else {
            OriginRequest  req;
            if (req.Read(is)) {
              _payload = req;
            }
          }
        }
        else if (_header.Format() == MessageFormat::e_json) {
          nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
          if (! j.is_discarded()) {
            if (_header.IsResponse()) {
              OriginResponse  resp;
              if (resp.FromJson(j)) {
                _payload = resp;
              }
            }
            else {
              OriginRequest  req;
              if (req.FromJson(j)) {
                _payload = req;
              }
              else {
                is.setstate(std::ios_base::failbit);
              }
            }
          }
          else {
            is.setstate(std::ios_base::failbit);
          }
        }
      }
      return is;
    }

    //------------------------------------------------------------------------
    std::ostream & Message::Write(std::ostream & os) const
    {
      if (_header.Write(os)) {
        if (_header.Format() == MessageFormat::e_binary) {
          StreamIO::Write(os, _payload);
        }
        else if (_header.Format() == MessageFormat::e_json) {
          nlohmann::json  j;
          if (std::holds_alternative<OriginRequest>(_payload)) {
            j = std::get<0>(_payload).ToJson();
          }
          else if (std::holds_alternative<OriginResponse>(_payload)) {
            j = std::get<1>(_payload).ToJson();
          }
          os << j.dump();
        }
      }
      return os;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
