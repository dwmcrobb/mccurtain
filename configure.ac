AC_INIT([mccurtain],m4_esyscmd_s([./getvers.sh -v]),[dwmcrobb@me.com],[mccurtain],[http://www.mcplex.net])

AC_PROG_CC
AC_PROG_CXX
AC_CANONICAL_TARGET

LT_INIT([])

dnl  ###  Read some generally useful macros.
builtin(include, ./dwm.m4)  dnl
builtin(include, ax_check_compile_flag.m4) dnl

dnl  Set variables for building shared library
DWM_SET_CXX_SHARED_FLAGS

dnl  Set variables for pthreads
DWM_SET_PTHREADFLAGS

OSLIBS=""
KVMLIB="-lkvm"
LIBTOOL="libtool"

case $host_os in
  freebsd[[89]]*)
    CXXFLAGS="-O2"
    LDFLAGS="-static"
    ;;
  freebsd1[[01234]]*)
    CXXFLAGS="-O2"
    LDFLAGS="-static"
    ;;
  linux*)
    CXXFLAGS="-O2"
    KVMLIB=""
    LDFLAGS="$LDFLAGS"
    DEBARCH=`dpkg --print-architecture`
    AC_SUBST(DEBARCH)
    ;;
  darwin*)
    CC="clang++"
    CXX="clang++"
    CXXFLAGS="${CXXFLAGS} -O2 -stdlib=libc++"
    KVMLIB=""
    LDFLAGS="${LDFLAGS} -O2 -stdlib=libc++ -std=c++14"
    LIBTOOL="glibtool"
    ;;
esac

echo "host_os ${host_os}"

dnl  Check for C++20
DWM_CHECK_CPLUSPLUS_20

AC_SUBST(KVMLIB)
AC_SUBST(LIBTOOL)
AC_SUBST(OSLIBS)

DWM_REQUIRES_DWMPKG(libDwm,0.9.56)

DWMDIR=`pkg-config --variable=prefix libDwm`
DWMINCS=`pkg-config --cflags-only-I libDwmCredence`
DWMINCS=`dwmcflags ${DWMINCS}`
DWMLIBS=`pkg-config --libs libDwmCredence`
DWMLIBS=`dwmcflags ${DWMLIBS}`

AC_SUBST(DWMDIR)
AC_SUBST(DWMINCS)
AC_SUBST(DWMLIBS)

DWM_CHECK_PKG(dwmwhat,[exit 1])

DWM_CHECK_NEED_LIBATOMIC

dnl  Look for libtermcap
AC_LANG_PUSH(C++)
AC_CHECK_LIB(termcap, tgetent, [TERMCAPLIB=-ltermcap], [exit 1])
AC_LANG_POP()
AC_SUBST(TERMCAPLIB)

dnl  Look for libz
AC_LANG_PUSH(C++)
AC_CHECK_LIB(z, gzwrite, [ZLIB=-lz], [exit 1])
AC_LANG_POP()
AC_SUBST(ZLIB)

dnl ------------------------------------------------------------------------
dnl //  Check for libbz2
dnl ------------------------------------------------------------------------
DWM_PREREQ_PATH(bz2src, [], [bz2 source path])
if [[ -n "$bz2src" ]]; then
   OLDLDFLAGS="${LDFLAGS}"
   LDFLAGS="${LDFLAGS} -L${bz2src}"
   AC_CHECK_LIB(bz2, BZ2_bzwrite, 
                [ BZ2LIB="-L${bz2src} -lbz2"
                  BZ2INC="-I${bz2src}"
                ],
                [exit 1])
   LDFLAGS="${OLDLDFLAGS}"
else
  AC_LANG_PUSH(C++)
  AC_CHECK_LIB(bz2, BZ2_bzwrite, [BZ2LIB=-lbz2], [exit 1])
  AC_LANG_POP()
fi

AC_SUBST(BZ2INC)
AC_SUBST(BZ2LIB)

dnl ------------------------------------------------------------------------
dnl //  Check for libpcap
dnl ------------------------------------------------------------------------
DWM_PREREQ_PATH(pcapsrc, [], [pcap source path])
if [[ -n "$pcapsrc" ]]; then
   OLDLDFLAGS="${LDFLAGS}"
   LDFLAGS="${LDFLAGS} -L${pcapsrc}"
   AC_CHECK_LIB(pcap, pcap_open_offline, 
                [ PCAPLIB="-L${pcapsrc} -lpcap"
                  PCAPINC="-I${pcapsrc}"
                ],
                [exit 1])
   LDFLAGS="${OLDLDFLAGS}"
else
  AC_LANG_PUSH(C++)
  AC_CHECK_LIB(pcap, pcap_open_offline, [PCAPLIB=-lpcap], [])
  AC_LANG_POP()
fi

DWM_CHECK_BOOSTASIO
DWM_CHECK_NEED_LIBIBVERBS

AC_SUBST(LDFLAGS)
AC_SUBST(PCAPINC)
AC_SUBST(PCAPLIB)

dnl DWM_CHECK_STRTOF()

DWM_PREREQ_PATH(htmlman,share/htmlman,[HTML manpage path])

DWM_SET_PKGVARS(mccurtain,[mccurtain-0.0.0])
DWM_GET_TAG([mccurtain])

AC_CHECK_PROG([MANDOC], [mandoc], [mandoc], [])

case $host_os in
  freebsd1*)  
    DWM_CHECK_SQLITE3()
    AC_SUBST(SQLITEDIR)
    ;;
esac

AC_SUBST(CXXFLAGS)

DWM_CHECK_LIBSTDCPPFS

DWM_WHAT_STATUS=`./getvers.sh -S`
AC_SUBST(DWM_WHAT_STATUS)

BUILD_DOCS=""
AC_ARG_ENABLE([docs],[AS_HELP_STRING([--enable-docs],[build documentation])],
	      [BUILD_DOCS="yes"], [])
AC_SUBST(BUILD_DOCS)

AC_CONFIG_FILES([Makefile.vars classes/include/DwmMcCurtainVersion.hh
                 packaging/debcontrol packaging/fbsd_manifest
		 packaging/mccurtain.pc doc/Doxyfile
		 etc/macos/net.mcplex.mccurtaind.plist])
AC_CONFIG_FILES([etc/macos/scripts/postinstall],
		[chmod a+x etc/macos/scripts/postinstall])
AC_CONFIG_FILES([etc/macos/scripts/preinstall],
		[chmod a+x etc/macos/scripts/preinstall])
AC_CONFIG_FILES([etc/freebsd/+POST_INSTALL],
		[chmod a+x etc/freebsd/+POST_INSTALL])
AC_OUTPUT
