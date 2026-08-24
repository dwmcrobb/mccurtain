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
//!  @file DwmMcCurtainOriginPrefix.hh
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINORIGINPREFIX_HH_
#define _DWMMCCURTAINORIGINPREFIX_HH_

#include <vector>

#include "DwmIpPrefix.hh"
#include "DwmMcCurtainOriginAS.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class OriginPrefix
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginPrefix() = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const IpPrefix & Prefix() const
      { return _prefix; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const IpPrefix & Prefix(const IpPrefix & pfx)
      { return _prefix = pfx; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<OriginAS> & ASes() const
      { return _ases; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<OriginAS> & ASes(const std::vector<OriginAS> & ases)
      { return _ases = ases; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool FromJson(const nlohmann::json & j);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;
      
    private:
      IpPrefix               _prefix;
      std::vector<OriginAS>  _ases;
    };
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINORIGINPREFIX_HH_
