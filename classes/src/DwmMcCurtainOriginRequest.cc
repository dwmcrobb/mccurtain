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
//!  @file DwmMcCurtainOriginRequest.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmMcCurtainOriginRequest.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::istream & OriginRequest::Read(std::istream & is)
    {
      _address.Read(is);
      return is;
    }

    //------------------------------------------------------------------------
    std::ostream & OriginRequest::Write(std::ostream & os) const
    {
      _address.Write(os);
      return os;
    }

    //------------------------------------------------------------------------
    bool OriginRequest::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      if (j.is_string()) {
        _address = IpAddress(j.get<std::string>());
        rc = true;
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    nlohmann::json OriginRequest::ToJson() const
    {
      nlohmann::json  j = (std::string)_address;
      return j;
    }

    //------------------------------------------------------------------------
    std::ostream & operator << (std::ostream & os,
                                const OriginRequest & origreq)
    {
      os << origreq._address;
      return os;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
