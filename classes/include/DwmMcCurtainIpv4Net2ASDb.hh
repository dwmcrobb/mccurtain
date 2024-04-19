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
//!  \file DwmMcCurtainIpv4Net2ASDb.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Ipv4Net2ASDb class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINIPV4NET2ASDB_HH_
#define _DWMMCCURTAINIPV4NET2ASDB_HH_

#include <set>

#include "DwmIpv4Routes.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Just a wrapper around an Ipv4Routes<std::set<uint32_t>>.  The
    //!  set<uint32_t> values for each route (prefix) contain all of the
    //!  origin AS numbers for the prefix.
    //------------------------------------------------------------------------
    class Ipv4Net2ASDb
    {
    public:
      using  NetASes = Ipv4Routes<std::set<uint32_t>>;

      //----------------------------------------------------------------------
      //!  Loads the contents from a gzip'ed routeviews file from CAIDA.
      //!  Returns true on success, falue on failure.
      //----------------------------------------------------------------------
      bool LoadCAIDARouteViews(const std::string & path);
      
      //----------------------------------------------------------------------
      //!  Loads the contents from the native binary file located at @c path.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Load(const std::string & path);
      
      //----------------------------------------------------------------------
      //!  Saves the contents in native binary form to the file at @c path.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Save(const std::string & path) const;

      //----------------------------------------------------------------------
      //!  Returns the contained entries.
      //----------------------------------------------------------------------
      const NetASes & Entries() const  { return _netASes; }
      
    private:
      NetASes  _netASes;
    };
    
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINIPV4NET2ASDB_HH_
