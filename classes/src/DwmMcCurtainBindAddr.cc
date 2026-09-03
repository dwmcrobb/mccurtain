//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmMcCurtainBindAddr.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmMclogLogger.hh"
#include "DwmMcCurtainBindAddr.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    bool BindAddr::Bind(int fd) const
    {
      if (0 <= fd) {
        if (addr.IsV4()) {
          struct sockaddr_in  inAddr;
          memset(&inAddr, 0, sizeof(inAddr));
          inAddr.sin_family = PF_INET;
          inAddr.sin_addr.s_addr = addr.Addr<Ipv4Address>()->Raw();
          inAddr.sin_port = htons(port);
#ifndef __linux__
          inAddr.sin_len = sizeof(inAddr);
#endif
          if (0 == bind(fd, (sockaddr *)&inAddr, sizeof(inAddr))) {
            return true;
          }
          else {
            MCLOG(LOG_ERR, "bind({}, {}:{}) failed: {}",
                  fd, addr, port, strerror(errno));
          }
        }
        else if (addr.IsV6()) {
          struct sockaddr_in6  inAddr;
          memset(&inAddr, 0, sizeof(inAddr));
          inAddr.sin6_family = PF_INET6;
          inAddr.sin6_addr = *(addr.Addr<Ipv6Address>());
          inAddr.sin6_port = htons(port);
#ifndef __linux__
          inAddr.sin6_len = sizeof(inAddr);
#endif
          if (0 == bind(fd, (sockaddr *)&inAddr, sizeof(inAddr))) {
            return true;
          }
          else {
            MCLOG(LOG_ERR, "bind({}, {}:{}) failed: {}",
                  fd, addr, port, strerror(errno));
          }
        }
      }
      return false;
    }
  
  }  // namespace McCurtain

}  // namespace Dwm
