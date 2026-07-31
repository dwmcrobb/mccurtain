//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024, 2026
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
//!  \file DwmMcCurtainAS2Ipv4NetDb.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::AS2Ipv4NetDb implementation
//---------------------------------------------------------------------------

#include <fstream>

#include "DwmStreamIO.hh"
#include "DwmMcCurtainAS2Ipv4NetDb.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool AS2Ipv4NetDb::Load(const Ipv4Net2ASDb & net2asdb)
    {
      _asNets.clear();
      const auto & entries = net2asdb.Entries();
      for (const auto & entry : entries) {
        for (const auto & as : entry.second) {
          _asNets[as][entry.first] = 1;
        }
      }
      return (! _asNets.empty());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool AS2Ipv4NetDb::Load(const std::string & path)
    {
      bool  rc = false;
      ifstream  is(path);
      if (is) {
        if (Read(is)) {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool AS2Ipv4NetDb::Save(const std::string & path) const
    {
      bool  rc = false;
      ofstream  os(path);
      if (os) {
        if (Write(os)) {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::istream & AS2Ipv4NetDb::Read(std::istream & is)
    {
      return StreamIO::Read(is, _asNets);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & AS2Ipv4NetDb::Write(std::ostream & os) const
    {
      return StreamIO::Write(os, _asNets);
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
