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
//!  @brief Dwm::McCurtain::OriginAS class declaration
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
    //!  Encapsulates autonomous system informationL AS number, country code
    //!  and AS name.
    //------------------------------------------------------------------------
    class OriginAS
    {
    public:
      //----------------------------------------------------------------------
      //!  default constructor
      //----------------------------------------------------------------------
      OriginAS() = default;
      
      //----------------------------------------------------------------------
      //!  Construct from the given AS number @c asnum, country code
      //!  @c countryCode and AS name @c name.
      //----------------------------------------------------------------------
      OriginAS(uint32_t asnum, const std::string & countryCode,
               const std::string & name)
          : _number(asnum), _countryCode(countryCode), _name(name)
      {}
      
      //----------------------------------------------------------------------
      //!  Returns the contained AS number.
      //----------------------------------------------------------------------
      uint32_t Number() const
      { return _number; }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the contained AS number.
      //----------------------------------------------------------------------
      uint32_t Number(uint32_t asnum)
      { return _number = asnum; }

      //----------------------------------------------------------------------
      //!  Returns the contained country code.
      //----------------------------------------------------------------------
      const std::string & CountryCode() const
      { return _countryCode; }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the contained country code.
      //----------------------------------------------------------------------
      const std::string & CountryCode(const std::string & countryCode)
      { return _countryCode = countryCode; }
      
      //----------------------------------------------------------------------
      //!  Returns the contained name.
      //----------------------------------------------------------------------
      const std::string & Name() const
      { return _name; }

      //----------------------------------------------------------------------
      //!  Sets and returns the contained name.
      //----------------------------------------------------------------------
      const std::string & Name(const std::string & name)
      { return _name = name; }

      //----------------------------------------------------------------------
      //!  Reads the OriginAS from an istream.  Returns the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  Writes the OriginAS to an ostream.  Returns the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  Populates the OriginAS from the given json @c j.  Returns true on
      //!  success, false on failure.
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  Returns a json representation of the OriginAS.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  Equality operator.
      //----------------------------------------------------------------------
      bool operator == (const OriginAS &) const = default;

      //----------------------------------------------------------------------
      //!  ostream output operator (human readable)
      //----------------------------------------------------------------------
      friend std::ostream & operator << (std::ostream & os,
                                         const OriginAS & origas);
      
    private:
      uint32_t     _number;
      std::string  _countryCode;
      std::string  _name;

      void Clear();
    };

  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINORIGINAS_HH_
