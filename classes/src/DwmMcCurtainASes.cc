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
//!  \file DwmMcCurtainASes.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <regex>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "DwmSysLogger.hh"
#include "DwmMcCurtainASes.hh"
#include "DwmMcCurtainAS2Ipv4NetDb.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;
    
    using boost::iostreams::filtering_streambuf;
    using boost::iostreams::gzip_decompressor;
    using boost::iostreams::gzip_compressor;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ASes::FromJsonFile(const string & filePath)
    {
      bool  rc = false;
      _asMap.clear();
      ifstream  is(filePath);
      if (is) {
        nlohmann::json  json = nlohmann::json::parse(is, nullptr, false);
        if (! json.is_discarded()) {
          auto  it = json.find("ASes");
          if ((it != json.end()) && it->is_array()) {
            rc = true;
            for (const auto & as : *it) {
              ASInfo  asinfo;
              if (asinfo.FromJson(as)) {
                _asMap[asinfo.Number()] = asinfo;
              }
              else {
                Syslog(LOG_ERR, "Failed to load ASInfo from JSON");
                rc = false;
                break;
              }
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    nlohmann::json ASes::ToJson() const
    {
      nlohmann::json  rc;
      if (! _asMap.empty()) {
        rc["ASes"] = nlohmann::json::array();
        size_t  i = 0;
        for (const auto & as : _asMap) {
          rc["ASes"][i++] = as.second.ToJson();
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void ASes::Coalesce()
    {
      for (auto & as : _asMap) {
        size_t  numNets;
        do {
          numNets = as.second.Nets().Size();
          as.second.Nets().Coalesce();
        } while (numNets != as.second.Nets().Size());
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ASes::LoadASDb(const string & asdbFile)
    {
      bool  rc = false;
      AS2Ipv4NetDb  asdb;
      if (asdb.Load(asdbFile)) {
        for (auto asi = _asMap.begin(); asi != _asMap.end(); ++asi) {
          auto  asdbi = asdb.Nets().find(asi->first);
          if (asdbi != asdb.Nets().end()) {
            asi->second.Nets(asdbi->second);
            rc = true;
          }
        }
      }
      return rc;
    }
      
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ASes::Load(const string & jsonFile, const string & asdbFile)
    {
      bool  rc = false;
      if (FromJsonFile(jsonFile)) {
        for (auto & as : _asMap) {
          as.second.Nets().Clear();
        }
        if (LoadASDb(asdbFile)) {
          rc = true;
        }
        else {
          Syslog(LOG_ERR, "Failed to load ASdb file '%s'",
                 asdbFile.c_str());
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ASes::MakePfList(vector<Dwm::Ipv4Prefix> & pfList,                    
                          const vector<Dwm::Ipv4Prefix> & exceptions)
    {
      Ipv4Routes<uint8_t>  routes;
      for (auto & as : _asMap) {
        vector<pair<Ipv4Prefix,uint8_t>> asnets;
        as.second.Nets().SortByKey(asnets);
        for (const auto & asnet : asnets) {
          bool  accept = true;
          for (const auto & exc : exceptions) {
            if (asnet.first.Contains(exc) || exc.Contains(asnet.first)) {
              accept = false;
              break;
            }
          }
          if (accept) {
            routes[asnet.first] = 1;
          }
        }
      }
      pfList.clear();
      routes.Coalesce();
      vector<pair<Ipv4Prefix,uint8_t>>  nets;
      routes.SortByKey(nets);
      for (const auto & net : nets) {
        pfList.push_back(net.first);
      }
      return (! pfList.empty());
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
