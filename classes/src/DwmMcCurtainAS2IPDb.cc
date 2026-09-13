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
//!  @file DwmMcCurtainAS2IPDb.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>

#include "DwmMclogLogger.hh"
#include "DwmMcCurtainAS2IPDb.hh"

namespace Dwm {

  namespace McCurtain {

    namespace fs = std::filesystem;
    
    //------------------------------------------------------------------------
    bool AS2IPDb::Load(const std::string & filepath,
                       AS2Ipv4Net & as2ip4,
                       AS2Ipv6Net & as2ip6)
    {
      bool  rc = false;
      fs::path  path(filepath);
      if (path.extension() == ".bz2") {
        rc = LoadBZ2(filepath, as2ip4, as2ip6);
      }
      else if (path.extension() == ".gz") {
        rc = LoadGZ(filepath, as2ip4, as2ip6);
      }
      else {
        rc = LoadIstream(filepath, as2ip4, as2ip6);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2IPDb::Save(const std::string & filepath,
                       const AS2Ipv4Net & as2ip4,
                       const AS2Ipv6Net & as2ip6)
    {
      bool      rc = false;
      fs::path  path(filepath);
      if (path.extension() == ".bz2") {
        rc = SaveBZ2(filepath, as2ip4, as2ip6);
      }
      else if (path.extension() == ".gz") {
        rc = SaveGZ(filepath, as2ip4, as2ip6);
      }
      else {
        rc = SaveOstream(filepath, as2ip4, as2ip6);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2IPDb::LoadIstream(const std::string & filepath,
                              AS2Ipv4Net & as2ip4,
                              AS2Ipv6Net & as2ip6)
    {
      bool           rc = false;
      std::ifstream  is(filepath);
      if (is) {
        if (as2ip4.Read(is)) {
          if (as2ip6.Read(is)) {
            rc = true;
          }
          else {
            MCLOG(LOG_ERR, "Failed to read as2ip6 from '{}'", filepath);
          }
        }
        else {
          MCLOG(LOG_ERR, "Failed to read as2ip4 from '{}'", filepath);
        }
        is.close();
      }
      else {
        MCLOG(LOG_ERR, "Failed to open '{}'", filepath);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2IPDb::LoadBZ2(const std::string & filepath,
                          AS2Ipv4Net & as2ip4,
                          AS2Ipv6Net & as2ip6)
    {
      bool     rc = false;
      BZFILE  *bzf = BZ2_bzopen(filepath.c_str(), "rb");
      if (bzf) {
        if (as2ip4.BZRead(bzf)) {
          if (as2ip6.BZRead(bzf)) {
            rc = true;
          }
          else {
            MCLOG(LOG_ERR, "Failed to read as2ip6 from '{}'", filepath);
          }
        }
        else {
          MCLOG(LOG_ERR, "Failed to read as2ip4 from '{}'", filepath);
        }
        BZ2_bzclose(bzf);
      }
      else {
        MCLOG(LOG_ERR, "BZ2_bzopen('{}',\"rb\") failed", filepath);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2IPDb::LoadGZ(const std::string & filepath,
                         AS2Ipv4Net & as2ip4,
                         AS2Ipv6Net & as2ip6)
    {
      bool    rc = false;
      gzFile  gzf = gzopen(filepath.c_str(), "rb");
      if (gzf) {
        if (as2ip4.Read(gzf)) {
          if (as2ip6.Read(gzf)) {
            rc = true;
          }
          else {
            MCLOG(LOG_ERR, "Failed to read as2ip6 from '{}'", filepath);
          }
        }
        else {
          MCLOG(LOG_ERR, "Failed to read as2ip4 from '{}'", filepath);
        }
        gzclose(gzf);
      }
      else {
        MCLOG(LOG_ERR, "gzopen(\"{}\",\"rb\") failed", filepath);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2IPDb::SaveOstream(const std::string & filepath,
                              const AS2Ipv4Net & as2ip4,
                              const AS2Ipv6Net & as2ip6)
    {
      bool           rc = false;
      std::ofstream  os(filepath);
      if (os) {
        if (as2ip4.Write(os)) {
          if (as2ip6.Write(os)) {
            rc = true;
          }
          else {
            MCLOG(LOG_ERR, "Failed to write as2ip6 to '{}'", filepath);
          }
        }
        else {
          MCLOG(LOG_ERR, "Failed to write as2ip4 to '{}'", filepath);
        }
        os.close();
      }
      else {
        MCLOG(LOG_ERR, "Failed to open '{}'", filepath);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    bool AS2IPDb::SaveBZ2(const std::string & filepath,
                          const AS2Ipv4Net & as2ip4,
                          const AS2Ipv6Net & as2ip6)
    {
      bool     rc = false;
      BZFILE  *bzf = BZ2_bzopen(filepath.c_str(), "wb");
      if (bzf) {
        if (as2ip4.BZWrite(bzf)) {
          if (as2ip6.BZWrite(bzf)) {
            rc = true;
          }
          else {
            MCLOG(LOG_ERR, "Failed to write as2ip6 to '{}'", filepath);
          }
        }
        else {
          MCLOG(LOG_ERR, "Failed to write as2ip4 to '{}'", filepath);
        }
        BZ2_bzclose(bzf);
      }
      else {
        MCLOG(LOG_ERR, "BZ2_bzopen(\"{}\",\"wb\") failed", filepath);
      }
      return rc;
    }
    
    
    //------------------------------------------------------------------------
    bool AS2IPDb::SaveGZ(const std::string & filepath,
                         const AS2Ipv4Net & as2ip4,
                         const AS2Ipv6Net & as2ip6)
    {
      bool    rc = false;
      gzFile  gzf = gzopen(filepath.c_str(), "wb");
      if (gzf) {
        if (as2ip4.Write(gzf)) {
          if (as2ip6.Write(gzf)) {
            rc = true;
          }
          else {
            MCLOG(LOG_ERR, "Failed to write as2ip6 to '{}'", filepath);
          }
        }
        else {
          MCLOG(LOG_ERR, "Failed to write as2ip4 to '{}'", filepath);
        }
        gzclose(gzf);
      }
      else {
        MCLOG(LOG_ERR, "gzopen(\"{}\",\"wb\") failed", filepath);
      }
      return rc;
    }
    
  }  // namespace McCurtain

}  // namespace Dwm
