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
//!  \file DwmMcCurtainASInfo.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::ASInfo class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINASINFO_HH_
#define _DWMMCCURTAINASINFO_HH_


#include <nlohmann/json.hpp>

#include "DwmIpv4Routes.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulate information for a single AS.
    //------------------------------------------------------------------------
    class ASInfo
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline uint32_t Number() const  { return _number; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline uint32_t Number(uint32_t number)  { return _number = number; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline const std::string & Org() const  { return _org; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline const std::string & Org(const std::string org)
      { return _org = org; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline const std::string & CountryCode() const  { return _countryCode; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline const std::string & CountryCode(const std::string & countryCode)
      { return _countryCode = countryCode; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline const Ipv4Routes<uint8_t> & Nets() const  { return _nets; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline Ipv4Routes<uint8_t> & Nets()   { return _nets; }

      void Clear();
      
    private:
      uint32_t             _number;
      std::string          _org;
      std::string          _countryCode;
      Ipv4Routes<uint8_t>  _nets;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINASINFO_HH_
