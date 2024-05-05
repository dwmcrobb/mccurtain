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
//!  \file DwmMcCurtainDatabaseConfig.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::DatabaseConfig class implementation
//---------------------------------------------------------------------------

#include "DwmMcCurtainDatabaseConfig.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    DatabaseConfig::DatabaseConfig()
    {
      Clear();
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream &
    operator << (std::ostream & os, const DatabaseConfig & cfg)
    {
      os << "#============================================================================\n"
         << "#  databases configuration.\n"
         << "#============================================================================\n"
         << "databases {\n"
         << "#------------------------------------------------------------------------\n"
         << "#  IPv4 to AS database.  If not configured, /usr/local/etc/ipv42as.db\n"
         << "#  will be assumed.\n"
         << "#------------------------------------------------------------------------\n"
         << "ipv4toas = \"" << cfg._ipv4ToASFile << "\";\n\n"
         << "#------------------------------------------------------------------------\n"
         << "#  AS to IPv4 prefixes database.  If not configured,\n"
         << "#  /usr/local/etc/as2ipv4.db will be assumed.\n"
         << "#------------------------------------------------------------------------\n"
         << "astoipv4 = \"" << cfg._asToIpv4File << "\";\n\n"
         << "#------------------------------------------------------------------------\n"
         << "#  RIPE asn.txt file location.  If not configured,\n"
         << "#  /usr/local/etc/asn.txt will be assumed.\n"
         << "#------------------------------------------------------------------------\n"
         << "asntxt = \"" << cfg._asnTxtFile << "\";\n"
         << "}\n";
      return os;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void DatabaseConfig::Clear()
    {
      _ipv4ToASFile = "/usr/local/etc/ipv42as.db";
      _asToIpv4File = "/usr/local/etc/as2ipv4.db";
      _asnTxtFile = "/usr/local/etc/asn.txt";
      return;
    }
      
  }  // namespace McCurtain

}  // namespace Dwm
