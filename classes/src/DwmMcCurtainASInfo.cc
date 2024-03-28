//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file DwmMcCurtainASInfo.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::ASInfo class implementation
//---------------------------------------------------------------------------

#include "DwmMcCurtainASInfo.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ASInfo::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      Clear();
      if (j.is_object()) {
        auto  it = j.find("AS");
        if ((it != j.end()) && it->is_number()) {
          _number = it->get<uint32_t>();
          it = j.find("orgName");
          if ((it != j.end()) && it->is_string()) {
            _orgName = it->get<string>();
            it = j.find("CC");
            if ((it != j.end()) && it->is_string()) {
              _countryCode = it->get<string>();
              rc = true;
              it = j.find("nets");
              if ((it != j.end()) && it->is_array()) {
                for (const auto & net : *it) {
                  if (net.is_string()) {
                    Ipv4Prefix  pfx(net.get<string>());
                    if ((pfx.Network().Raw() != 0xFFFFFFFF)
                        && (pfx.MaskLength() != 0)) {
                      _nets[pfx] = 1;
                    }
                    else {
                      rc = false;
                      break;
                    }
                  }
                }
              }
              else {
                rc = false;
              }
            }
          }
        }
      }
      if (! rc) {
        Clear();
      }
      return rc;
    }
            
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    nlohmann::json ASInfo::ToJson() const
    {
      nlohmann::json  j;
      j["AS"] = _number;
      j["orgName"] = _orgName;
      j["CC"] = _countryCode;
      if (! _nets.Empty()) {
        j["nets"] = nlohmann::json::array();
        vector<pair<Ipv4Prefix,uint8_t>>  netvec;
        _nets.SortByKey(netvec);
        for (size_t i = 0; i < netvec.size(); ++i) {
          j["nets"][i] = netvec[i].first.ToString();
        }
      }
      return j;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void ASInfo::Clear()
    {
      _number = 0;
      _orgName.clear();
      _countryCode.clear();
      _nets.Clear();
      return;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
