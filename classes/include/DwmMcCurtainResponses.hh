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
//!  \file DwmMcCurtainResponses.hh
//!  \author Daniel W. McRobb
//!  \brief Types of responses from mccurtaind
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINRESPONSES_HH_
#define _DWMMCCURTAINRESPONSES_HH_

#include <tuple>
#include <vector>

#include "DwmIpv4Prefix.hh"
#include "DwmMcCurtainRipeAsnTxt.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  A tuple containing an IPV4 prefix and a vector of
    //!  (AS_number,RipeAsnTxt::Entry) pairs.
    //------------------------------------------------------------------------
    using Ipv4AddrResponseEntry =
      std::tuple<Ipv4Prefix,
                 std::vector<std::pair<uint32_t,RipeAsnTxt::Entry>>>;

    //------------------------------------------------------------------------
    //!  For an IPv4 address request, mccurtaind responds with a vector of
    //!  Ipv4AddrResponseEntry.
    //------------------------------------------------------------------------
    using Ipv4AddrResponse = std::vector<Ipv4AddrResponseEntry>;

    //------------------------------------------------------------------------
    //!  For an AS request, mccurtaind responds with a tuple of AS number,
    //!  RipeAsnTxt::Entry and vector of IPv4 prefixes.
    //------------------------------------------------------------------------
    using ASPrefixesResponse = std::tuple<uint32_t,RipeAsnTxt::Entry,
                                          std::vector<Ipv4Prefix>>;

  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINRESPONSES_HH_
