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
//!  \file mkcurtain.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "DwmIpv4Routes.hh"

using namespace std;

using ASNetworksMap =
  unordered_map<uint32_t,pair<string,Dwm::Ipv4Routes<uint32_t>>>;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool LoadASesFromJson(const std::string & fileName,
                      ASNetworksMap & ases)
{
  bool  rc = false;
  ases.clear();
  std::ifstream  is(fileName);
  if (is) {
    nlohmann::json  json =
      nlohmann::json::parse(is, nullptr, false);
    if (! json.is_discarded()) {
      auto  it = json.find("ASes");
      if ((it != json.end()) && it->is_array()) {
        for (const auto & as : *it) {
          auto  asnum = as.find("AS");
          if ((asnum != as.end()) && asnum->is_number()) {
            auto  asname = as.find("name");
            if ((asname != as.end()) && asname->is_string()) {
              ases[asnum->get<uint32_t>()].first = asname->get<string>();
            }
          }
        }
      }
    }
  }
  return (! ases.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ASNetworksMap  ases;
  LoadASesFromJson(argv[1], ases);
  cout << "ases.size(): " << ases.size() << '\n';
}
