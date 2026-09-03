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
//!  @file DwmMcCurtainBindAddr.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::McCurtain::BindAddr class definition
//---------------------------------------------------------------------------

#ifndef _DWMMCCURTAINBINDADDR_HH_
#define _DWMMCCURTAINBINDADDR_HH_

#include "DwmIpAddress.hh"

namespace Dwm {

  namespace McCurtain {

    //------------------------------------------------------------------------
    //!  Encapsulate an IP address and port number, used for binding a UDP
    //!  socket.
    //------------------------------------------------------------------------
    class BindAddr
    {
    public:
      IpAddress  addr;
      uint16_t   port;

      BindAddr() = default;
      BindAddr(const BindAddr &) = default;
      BindAddr(BindAddr &&) = default;
      BindAddr & operator = (const BindAddr &) = default;
      BindAddr & operator = (BindAddr &&) = default;
      ~BindAddr() = default;
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      BindAddr(const IpAddress & a, uint16_t p)
          : addr(a), port(p)
      {}
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator < (const BindAddr & ba) const
      {
        if (addr < ba.addr) {
          return true;
        }
        else if (addr == ba.addr) {
          return port < ba.port;
        }
        return false;
      }

      bool Bind(int fd) const;

    private:
      bool BindV4(int fd) const;
      bool BindV6(int fd) const;
    };
    
  }  // namespace McCurtain

}  // namespace Dwm

#endif  // _DWMMCCURTAINBINDADDR_HH_
