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
//!  @file DwmMcCurtainCaidaRouteviews.hh
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINCAIDAV4ROUTEVIEWS_HH_
#define _DWMMCCURTAINCAIDAV4ROUTEVIEWS_HH_

#include <map>
#include <set>

#include "DwmIpv4Prefix.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class CaidaV4Routeviews
    {
    public:
      using V4PrefixSet = std::set<Ipv4Prefix>;
      using ASSet = std::set<uint32_t>;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      class ASMapValue
      {
      public:
        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        ASMapValue() = default;

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        inline void Insert(const Ipv4Prefix & pfx)
        {
          _prefixSets[pfx.MaskLength()].insert(pfx);
        }

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        inline const std::map<uint8_t,V4PrefixSet> & PrefixSets() const
        { return _prefixSets; }
          
        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        void Aggregate();

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        bool HaveWider(const Ipv4Prefix & pfx) const;

        //--------------------------------------------------------------------
        //!  
        //--------------------------------------------------------------------
        size_t TotalPrefixes() const
        {
          size_t  rc = 0;
          for (const auto & pfxSet : _prefixSets) {
            rc += pfxSet.second.size();
          }
          return rc;
        }
          
      private:
        std::map<uint8_t,V4PrefixSet>  _prefixSets;

        void SwallowSpecifics();
      };

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      inline const std::map<ASSet,ASMapValue> & ASSets() const
      { return _asSets; }
            
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool Load(const std::string & filePath);
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void Aggregate();

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      size_t TotalPrefixes() const
      {
        size_t  rc = 0;
        for (const auto & asSet : _asSets) {
          rc += asSet.second.TotalPrefixes();
        }
        return rc;
      }
      
    private:
      std::map<ASSet,ASMapValue>  _asSets;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINCAIDAV4ROUTEVIEWS_HH_
