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
//!  \file DwmMcCurtainRequests.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Request class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINREQUESTS_HH_
#define _DWMMCCURTAINREQUESTS_HH_

#include "DwmIpv4Address.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates a request to mccurtaind.
    //------------------------------------------------------------------------
    class Request
      : public Dwm::StreamIOCapable
    {
    public:
      //----------------------------------------------------------------------
      //!  Defaulted constructors and assignments.
      //----------------------------------------------------------------------
      Request() = default;
      Request(const Request &) = default;
      Request(Request &&) = default;
      Request & operator = (const Request &) = default;
      Request & operator = (Request &&) = default;
      
      //----------------------------------------------------------------------
      //!  Construct a request for prefixes and ASes for a given IPv4
      //!  address.
      //----------------------------------------------------------------------
      Request(const Ipv4Address & addr)
          : _data(addr)
      {}

      //----------------------------------------------------------------------
      //!  Construct a request for prefixes announced by the given origin
      //!  AS @c asnum.
      //----------------------------------------------------------------------
      Request(uint32_t asnum)
          : _data(asnum)
      {}
      
      //----------------------------------------------------------------------
      //!  Read the request from an istream.  Return the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is) override
      { return StreamIO::Read(is, _data); }

      //----------------------------------------------------------------------
      //!  Write the request to an ostream.  Return the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const override
      { return StreamIO::Write(os, _data); }

      //----------------------------------------------------------------------
      //!  We only have two types of requests, which we can hold in a
      //!  std::variant.
      //----------------------------------------------------------------------
      using DataType = std::variant<Ipv4Address,uint32_t>;

      //----------------------------------------------------------------------
      //!  Returns the encapsulated data.
      //----------------------------------------------------------------------
      const DataType & Data() const  { return _data; }
      
    private:
      DataType  _data;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINREQUESTS_HH_
