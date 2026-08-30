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
//!  @file DwmMcCurtainOriginPrefix.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmStreamIO.hh"
#include "DwmMcCurtainOriginPrefix.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    std::istream & OriginPrefix::Read(std::istream & is)
    {
      if (_prefix.Read(is)) {
        StreamIO::Read(is, _ases);
      }
      return is;
    }
    
    //------------------------------------------------------------------------
    std::ostream & OriginPrefix::Write(std::ostream & os) const
    {
      if (_prefix.Write(os)) {
        StreamIO::Write(os, _ases);
      }
      return os;
    }

    //------------------------------------------------------------------------
    bool OriginPrefix::FromJson(const nlohmann::json & j)
    {
      bool  rc = false;
      _ases.clear();
      if (j.is_object()) {
        auto  it = j.find("pfx");
        if ((j.end() != it) && it->is_string()) {
          _prefix = IpPrefix(it->get<std::string>());
          it = j.find("ases");
          if ((j.end() != it) && it->is_array()) {
            size_t  i = 0;
            for ( ; i < it->size(); ++i) {
              OriginAS  origas;
              if (origas.FromJson((*it)[i])) {
                _ases.push_back(origas);
              }
              else {
                break;
              }
            }
            rc = (it->size() == i);
          }
        }
      }
      if (! rc)  { _ases.clear(); }
      return rc;
    }
    
    //------------------------------------------------------------------------
    nlohmann::json OriginPrefix::ToJson() const
    {
      nlohmann::json  j;
      j["pfx"] = (std::string)_prefix;
      j["ases"] = nlohmann::json::array();
      for (size_t i = 0; i < _ases.size(); ++i) {
        j["ases"][i] = _ases[i].ToJson();
      }
      return j;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & operator << (std::ostream & os,
                                const OriginPrefix & origpfx)
    {
      os << "   " << origpfx.Prefix() << '\n';
      if (! origpfx.ASes().empty()) {
        for (const auto & as : origpfx.ASes()) {
          os << "      " << as << '\n';
        }
      }
      return os;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
