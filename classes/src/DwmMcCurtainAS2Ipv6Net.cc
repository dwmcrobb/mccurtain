//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024, 2026
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
//!  \file DwmMcCurtainAS2Ipv6Net.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::McCurtain::AS2Ipv6Net implementation
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>

#include "DwmStreamIO.hh"
#include "DwmMcCurtainAS2Ipv6Net.hh"

namespace Dwm {

  namespace McCurtain {

    using namespace std;
    namespace fs = std::filesystem;
    
    //------------------------------------------------------------------------
    AS2Ipv6Net::AS2Ipv6Net(const CaidaV6Routeviews & rv)
        : _asNets()
    {
      Load(rv);
    }

    //------------------------------------------------------------------------
    void AS2Ipv6Net::Load(const CaidaV6Routeviews & rv)
    {
      _asNets.clear();
      for (const auto & asSet : rv.ASSets()) {
        for (auto as : asSet.first) {
          for (const auto & pfxSet : asSet.second.PrefixSets()) {
            for (const auto & pfx : pfxSet.second) {
              _asNets[as][pfx] = 1;
            }
          }
        }
      }
      return;
    }

    //------------------------------------------------------------------------
    static bool LoadViaIstream(const fs::path & path, AS2Ipv6Net & as2ip6)
    {
      bool  rc = false;
      ifstream  is(path);
      if (is) {
        if (as2ip6.Read(is)) {
          rc = true;
        }
        is.close();
      }
      return rc;
    }

    //------------------------------------------------------------------------
    static bool LoadViaBZ2(const fs::path & path, AS2Ipv6Net & as2ip6)
    {
      bool     rc = false;
      BZFILE  *bzf = BZ2_bzopen(path.c_str(), "rb");
      if (bzf) {
        rc = as2ip6.BZRead(bzf);
        BZ2_bzclose(bzf);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    static bool LoadViaGZ(const fs::path & path, AS2Ipv6Net & as2ip6)
    {
      bool    rc = false;
      gzFile  gzf = gzopen(path.c_str(), "rb");
      if (gzf) {
        rc = as2ip6.Read(gzf);
        gzclose(gzf);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2Ipv6Net::Load(const std::string & pathstr)
    {
      bool      rc = false;
      fs::path  path(pathstr);
      if (path.extension() == ".bz2")     { rc = LoadViaBZ2(path, *this); }
      else if (path.extension() == ".gz") { rc = LoadViaGZ(path, *this); }
      else                                { rc = LoadViaIstream(path, *this); }

      return rc;
    }

    //------------------------------------------------------------------------
    static bool SaveViaOstream(const fs::path & path,
                               const AS2Ipv6Net & as2ip6)
    {
      bool  rc = false;
      ofstream  os(path);
      if (os) {
        if (as2ip6.Write(os)) {
          rc = true;
        }
        os.close();
      }
      return rc;
    }

    //------------------------------------------------------------------------
    static bool SaveViaBZ2(const fs::path & path, const AS2Ipv6Net & as2ip6)
    {
      bool     rc = false;
      BZFILE  *bzf = BZ2_bzopen(path.c_str(), "wb");
      if (bzf) {
        if (as2ip6.BZWrite(bzf) > 0) {
          rc = true;
        }
        BZ2_bzclose(bzf);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    static bool SaveViaGZ(const fs::path & path, const AS2Ipv6Net & as2ip6)
    {
      bool    rc = false;
      gzFile  gzf = gzopen(path.c_str(), "wb");
      if (gzf) {
        if (as2ip6.Write(gzf) > 0) {
          rc = true;
        }
        gzclose(gzf);
      }
      return rc;
    }
        
    //------------------------------------------------------------------------
    bool AS2Ipv6Net::Save(const std::string & pathstr) const
    {
      bool      rc = false;
      fs::path  path(pathstr);
      if (path.extension() == ".bz2")     { rc = SaveViaBZ2(path, *this);     }
      else if (path.extension() == ".gz") { rc = SaveViaGZ(path, *this);      }
      else                                { rc = SaveViaOstream(path, *this); }

      return rc;
    }

    //------------------------------------------------------------------------
    std::istream & AS2Ipv6Net::Read(std::istream & is)
    {
      return StreamIO::Read(is, _asNets);
    }
    
    //------------------------------------------------------------------------
    std::ostream & AS2Ipv6Net::Write(std::ostream & os) const
    {
      return StreamIO::Write(os, _asNets);
    }

    //------------------------------------------------------------------------
    ssize_t AS2Ipv6Net::Read(int fd)
    {
      return DescriptorIO::Read(fd, _asNets);
    }
    
    //------------------------------------------------------------------------
    ssize_t AS2Ipv6Net::Write(int fd) const
    {
      return DescriptorIO::Write(fd, _asNets);
    }
    
    //------------------------------------------------------------------------
    size_t AS2Ipv6Net::Read(FILE *f)
    {
      return FileIO::Read(f, _asNets);
    }
    
    //------------------------------------------------------------------------
    size_t AS2Ipv6Net::Write(FILE *f) const
    {
      return FileIO::Write(f, _asNets);
    }
    
    //------------------------------------------------------------------------
    int AS2Ipv6Net::BZRead(BZFILE *bzf)
    {
      return BZ2IO::BZRead(bzf, _asNets);
    }
    
    //------------------------------------------------------------------------
    int AS2Ipv6Net::BZWrite(BZFILE *bzf) const
    {
      return BZ2IO::BZWrite(bzf, _asNets);
    }
    
    //------------------------------------------------------------------------
    int AS2Ipv6Net::Read(gzFile gzf)
    {
      return GZIO::Read(gzf, _asNets);
    }
    
    //------------------------------------------------------------------------
    int AS2Ipv6Net::Write(gzFile gzf) const
    {
      return GZIO::Write(gzf, _asNets);
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
