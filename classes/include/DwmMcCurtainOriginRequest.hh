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
//!  @file DwmMcCurtainOriginRequest.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::McCurtain::OriginRequest class definition
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINORIGINREQUEST_HH_
#define _DWMMCCURTAINORIGINREQUEST_HH_

#include <nlohmann/json.hpp>

#include "DwmIpAddress.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates a request for IP address origin information.  This
    //!  represents one potential payload inside a Message.
    //------------------------------------------------------------------------
    class OriginRequest
    {
    public:
      //----------------------------------------------------------------------
      //!  default constructor
      //----------------------------------------------------------------------
      OriginRequest() = default;

      //----------------------------------------------------------------------
      //!  Construct from the given IpAddress @c address
      //----------------------------------------------------------------------
      OriginRequest(const IpAddress & address)
          : _address(address)
      {}

      //----------------------------------------------------------------------
      //!  Construct from the given Ipv4Address @c address
      //----------------------------------------------------------------------
      OriginRequest(const Ipv4Address & address)
          : _address(address)
      {}

      //----------------------------------------------------------------------
      //!  Construct from the given Ipv6Address @c address
      //----------------------------------------------------------------------
      OriginRequest(const Ipv6Address & address)
          : _address(address)
      {}
      
      //----------------------------------------------------------------------
      //! copy constructor
      //----------------------------------------------------------------------
      OriginRequest(const OriginRequest &) = default;

      //----------------------------------------------------------------------
      //!  move constructor
      //----------------------------------------------------------------------
      OriginRequest(OriginRequest &&) = default;

      //----------------------------------------------------------------------
      //!  Copy assignment operator
      //----------------------------------------------------------------------
      OriginRequest & operator = (const OriginRequest &) = default;

      //----------------------------------------------------------------------
      //!  Move assignment operator
      //----------------------------------------------------------------------
      OriginRequest & operator = (OriginRequest &&) = default;

      //----------------------------------------------------------------------
      //!  destructor
      //----------------------------------------------------------------------
      ~OriginRequest() = default;
      
      //----------------------------------------------------------------------
      //!  Returns the encapsulated IP address.
      //----------------------------------------------------------------------
      const IpAddress & Address() const
      { return _address; }

      //----------------------------------------------------------------------
      //!  Sets and returns the encapsulated IP address.
      //----------------------------------------------------------------------
      const IpAddress & Address(const IpAddress & address)
      { return _address = address; }

      //----------------------------------------------------------------------
      //!  Reads the OriginRequest from an istream.  Returns the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  Writes the OriginRequest to an ostream.  Returns the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  Populates the OriginRequest from the given json @c j.  Returns
      //!  true on success, false on failure.
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  Returns a json representation of the OriginRequest.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  equality operator
      //----------------------------------------------------------------------
      bool operator == (const OriginRequest &) const = default;

      //----------------------------------------------------------------------
      //!  ostream output operator (human-readable output)
      //----------------------------------------------------------------------
      friend std::ostream & operator << (std::ostream & os,
                                         const OriginRequest & origreq);
      
    private:
      IpAddress  _address;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINORIGINREQUEST_HH_
