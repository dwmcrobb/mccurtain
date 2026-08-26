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
//!  @file TestMessage.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <fstream>
#if defined(__cpp_lib_spanstream)
#  if (__cpp_lib_spanstream >= 202106L)
#    if __has_include(<spanstream>)
#      include <spanstream>
#      define HAVE_STD_SPANSTREAM 1
#    endif
#  endif
#endif

#ifndef HAVE_STD_SPANSTREAM
#  include "DwmDnsSpanstream.hh"
#endif

#include "DwmUnitAssert.hh"
#include "DwmMcCurtainMessage.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool PopulateResponse(McCurtain::Message & msg)
{
  ifstream  is("inputs/origin_response_msg.json");
  if (UnitAssert(is)) {
    nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
    if (UnitAssert(! j.is_discarded())) {
      if (UnitAssert(msg.FromJson(j))) {
        msg.Header().Type(McCurtain::MessageHeader::MsgType::e_typeOriginResponse);
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool PopulateRequest(McCurtain::Message & msg)
{
  ifstream  is("inputs/origin_request_msg.json");
  if (UnitAssert(is)) {
    nlohmann::json  j = nlohmann::json::parse(is, nullptr, false);
    if (UnitAssert(! j.is_discarded())) {
      if (UnitAssert(msg.FromJson(j))) {
        McCurtain::MessageHeader  hdr;
        msg.Header().Type(McCurtain::MessageHeader::MsgType::e_typeOriginRequest);
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSendRecv()
{
  int  recvfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (UnitAssert(0 <= recvfd)) {
    sockaddr_in  recvAddr;
    memset(&recvAddr, 0, sizeof(recvAddr));
    recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    recvAddr.sin_port = htons(8647);
    recvAddr.sin_family = PF_INET;
#ifndef __linux__
    recvAddr.sin_len = sizeof(recvAddr);
#endif    
    socklen_t    recvAddrLen = sizeof(recvAddr);
    if (UnitAssert(0 == ::bind(recvfd, (sockaddr *)&recvAddr, recvAddrLen))) {
      if (UnitAssert(0 == getsockname(recvfd, (sockaddr *)&recvAddr,
                                      &recvAddrLen))) {
        int  sendfd = socket(PF_INET, SOCK_DGRAM, 0);
        if (UnitAssert(0 <= sendfd)) {
          McCurtain::Message  msg;
          if (UnitAssert(PopulateRequest(msg))) {
            struct sockaddr_in  sendAddr = recvAddr;
            if (UnitAssert(0 < msg.SendTo(sendfd, &sendAddr))) {
              McCurtain::Message  msg2;
              sockaddr_in  fromAddr;
              if (UnitAssert(0 < msg2.RecvFrom(recvfd, &fromAddr))) {
                UnitAssert(msg2 == msg);
              }
            }
          }
          ::close(sendfd);
        }
      }
    }
    else {
      std::cerr << "bind(" << recvfd << ") failed: " << strerror(errno)
                << '\n';
    }
    ::close(recvfd);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  McCurtain::Message        msg;
  McCurtain::MessageHeader  hdr;
  hdr.Type(McCurtain::MessageHeader::MsgType::e_typeOriginResponse);
  hdr.Truncated(false);
  hdr.Id(8647);

  msg.Header(hdr);
  if (UnitAssert(PopulateResponse(msg))) {
    char  buf[4096];
    std::spanstream  ss{std::span{buf,sizeof(buf)}};
    if (UnitAssert(msg.Write(ss))) {
      McCurtain::Message  msg2;
      if (UnitAssert(msg2.Read(ss))) {
        UnitAssert(msg2 == msg);
      }
    }
  }

  hdr.Type(McCurtain::MessageHeader::MsgType::e_typeOriginRequest);
  msg.Header(hdr);
  if (UnitAssert(PopulateRequest(msg))) {
    char  buf[4096];
    std::stringstream  ss;
    if (UnitAssert(msg.Write(ss))) {
      McCurtain::Message  msg2;
      if (UnitAssert(msg2.Read(ss))) {
        UnitAssert(msg2 == msg);
      }
    }
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestJsonIO()
{
  McCurtain::Message        msg;
  McCurtain::MessageHeader  hdr;
  hdr.Truncated(false);
  hdr.Type(McCurtain::MessageHeader::MsgType::e_typeOriginResponse);
  hdr.Id(8647);

  msg.Header(hdr);
  if (UnitAssert(PopulateResponse(msg))) {
    char  buf[4096];
    std::spanstream  ss{std::span{buf,sizeof(buf)}};
    if (UnitAssert(msg.Write(ss))) {
      McCurtain::Message  msg2;
      if (UnitAssert(msg2.Read(ss))) {
        UnitAssert(msg2 == msg);
      }
    }
  }

  hdr.Type(McCurtain::MessageHeader::MsgType::e_typeOriginRequest);
  msg.Header(hdr);
  if (UnitAssert(PopulateRequest(msg))) {
    char  buf[4096];
    memset(buf, 0, sizeof(buf));
    std::spanstream  ss{std::span{buf,sizeof(buf)}};
    if (UnitAssert(msg.Write(ss))) {
      McCurtain::Message  msg2;
      if (UnitAssert(msg2.Read(ss))) {
        UnitAssert(msg2 == msg);
      }
    }
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIO();
  TestJsonIO();
  TestSendRecv();
  
  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;

  exit(0);
  
testsFailed:
  Assertions::Print(cerr, true);
  exit(1);
}
