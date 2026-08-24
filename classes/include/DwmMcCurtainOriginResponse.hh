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
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINORIGINRESPONSE_HH_
#define _DWMMCCURTAINORIGINRESPONSE_HH_

#include <nlohmann/json.hpp>

#include "DwmMcCurtainOriginPrefix.hh"
#include "DwmMcCurtainOriginRequest.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class OriginResponse
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginResponse() = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginResponse(const OriginRequest & request,
                     const std::vector<OriginPrefix> & prefixes)
          : _request(request), _prefixes(prefixes)
      {}

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginResponse(const OriginResponse &) = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginResponse(OriginResponse &&) = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginResponse & operator = (const OriginResponse &) = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      OriginResponse & operator = (OriginResponse &&) = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      ~OriginResponse() = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const OriginRequest & Request() const
      { return _request; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const OriginRequest & Request(const OriginRequest & request)
      { return _request = request; }
        
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<OriginPrefix> & Prefixes() const
      { return _prefixes; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<OriginPrefix> &
      Prefixes(const std::vector<OriginPrefix> & prefixes)
      { return _prefixes = prefixes; }

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

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator == (const OriginResponse &) const = default;
      
    private:
      OriginRequest              _request;
      std::vector<OriginPrefix>  _prefixes;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINORIGINRESPONSE_HH_
