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
//!  \file mccurtaindb.cc
//!  \author Daniel W. McRobb
//!  \brief trivial utility to dump the contents of the mccurtain db file in
//!    human-readable form
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <regex>

#include "DwmMcCurtainAS2IPDb.hh"
#include "DwmMcCurtainIpv4Net2AS.hh"
#include "DwmMcCurtainIpv6Net2AS.hh"
#include "DwmMcCurtainRipeAsnTxt.hh"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool IsIpv4Address(const std::string & s)
{
  const std::regex
    rgx("(25[0-5]|2[01234][0-9]|1[0-9][0-9]|[0-9][0-9]|[0-9])\\."
        "(25[0-5]|2[01234][0-9]|1[0-9][0-9]|[0-9][0-9]|[0-9])\\."
        "(25[0-5]|2[01234][0-9]|1[0-9][0-9]|[0-9][0-9]|[0-9])\\."
        "(25[0-5]|2[01234][0-9]|1[0-9][0-9]|[0-9][0-9]|[0-9])",
        std::regex::ECMAScript|std::regex::optimize);
  std::smatch  sm;
  return std::regex_match(s, sm, rgx);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool IsIpv6Address(const std::string & s)
{
  return (Dwm::Ipv6Address(s) != Dwm::Ipv6Address());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool IsCountryCode(const std::string & s)
{
  if (s.size() == 2) {
    return (std::isupper(s[0]) && std::isupper(s[1]));
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool IsASNumber(const std::string & s)
{
  try {
    auto  u = std::stoul(s);
    return (u <= 0xFFFFFFFF);
  }
  catch (...) {
    return false;
  }
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintAllASes(const Dwm::McCurtain::AS2Ipv4Net & as2ip4,
                         const Dwm::McCurtain::AS2Ipv6Net & as2ip6,
                         const Dwm::McCurtain::RipeAsnTxt & asntxt,
                         bool showV4, bool showV6)
{
  std::set<uint32_t>  asnumbers;
  if (showV4) {
    for (const auto & entry : as2ip4.Nets()) {
      asnumbers.insert(entry.first);
    }
  }
  if (showV6) {
    for (const auto & entry : as2ip6.Nets()) {
      asnumbers.insert(entry.first);
    }
  }
  
  for (const auto as : asnumbers) {
    std::cout << as;
    auto  asnit = asntxt.Entries().find(as);
    if (asnit != asntxt.Entries().end()) {
      std::cout << ' ' << asnit->second.CountryCode()
                << ' ' << asnit->second.Name();
    }
    std::cout << '\n';
    if (showV4) {
      auto  asit4 = as2ip4.Nets().find(as);
      if (asit4 != as2ip4.Nets().end()) {
        for (const auto & pfx : asit4->second) {
          std::cout << "  " << pfx.first << '\n';
        }
      }
    }
    if (showV6) {
      auto  asit6 = as2ip6.Nets().find(as);
      if (asit6 != as2ip6.Nets().end()) {
        for (const auto & pfx : asit6->second) {
          std::cout << "  " << pfx.first << '\n';
        }
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintOneAS(const Dwm::McCurtain::AS2Ipv4Net & as2ip4,
                       const Dwm::McCurtain::AS2Ipv6Net & as2ip6,
                       const Dwm::McCurtain::RipeAsnTxt & asntxt,
                       bool showV4, bool showV6, uint32_t asnum)
{
  bool  printedASNumber = false;

  if (showV4) {
    auto  it4 = as2ip4.Nets().find(asnum);
    if (it4 != as2ip4.Nets().end()) {
      for (const auto & pfx : it4->second) {
        std::cout << pfx.first << '\n';
      }
    }
  }
  if (showV6) {
    auto  it6 = as2ip6.Nets().find(asnum);
    if (it6 != as2ip6.Nets().end()) {
      for (const auto & pfx : it6->second) {
        std::cout << pfx.first << '\n';
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, typename U>
static void PrintPrefixMatches(const Dwm::McCurtain::RipeAsnTxt & asntxt,
                               const U & addr, const std::vector<T> & matches)
{
  if (! matches.empty()) {
    std::cout << addr << ":\n";
    for (const auto & match : matches) {
      std::cout << "  " << match.first << '\n';
      for (const auto & as : match.second) {
        std::cout << "    " << as;
        auto  asnit = asntxt.Entries().find(as);
        if (asnit != asntxt.Entries().end()) {
          std::cout << ' ' << asnit->second.CountryCode()
                    << ' ' << asnit->second.Name();
        }
        std::cout << '\n';
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintIpv4Matches(const Dwm::McCurtain::AS2Ipv4Net & as2ip4,
                             const Dwm::McCurtain::RipeAsnTxt & asntxt,
                             const Dwm::Ipv4Address & addr)
{
  using Dwm::McCurtain::Ipv4Net2AS;
  Ipv4Net2AS  ip42as;
  if (ip42as.Load(as2ip4)) {
    std::vector<Ipv4Net2AS::value_type>  matches;
    if (ip42as.find_matches(addr, matches)) {
      PrintPrefixMatches(asntxt, addr, matches);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintIpv6Matches(const Dwm::McCurtain::AS2Ipv6Net & as2ip6,
                             const Dwm::McCurtain::RipeAsnTxt & asntxt,
                             const Dwm::Ipv6Address & addr)
{
  using Dwm::McCurtain::Ipv6Net2AS;
  Ipv6Net2AS  ip62as;
  if (ip62as.Load(as2ip6)) {
    std::vector<Ipv6Net2AS::value_type>  matches;
    if (ip62as.find_matches(addr, matches)) {
      PrintPrefixMatches(asntxt, addr, matches);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintCountryCodePrefixes(const Dwm::McCurtain::AS2Ipv4Net & as2ip4,
                                     const Dwm::McCurtain::AS2Ipv6Net & as2ip6,
                                     const Dwm::McCurtain::RipeAsnTxt & asntxt,
                                     bool showV4, bool showV6,
                                     const std::string & countryCode)
{
  if (showV4) {
    Dwm::McCurtain::CaidaV4Routeviews::ASMapValue  pfxs4;
    
    for (const auto & as : as2ip4.Nets()) {
      auto  asnit = asntxt.Entries().find(as.first);
      if (asnit != asntxt.Entries().end()) {
        if (asnit->second.CountryCode() == countryCode) {
          for (const auto & pfx : as.second) {
            pfxs4.Insert(pfx.first);
          }
        }
      }
    }
    pfxs4.Aggregate();
    std::set<Dwm::Ipv4Prefix>  pfx4set;
    for (const auto & asms : pfxs4.PrefixSets()) {
      for (const auto & pfx : asms.second) {
        pfx4set.insert(pfx);
      }
    }
    for (const auto & pfx : pfx4set) {
      std::cout << pfx << '\n';
    }
  }

  if (showV6) {
    Dwm::McCurtain::CaidaV6Routeviews::ASMapValue  pfxs6;
    
    for (const auto & as : as2ip6.Nets()) {
      auto  asnit = asntxt.Entries().find(as.first);
      if (asnit != asntxt.Entries().end()) {
        if (asnit->second.CountryCode() == countryCode) {
          for (const auto & pfx : as.second) {
            pfxs6.Insert(pfx.first);
          }
        }
      }
    }
    pfxs6.Aggregate();
    std::set<Dwm::Ipv6Prefix>  pfx6set;
    for (const auto & asms : pfxs6.PrefixSets()) {
      for (const auto & pfx : asms.second) {
        pfx6set.insert(pfx);
      }
    }
    for (const auto & pfx : pfx6set) {
      std::cout << pfx << '\n';
    }
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void PrintCountryCodeASes(const Dwm::McCurtain::RipeAsnTxt & asntxt,
                                 const std::string & countryCode)
{
  std::vector<std::pair<uint32_t,Dwm::McCurtain::RipeAsnTxt::Entry>>  entries;
  for (const auto & asn : asntxt.Entries()) {
    if (asn.second.CountryCode() == countryCode) {
      entries.push_back(asn);
    }
  }
  std::sort(entries.begin(), entries.end(),
            [] (const auto & a, const auto & b)
            { return (a.first < b.first); });
  for (const auto & entry : entries) {
    std::cout << entry.first << ' ' << entry.second.CountryCode()
              << ' ' << entry.second.Name() << '\n';
  }
  return;
}

//----------------------------------------------------------------------------
static void Usage(const char *argv0)
{
  std::cerr << "Usage: " << argv0 << " [-f dbfile] [-a asnTxtFile] [-p] [-4] [-6]\n"
            << "       " << "[ipv4addr|ipv6addr|AS_number|country_code]\n\n"
            << "default dbfile: /usr/local/etc/mccas2ip.db\n"
            << "default asnTxtFile: /usr/local/etc/asn.txt\n";
  return;
}

//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  std::string  dbFile("/usr/local/etc/mccas2ip.db");
  std::string  asnTxtFile("/usr/local/etc/asn.txt");

  bool  requestedV4 = false, showV4 = true;
  bool  requestedV6 = false, showV6 = true;
  bool  showPrefixes = false;
  int   optchar;

  while ((optchar = getopt(argc, argv, "46a:f:p")) != -1) {
    switch (optchar) {
      case '4':
        requestedV4 = true;
        break;
      case '6':
        requestedV6 = true;
        break;
      case 'a':
        asnTxtFile = optarg;
        break;
      case 'f':
        dbFile = optarg;
        break;
      case 'p':
        showPrefixes = true;
        break;
      default:
        Usage(argv[0]);
        exit(1);
        break;
    }
  }
  if (requestedV4) {
    if (! requestedV6) {
      showV6 = false;
    }
  }
  else if (requestedV6) {
    if (! requestedV4) {
      showV4 = false;
    }
  }
  
  Dwm::McCurtain::AS2Ipv4Net  as2ipv4;
  Dwm::McCurtain::AS2Ipv6Net  as2ipv6;
  if (Dwm::McCurtain::AS2IPDb::Load(dbFile, as2ipv4, as2ipv6)) {
    Dwm::McCurtain::RipeAsnTxt  asntxt;
    if (asntxt.Load(asnTxtFile)) {
      if (argc > optind) {
        if (IsIpv6Address(argv[optind])) {
          PrintIpv6Matches(as2ipv6, asntxt, Dwm::Ipv6Address(argv[optind]));
        }
        else if (IsIpv4Address(argv[optind])) {
          PrintIpv4Matches(as2ipv4, asntxt, Dwm::Ipv4Address(argv[optind]));
        }
        else if (IsCountryCode(argv[optind])) {
          if (showPrefixes) {
            PrintCountryCodePrefixes(as2ipv4, as2ipv6, asntxt, showV4,
                                     showV6, argv[optind]);
          }
          else {
            PrintCountryCodeASes(asntxt, argv[optind]);
          }
        }
        else if (IsASNumber(argv[optind])) {
          PrintOneAS(as2ipv4, as2ipv6, asntxt, showV4, showV6,
                     std::stoul(argv[optind]));
        }
      }
      else {
        PrintAllASes(as2ipv4, as2ipv6, asntxt, showV4, showV6);
      }
    }
    else {
      std::cerr << "Failed to load asn.txt file '" << asnTxtFile << "'\n";
      return 1;
    }
  }
  else {
    std::cerr << "Failed to load db file '" << dbFile << "'\n";
    return 1;
  }
  
  return 0;
}
