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
//!  @file DwmMcCurtainOriginAS.hh
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINORIGINAS_HH_
#define _DWMMCCURTAINORIGINAS_HH_

#include <cstdint>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class OriginAS
    {
    public:
      OriginAS() = default;
      OriginAS(uint32_t asnum, const std::string & countryCode,
               const std::string & name)
          : _number(asnum), _countryCode(countryCode), _name(name)
      {}
      uint32_t Number() const                  { return _number; }
      uint32_t Number(uint32_t asnum)          { return _number = asnum; }
      const std::string & CountryCode() const  { return _countryCode; }
      const std::string & CountryCode(const std::string & countryCode)
      { return _countryCode = countryCode; }
      const std::string & Name() const         { return _name; }
      const std::string & Name(const std::string & name)
      { return _name = name; }

      std::istream & Read(std::istream & is);
      std::ostream & Write(std::ostream & os) const;

      bool FromJson(const nlohmann::json & j);
      nlohmann::json ToJson() const;
      
    private:
      uint32_t     _number;
      std::string  _countryCode;
      std::string  _name;

      void Clear();
    };

  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINORIGINAS_HH_
