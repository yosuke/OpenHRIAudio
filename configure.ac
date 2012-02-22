#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.56])
AC_INIT([OpenHRIAudio], [2.00], [yosuke.matsusaka@aist.go.jp])
AM_INIT_AUTOMAKE([1.10 tar-ustar filename-length-max=299])
LT_INIT

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC_STDC
AC_PROG_RANLIB
AC_PROG_CXX

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([OPENRTM], [openrtm-aist >= 1.0.0])
AC_SUBST(OPENRTM_CFLAGS)
AC_SUBST(OPENRTM_LIBS)
PKG_CHECK_MODULES([PULSEAUDIO], [libpulse-simple])
AC_SUBST(PULSEAUDIO_CFLAGS)
AC_SUBST(PULSEAUDIO_LIBS)
PKG_CHECK_MODULES([GTKMM], [gtkmm-2.4],
   [AC_DEFINE([HAVE_GTKMM], [1], [Use gtkmm])])
AC_SUBST(GTKMM_CFLAGS)
AC_SUBST(GTKMM_LIBS)
AC_CHECK_LIB(portaudio, Pa_OpenStream)
AC_CHECK_LIB(speexdsp, speex_preprocess_state_init)
AC_CHECK_LIB(resample, resample_open)
AC_CHECK_LIB(portmixer, Px_OpenMixer)
AC_CHECK_LIB(sndfile, sf_open)

# Check for gettext
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Check for operating system
case ${host_os} in
	*mingw32*)
		host_os_mingw32=yes
		AC_CHECK_TOOL([WINDRES], [windres], AC_MSG_ERROR([windres not found]))
		WIN32_LIBS='-lwinmm -lwsock32'
		AC_SUBST([WIN32_LIBS], [${WIN32_LIBS}])
		;;
	*)
		LDFLAGS="-rdynamic ${LDFLAGS}"
		;;
esac
AM_CONDITIONAL([MINGW32], test "x$host_os_mingw32" = "xyes")

# Commandline options - enable:
AC_ARG_ENABLE([installer],
	AS_HELP_STRING([--enable-installer], [Build NSIS installer [no]]),
	[ enable_installer=${enableval} ], [ enable_installer=no ])
AM_CONDITIONAL(INSTALLER, test "x$enable_installer" = "xyes")
AC_MSG_CHECKING([whether to build NSIS installer])
AC_MSG_RESULT([${enable_installer}])

if test "x$enable_installer" = "xyes" ; then
	AC_CHECK_TOOL([MAKENSIS], [makensis], [no])
	if test "x$MAKENSIS" = "xno" ; then
		AC_MSG_ERROR([makensis not found])
	fi
fi

# some options for speexdsp
FFT_PKGCONFIG=smallft
AC_DEFINE([FLOATING_POINT], [], [Compile as floating-point])
AC_DEFINE([USE_SMALLFT], [], [Use FFT from OggVorbis])
AC_SUBST(FFT_PKGCONFIG)

#PKG_CHECK_MODULES([OPENRTM], [OpenRTM-aist >= 1.0])

AC_CONFIG_HEADER([src/config.h])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 po/Makefile.in
                 pkg/Makefile
                 pkg/nsis/Makefile
                 src/thirdparty/Makefile
                 src/thirdparty/webrtc/Makefile
                 src/thirdparty/webrtc/common_audio/Makefile])

AC_OUTPUT
