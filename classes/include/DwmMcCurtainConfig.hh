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
//!  \file DwmMcCurtainConfig.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::Config class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINCONFIG_HH_
#define _DWMMCCURTAINCONFIG_HH_

#include <string>

#include "DwmMcCurtainDatabaseConfig.hh"
#include "DwmMcCurtainServiceConfig.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulates mccurtaind configuration.
    //------------------------------------------------------------------------
    class Config
    {
    public:
      //----------------------------------------------------------------------
      //!  Default constructor
      //----------------------------------------------------------------------
      Config();
      
      //----------------------------------------------------------------------
      //!  Parse the configuration from the file at the given @c path.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Parse(const std::string & path);

      //----------------------------------------------------------------------
      //!  Returns the syslog facility that will be used for logging.
      //----------------------------------------------------------------------
      const std::string & SyslogFacility() const;
      
      //----------------------------------------------------------------------
      //!  Sets and returns the syslog facility that will be used for
      //!  logging.
      //----------------------------------------------------------------------
      const std::string & SyslogFacility(const std::string & facility);

      //----------------------------------------------------------------------
      //!  Returns the minimum syslog priority that will be logged.
      //----------------------------------------------------------------------
      const std::string & SyslogLevel() const;

      //----------------------------------------------------------------------
      //!  Sets and returns the minimum syslog priority that will be logged.
      //----------------------------------------------------------------------
      const std::string & SyslogLevel(const std::string & level);

      //----------------------------------------------------------------------
      //!  Returns true if the source file and line number of syslog messages
      //!  will be logged.
      //----------------------------------------------------------------------
      bool SyslogLocations() const;

      //----------------------------------------------------------------------
      //!  If @c logLocations is true, mccurtaind will log the source file
      //!  and line number in syslog messages.
      //----------------------------------------------------------------------
      bool SyslogLocations(bool logLocations);

      //----------------------------------------------------------------------
      //!  Returns true if there is a service configuration and hence the
      //!  mccurtaind service should be run.
      //----------------------------------------------------------------------
      bool RunService() const;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool RunService(bool runService);

      //----------------------------------------------------------------------
      //!  Returns a const reference to the service configuration.
      //----------------------------------------------------------------------
      const ServiceConfig & Service() const;

      //----------------------------------------------------------------------
      //!  Sets and returns the service configuration.
      //----------------------------------------------------------------------
      const ServiceConfig & Service(const ServiceConfig & service);

      //----------------------------------------------------------------------
      //!  Returns a const reference to the database configuration.
      //----------------------------------------------------------------------
      const DatabaseConfig & Database() const;

      //----------------------------------------------------------------------
      //!  Sets and returns the database configuration.
      //----------------------------------------------------------------------
      const DatabaseConfig & Database(const DatabaseConfig & database);
      
      //----------------------------------------------------------------------
      //!  Prints the given Config @c cfg to an ostream @c os.  Returns the
      //!  ostream @c os.
      //----------------------------------------------------------------------
      friend std::ostream &
      operator << (std::ostream & os, const Config & cfg);
      
    private:
      bool            _runService;
      DatabaseConfig  _database;
      ServiceConfig   _service;
      std::string     _syslogFacility;
      std::string     _syslogLevel;
      bool            _logLocations;
      
      void Clear();
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINCONFIG_HH_
