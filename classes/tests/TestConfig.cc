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
//!  \file TestConfig.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"
#include "DwmMcCurtainConfig.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDefaults(const Dwm::McCurtain::Config & cfg)
{
  UnitAssert(cfg.SyslogFacility() == "daemon");
  UnitAssert(cfg.SyslogLevel() == "info");
  UnitAssert(cfg.Database().Ipv4ToASFile()
             == "/usr/local/etc/ipv42as.db");
  UnitAssert(cfg.Database().ASToIpv4File()
             == "/usr/local/etc/as2ipv4.db");
  UnitAssert(cfg.Database().ASNTxtFile()
             == "/usr/local/etc/asn.txt");
  return;
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestCfg1()
{
  using  batcp = boost::asio::ip::tcp;
  using  batcpep = boost::asio::ip::tcp::endpoint;
  
  Dwm::McCurtain::Config  cfg;
  TestDefaults(cfg);
  if (UnitAssert(cfg.Parse("inputs/cfg1"))) {
    UnitAssert(cfg.SyslogFacility() == "local0");
    UnitAssert(cfg.SyslogLevel() == "info");
    UnitAssert(cfg.SyslogLocations() == false);
    UnitAssert(cfg.Service().KeyDirectory() == "/usr/local/etc/mccurtaind");
    UnitAssert(cfg.Service().Addresses().size() == 2);
    UnitAssert(cfg.Service().Addresses().find(batcpep(batcp::v4(), 2126))
               != cfg.Service().Addresses().end());
    UnitAssert(cfg.Service().Addresses().find(batcpep(batcp::v6(), 2126))
               != cfg.Service().Addresses().end());
    UnitAssert(cfg.Service().AllowedClients().size() == 4);
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("192.168.168/24"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("127.0.0.1/32"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("fd60:3019:f4a:6aaf::0/64"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("::1"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Database().Ipv4ToASFile() == "/usr/local/etc/ipv42as.db");
    UnitAssert(cfg.Database().ASToIpv4File() == "/usr/local/etc/as2ipv4.db");
    UnitAssert(cfg.Database().ASNTxtFile() == "/usr/local/etc/asn.txt");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestCfg2()
{
  using  batcp = boost::asio::ip::tcp;
  using  batcpep = boost::asio::ip::tcp::endpoint;
  
  Dwm::McCurtain::Config  cfg;
  TestDefaults(cfg);
  if (UnitAssert(cfg.Parse("inputs/cfg2"))) {
    UnitAssert(cfg.SyslogFacility() == "local1");
    UnitAssert(cfg.SyslogLevel() == "debug");
    UnitAssert(cfg.SyslogLocations() == true);
    UnitAssert(cfg.Service().KeyDirectory() == "/etc/mccurtaind");
    UnitAssert(cfg.Service().Addresses().size() == 2);
    UnitAssert(cfg.Service().Addresses().find(batcpep(batcp::v4(), 2126))
               != cfg.Service().Addresses().end());
    UnitAssert(cfg.Service().Addresses().find(batcpep(batcp::v6(), 2126))
               != cfg.Service().Addresses().end());
    UnitAssert(cfg.Service().AllowedClients().size() == 4);
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("192.168.168/24"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("127.0.0.1/32"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("fd60:3019:f4a:6aaf::0/64"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Service().AllowedClients().find(Dwm::IpPrefix("::1"))
               != cfg.Service().AllowedClients().end());
    UnitAssert(cfg.Database().Ipv4ToASFile() == "/etc/ipv42as.db");
    UnitAssert(cfg.Database().ASToIpv4File() == "/etc/as2ipv4.db");
    UnitAssert(cfg.Database().ASNTxtFile() == "/etc/asn.txt");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("TestConfig", LOG_PERROR|LOG_PID, "user");

  TestCfg1();
  TestCfg2();

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return 1;
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    return 0;
  }
}

  
