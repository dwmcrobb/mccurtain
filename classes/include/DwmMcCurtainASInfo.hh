//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024, 2026
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
//!  \file DwmMcCurtainASInfo.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::ASInfo class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINASINFO_HH_
#define _DWMMCCURTAINASINFO_HH_


#include <nlohmann/json.hpp>

#include "DwmIpv4PrefixPatricia.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulate information for a single AS.
    //------------------------------------------------------------------------
    class ASInfo
    {
    public:
      //----------------------------------------------------------------------
      //!  Pupulates the ASInfo from the given JSON @c j.
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);
      
      //----------------------------------------------------------------------
      //!  Returns a JSON representation of the ASInfo.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  Returns the AS number of the AS.
      //----------------------------------------------------------------------
      inline uint32_t Number() const  { return _number; }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the AS number of the AS.
      //----------------------------------------------------------------------
      inline uint32_t Number(uint32_t number)  { return _number = number; }

      //----------------------------------------------------------------------
      //!  Returns the name of the AS.
      //----------------------------------------------------------------------
      inline const std::string & Name() const  { return _name; }

      //----------------------------------------------------------------------
      //!  Sets and returns the name of the AS.
      //----------------------------------------------------------------------
      inline const std::string & Name(const std::string & name)
      { return _name = name; }
      
      //----------------------------------------------------------------------
      //!  Returns the organization that owns the AS.
      //----------------------------------------------------------------------
      inline const std::string & Org() const  { return _org; }

      //----------------------------------------------------------------------
      //!  Sets and returns the organization that owns the AS.
      //----------------------------------------------------------------------
      inline const std::string & Org(const std::string org)
      { return _org = org; }

      //----------------------------------------------------------------------
      //!  Returns the country code of the AS.
      //----------------------------------------------------------------------
      inline const std::string & CountryCode() const  { return _countryCode; }

      //----------------------------------------------------------------------
      //!  Sets and returns the country code of the AS.
      //----------------------------------------------------------------------
      inline const std::string & CountryCode(const std::string & countryCode)
      { return _countryCode = countryCode; }

      //----------------------------------------------------------------------
      //!  Returns a const reference to the prefixes for the AS.
      //----------------------------------------------------------------------
      inline const Ipv4PrefixPatricia<uint8_t> & Nets() const
      { return _nets; }
      
      //----------------------------------------------------------------------
      //!  Returns a mutable reference to the prefixes for the AS.
      //----------------------------------------------------------------------
      inline Ipv4PrefixPatricia<uint8_t> & Nets()   { return _nets; }

      //----------------------------------------------------------------------
      //!  Sets and returns the prefixes for the AS.
      //----------------------------------------------------------------------
      inline Ipv4PrefixPatricia<uint8_t> &
      Nets(const Ipv4PrefixPatricia<uint8_t> & nets)
      { return _nets = nets; }

      void Clear();
      
    private:
      uint32_t                     _number;
      std::string                  _name;
      std::string                  _org;
      std::string                  _countryCode;
      Ipv4PrefixPatricia<uint8_t>  _nets;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINASINFO_HH_
