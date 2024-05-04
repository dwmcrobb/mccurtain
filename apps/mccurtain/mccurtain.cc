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
//!  \file mccurtain.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
  #include <sys/resource.h>
}

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

#include "DwmMcCurtainAS2Ipv4NetDb.hh"
#include "DwmMcCurtainRipeAsnTxt.hh"

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool IsValidCountryCode(const std::string & countryCode)
{
  bool      rc = false;
  ifstream  is("/usr/local/etc/country_codes.json");
  if (is) {
    nlohmann::json  jv = nlohmann::json::parse(is, nullptr, false);
    if (! jv.is_discarded()) {
      auto  cit = jv.find("codes");
      if ((cit != jv.end()) && (cit->is_array())) {
        for (auto & el : *cit) {
          auto  a2it = el.find("a2");
          if ((a2it != el.end()) && a2it->is_string()) {
            auto  cc = a2it->get<string>();
            if ((cc.length() == 2) && (countryCode == cc)) {
              rc = true;
              break;
            }
          }
        }
      }
    }
    if (! rc) {
      cerr << "Invalid country code '" << countryCode << "'\n";
    }
  }
  else {
    cerr << "Failed to open '/usr/local/etc/country_codes.json'\n";
  }
  return rc;
}
            
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ShowASes(const Dwm::Ipv4Address & addr)
{
  Dwm::McCurtain::Ipv4Net2ASDb  netdb;
  if (netdb.Load(string(getenv("HOME")) + "/etc/ipv42as.db")) {
    std::vector<std::pair<Dwm::Ipv4Prefix,set<uint32_t>>>  matches;
    if (netdb.Entries().Find(addr, matches)) {
      for (const auto & match : matches) {
        cout << setiosflags(ios::left) << setw(20) << match.first << ' ';
        if (! match.second.empty()) {
          auto  asit = match.second.begin();
          cout << *asit;
          ++asit;
          for (; asit != match.second.end(); ++asit) {
            cout << ',' << *asit;
          }
        }
        cout << '\n';
      }
    }
  }
  else {
    cerr << "Failed to load ipv42as.db\n";
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ShowASesVerbose(const Dwm::Ipv4Address & addr)
{
  Dwm::McCurtain::Ipv4Net2ASDb  netdb;
  if (netdb.Load(string(getenv("HOME")) + "/etc/ipv42as.db")) {
    Dwm::McCurtain::RipeAsnTxt  asntxt;
    asntxt.Load(string(getenv("HOME")) + "/etc/asn.txt");
    std::vector<std::pair<Dwm::Ipv4Prefix,set<uint32_t>>>  matches;
    if (netdb.Entries().Find(addr, matches)) {
      for (const auto & match : matches) {
        cout << match.first << ':' << '\n';
        if (! match.second.empty()) {
          for (auto asit = match.second.begin();
               asit != match.second.end(); ++asit) {
            cout << "  " << *asit;
            auto  txtit = asntxt.Entries().find(*asit);
            if (txtit != asntxt.Entries().end()) {
              cout << ' ' << txtit->second.CountryCode() << ", "
                   << txtit->second.Name();
            }
            cout << '\n';
          }
        }
        // cout << '\n';
      }
    }
  }
  else {
    cerr << "Failed to load ipv42as.db\n";
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetASesForCountry(const std::string & countryCode,
                              vector<uint32_t> & ases)
{
    Dwm::McCurtain::RipeAsnTxt  asntxt;
    if (asntxt.Load(string(getenv("HOME")) + "/etc/asn.txt")) {
      for (const auto & entry : asntxt.Entries()) {
        if (countryCode == entry.second.CountryCode()) {
          ases.push_back(entry.first);
        }
      }
    }
    return (! ases.empty());
}
      
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ShowNetsForCountry(const std::string & country)
{
  vector<uint32_t>  ases;
  if (GetASesForCountry(country, ases)) {
    Dwm::McCurtain::AS2Ipv4NetDb  asdb;
    if (asdb.Load(string(getenv("HOME")) + "/etc/as2ipv4.db")) {
      Dwm::Ipv4Routes<uint8_t>  prefixes;
      for (const auto as : ases) {
        auto  asit = asdb.Nets().find(as);
        if (asit != asdb.Nets().end()) {
          prefixes.Add(asit->second);
        }
      }
      prefixes.Coalesce();
      vector<pair<Dwm::Ipv4Prefix,uint8_t>> sortedPrefixes;
      prefixes.SortByKey(sortedPrefixes);
      for (const auto & pfx : sortedPrefixes) {
        cout << pfx.first << '\n';
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ShowNets(uint32_t as)
{
  Dwm::McCurtain::AS2Ipv4NetDb  asdb;
  if (asdb.Load(string(getenv("HOME")) + "/etc/as2ipv4.db")) {
    auto  asit = asdb.Nets().find(as);
    if (asit != asdb.Nets().end()) {
      auto  netsCopy = asit->second;
      netsCopy.Coalesce();
      std::vector<std::pair<Dwm::Ipv4Prefix,uint8_t>>  prefixes;
      netsCopy.SortByKey(prefixes);
      for (const auto & pfx : prefixes) {
        cout << pfx.first << '\n';
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  cerr << "Usage: " << argv0 << " AS_number\n"
       << "       " << argv0 << " IPv4_address\n"
       << "       " << argv0 << " country_code\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (argc < 2) {
    Usage(argv[0]);
    return 1;
  }
  
  string  arg(argv[1]);
  if (arg.find_first_of('.') != string::npos) {
    Dwm::Ipv4Address  addr(arg);
    ShowASesVerbose(addr);
  }
  else {
    try {
      uint32_t  asNum = stoul(arg);
      ShowNets(asNum);
    }
    catch (...) {
      if (IsValidCountryCode(arg)) {
        ShowNetsForCountry(arg);
      }
      else {
        Usage(argv[0]);
      }
    }
  }

  struct rusage  rusage;
  getrusage(RUSAGE_SELF, &rusage);

  //  cout << "ru_maxrss: " << rusage.ru_maxrss << '\n' << flush;
  return 0;
}
