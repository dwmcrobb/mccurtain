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
//!  @file DwmMcCurtainOriginAS.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcCurtainOriginAS.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::istream & OriginAS::Read(std::istream & is)
    {
      Clear();
      if (StreamIO::Read(is, _number)) {
        if (is.read((caddr_t)_countryCode.data(), 2)) {
          uint8_t  namelen;
          if (is.read((caddr_t)&namelen, sizeof(namelen))) {
            if (namelen) {
              _name.resize(namelen);
              is.read((caddr_t)_name.data(), namelen);
            }
          }
        }
      }
      
      return is;
    }
    
    //------------------------------------------------------------------------
    std::ostream & OriginAS::Write(std::ostream & os) const
    {
      if (StreamIO::Write(os, _number)) {
        if (os.write((caddr_t)_countryCode.data(), 2)) {
          uint8_t  namelen = _name.size();
          if (os.write((caddr_t)&namelen, sizeof(namelen))) {
            if (namelen) {
              os.write((caddr_t)_name.data(), namelen);
            }
          }
        }
      }
      return os;
    }

    //------------------------------------------------------------------------
    bool OriginAS::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      Clear();
      if (j.is_object()) {
        auto it = j.find("as");
        if ((j.end() != it) && it->is_number()) {
          _number = it->get<uint32_t>();
          it = j.find("cc");
          if ((j.end() != it) && it->is_string()) {
            _countryCode = it->get<std::string>();
            it = j.find("nm");
            if ((j.end() != it) && it->is_string()) {
              _name = it->get<std::string>();
              rc = true;
            }
          }
        }
      }
      return rc;
    }
        
    //------------------------------------------------------------------------
    nlohmann::json OriginAS::ToJson() const
    {
      nlohmann::json  j;
      j["as"] = _number;
      j["cc"] = _countryCode;
      j["nm"] = _name;
      return j;
    }
    
    //------------------------------------------------------------------------
    void OriginAS::Clear()
    {
      _number = 0;
      _countryCode = "--";
      _name.clear();
      return;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
