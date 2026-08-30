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
//!  @file DwmMcCurtainOriginResponse.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::McCurtain::OriginResponse class definition
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINORIGINRESPONSE_HH_
#define _DWMMCCURTAINORIGINRESPONSE_HH_

#include <nlohmann/json.hpp>

#include "DwmMcCurtainOriginPrefix.hh"
#include "DwmMcCurtainOriginRequest.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates a response to a query for IP address origin information.
    //------------------------------------------------------------------------
    class OriginResponse
    {
    public:
      //----------------------------------------------------------------------
      //!  default constructor
      //----------------------------------------------------------------------
      OriginResponse() = default;

      //----------------------------------------------------------------------
      //!  Construct from the given @c request and @c prefixes.
      //----------------------------------------------------------------------
      OriginResponse(const OriginRequest & request,
                     const std::vector<OriginPrefix> & prefixes)
          : _request(request), _prefixes(prefixes)
      {}

      //----------------------------------------------------------------------
      //!  copy constructor
      //----------------------------------------------------------------------
      OriginResponse(const OriginResponse &) = default;

      //----------------------------------------------------------------------
      //!  move constructor
      //----------------------------------------------------------------------
      OriginResponse(OriginResponse &&) = default;

      //----------------------------------------------------------------------
      //!  copy assignment operator
      //----------------------------------------------------------------------
      OriginResponse & operator = (const OriginResponse &) = default;

      //----------------------------------------------------------------------
      //!  move assignment operator
      //----------------------------------------------------------------------
      OriginResponse & operator = (OriginResponse &&) = default;

      //----------------------------------------------------------------------
      //!  destructor
      //----------------------------------------------------------------------
      ~OriginResponse() = default;

      //----------------------------------------------------------------------
      //!  Returns the encapsulated request.
      //----------------------------------------------------------------------
      const OriginRequest & Request() const
      { return _request; }

      //----------------------------------------------------------------------
      //!  Sets and returns the encapsulated request.
      //----------------------------------------------------------------------
      const OriginRequest & Request(const OriginRequest & request)
      { return _request = request; }
        
      //----------------------------------------------------------------------
      //!  Returns the encapsulated prefixes.
      //----------------------------------------------------------------------
      const std::vector<OriginPrefix> & Prefixes() const
      { return _prefixes; }

      //----------------------------------------------------------------------
      //!  Sets and returns the encapsulated prefixes.
      //----------------------------------------------------------------------
      const std::vector<OriginPrefix> &
      Prefixes(const std::vector<OriginPrefix> & prefixes)
      { return _prefixes = prefixes; }

      //----------------------------------------------------------------------
      //!  Reads the OriginResponse from an istream.  Returns the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  Writes the OriginResponse to an ostream.  Returns the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  Populqtes the OriginResponse from the given json @c j.  Returns
      //!  true on success, false on failure.
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  Returns a json representation of the OriginResponse.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  Equality operator
      //----------------------------------------------------------------------
      bool operator == (const OriginResponse &) const = default;

      //----------------------------------------------------------------------
      //!  ostream output operator
      //----------------------------------------------------------------------
      friend std::ostream & operator << (std::ostream & os,
                                         const OriginResponse & origresp);
      
    private:
      OriginRequest              _request;
      std::vector<OriginPrefix>  _prefixes;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINORIGINRESPONSE_HH_
