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
//!  \file DwmMcCurtainDatabaseConfig.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::DatabaseConfig class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINDATABASECONFIG_HH_
#define _DWMMCCURTAINDATABASECONFIG_HH_

#include <iostream>
#include <string>

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulate the database parts of the mccurtaind configuration.
    //------------------------------------------------------------------------
    class DatabaseConfig
    {
    public:
      //----------------------------------------------------------------------
      //!  Default constructor
      //----------------------------------------------------------------------
      DatabaseConfig();
      
      //----------------------------------------------------------------------
      //!  Copy constructor
      //----------------------------------------------------------------------
      DatabaseConfig(const DatabaseConfig &) = default;

      //----------------------------------------------------------------------
      //!  Move constructor
      //----------------------------------------------------------------------
      DatabaseConfig(DatabaseConfig &&) = default;
      
      //----------------------------------------------------------------------
      //!  Copy assignment
      //----------------------------------------------------------------------
      DatabaseConfig & operator = (const DatabaseConfig &) = default;

      //----------------------------------------------------------------------
      //!  Move assignment
      //----------------------------------------------------------------------
      DatabaseConfig & operator = (DatabaseConfig &&) = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Ipv4ToASFile() const  { return _ipv4ToASFile; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Ipv4ToASFile(const std::string & f)
      { return _ipv4ToASFile = f; }
        
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & ASToIpv4File() const  { return _asToIpv4File; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & ASToIpv4File(const std::string & f)
      { return _asToIpv4File = f; }
        
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & ASNTxtFile() const  { return _asnTxtFile; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & ASNTxtFile(const std::string & f)
      { return _asnTxtFile = f; }

      //----------------------------------------------------------------------
      //!  Prints the given database configuration @c cfg to the given
      //!  ostream @c os.
      //----------------------------------------------------------------------
      friend std::ostream &
      operator << (std::ostream & os, const DatabaseConfig & cfg);

      //----------------------------------------------------------------------
      //!  Reset the configuration to the defaults.
      //----------------------------------------------------------------------
      void Clear();
      
    private:
      std::string  _ipv4ToASFile;
      std::string  _asToIpv4File;
      std::string  _asnTxtFile;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINDATABASECONFIG_HH_
