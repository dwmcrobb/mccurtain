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
//!  \file DwmMcCurtainAS2Ipv4NetDb.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::AS2Ipv4NetDb class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINAS2IPV4NETDB_HH_
#define _DWMMCCURTAINAS2IPV4NETDB_HH_

#include "DwmMcCurtainIpv4Net2ASDb.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Holds IPv4 prefixes for a set of Autonomous Systems (ASes) on a
    //!  per-AS basis.  I do this by using an unordered_map whose keys are
    //!  AS numbers and whose values are Ipv4Routes<uint8_t>.  Note that
    //!  the uint8_t value in the Ipv4Routes is always set to 1 because it's
    //!  effectively unused.  I'm just using Ipv4Routes because it has some
    //!  desirable functionality for a set of IPv4 prefixes.
    //!
    //!  The contents of an instance of this class can be loaded in 2 ways:
    //!   - from an existing Ipv4Net2ASDb
    //!   - from a file that was previously created via the Save() member
    //!     (this is the 'native' binary form and is portable).
    //!
    //!  I typically get a routeviews file from CAIDA and use it to create
    //!  an Ipv4Net2ASDb, then create an AS2Ipv4NetDb from the Ipv4Net2ASDb.
    //!  I then Save() both, so I can use the native binary form in various
    //!  utilities.
    //------------------------------------------------------------------------
    class AS2Ipv4NetDb
      : public StreamIOCapable
    {
    public:
      using  ASNets = Ipv4Routes<uint8_t>;

      //----------------------------------------------------------------------
      //!  Load from the given Ipv4Net2ASDb @c net2asdb.  Returns true on
      //!  success, false on failure.
      //----------------------------------------------------------------------
      bool Load(const Ipv4Net2ASDb & net2asdb);

      //----------------------------------------------------------------------
      //!  Load from a file at the given @c path.  Returns true on       
      //!  success, false on failure.
      //----------------------------------------------------------------------
      bool Load(const std::string & path);

      //----------------------------------------------------------------------
      //!  Saves to a file at the given @c path.  Returns true on success,
      //!  falue on failure.
      //----------------------------------------------------------------------
      bool Save(const std::string & path) const;

      //----------------------------------------------------------------------
      //!  Reads the contents from an istream.  Returns the istream.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is) override;

      //----------------------------------------------------------------------
      //!  Writes the contents to an ostream.  Returns the ostream.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const override;

      //----------------------------------------------------------------------
      //!  Returns the total number of contained prefixes, across all ASes.
      //!  Note that a prefix may be announced by more than one AS, hence
      //!  the returned value does not represent the number of unique
      //!  prefixes.
      //----------------------------------------------------------------------
      uint32_t Size() const
      {
        uint32_t  rc = 0;
        for (auto it = _asNets.begin(); it != _asNets.end(); ++it) {
          rc += it->second.Size();
        }
        return rc;
      }

      //----------------------------------------------------------------------
      //!  Returns a const reference to the contained unordered_map of ASNets
      //!  keyed by AS number.
      //----------------------------------------------------------------------
      const std::unordered_map<uint32_t,ASNets> & Nets() const
      {
        return _asNets;
      }
      
    private:
      std::unordered_map<uint32_t,ASNets>  _asNets;
    };
    
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINAS2IPV4NETDB_HH_
