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
//!  \file DwmMcCurtainASes.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::ASes class declaration
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINASES_HH_
#define _DWMMCCURTAINASES_HH_

#include "DwmMcCurtainASInfo.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  This class is used to hold information about ASes that I like to
    //!  block from accessing servers on my home network.  Internally it's
    //!  just a map of AS numbers to ASInfo objects.  I keep a JSON file of
    //!  skeletal information about ASes of interest (AS number, country code,
    //!  organization), which can be used in combination with an AS2Ipv4NetDb
    //!  file to pupulate all the prefixes being announced by each AS of
    //!  interest on a per-AS basis (as the values in the map).  A JSON
    //!  object that contains all of the information of interest can then
    //!  be fetched via the ToJson() member.  A vector of all prefixes
    //!  (useful for a pf table) may also be populated via the MakePfList()
    //!  member.
    //------------------------------------------------------------------------
    class ASes
    {
    public:
      //----------------------------------------------------------------------
      //!  Returns a JSON representation of the ASes.
      //----------------------------------------------------------------------
      nlohmann::json ToJson() const;

      //----------------------------------------------------------------------
      //!  Loads from the given @c jsonFile and an AS2Ipv4NetDb file
      //!  @c asdbFile.  Returns true on success, false on failure.
      //!  
      //!  @c jsonFile contains skeletal information about
      //!  ASes of interest (see ../../etc/asn-list.json as an example).
      //!  @c asdbFile contains prefixes announced by each AS in a global
      //!  BGP routing table dump.
      //----------------------------------------------------------------------
      bool Load(const std::string & jsonFile, const std::string & asdbFile);
      
      //----------------------------------------------------------------------
      //!  Returns a reference to the contained map of ASInfo objects keyed
      //!  by AS number.
      //----------------------------------------------------------------------
      inline std::map<uint32_t,ASInfo> & ASMap()  
      { return _asMap; }

      //----------------------------------------------------------------------
      //!  Populates @c pfList with all of the contained network prefixes,
      //!  with the exception of those contained in @c exceptions.  Returns
      //!  true if the resulting @c pfList is non-empty as a result, else
      //!  returns false.
      //----------------------------------------------------------------------
      bool MakePfList(std::vector<Dwm::Ipv4Prefix> & pfList,
                      const std::vector<Dwm::Ipv4Prefix> & exceptions);
      
    private:
      std::map<uint32_t,ASInfo>  _asMap;

      void Coalesce();
      bool FromJsonFile(const std::string & filePath);
      bool LoadASDb(const std::string & asdbFile);
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINASES_HH_
