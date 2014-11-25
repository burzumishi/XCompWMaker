# Process this file with autoconf to produce a configure script.
AC_INIT([wmsystemtray], [1.4])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE()
AM_SILENT_RULES([yes])

DATE=`date '+%B %e, %Y'`
AC_SUBST(DATE)
AC_SUBST(VERSION)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES(Xext,xext)
PKG_CHECK_MODULES(Xfixes,xfixes)
PKG_CHECK_MODULES(Xmuu,xmuu,,[
 PKG_CHECK_MODULES(Xmu,xmu)
])

# Checks for header files.
AC_PATH_XTRA
AC_FIND_XPM
if test "$LINK_XPM" = ""; then
    AC_MSG_ERROR(cannot find libxpm)
fi
AC_HEADER_STDC
if test "$ac_cv_header_stdc" != "yes"; then AC_MSG_WARN(standard C headers not found); fi
AC_CHECK_HEADERS([stdlib.h string.h unistd.h],, AC_MSG_WARN($ac_header not found))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ssize_t])
AC_CHECK_TYPES([sig_atomic_t],,,[[#include <signal.h>]])
AH_BOTTOM(
[#if !HAVE_SSIZE_T
typedef signed ssize_t;
#endif

#if !HAVE_SIG_ATOMIC_T
/* Just a guess */
typedef int sig_atomic_t;
#endif])

# Compatability define
AH_BOTTOM(
[#ifndef __GNUC__
#define  __attribute__(x)  /*NOTHING*/
#endif])


# Checks for library functions.
AC_FUNC_MALLOC
if test "$ac_cv_func_malloc_0_nonnull" != "yes"; then AC_MSG_WARN(malloc() doesn't seem to work); fi
AC_CHECK_FUNCS([atexit dup2 mkdir select strchr strcspn strdup strerror strrchr strspn strstr strtol],, AC_MSG_WARN($ac_func doesn't seem to be available))

X_CFLAGS="$X_CFLAGS \$(Xmu_CFLAGS) \$(Xmuu_CFLAGS) \$(Xfixes_CFLAGS) \$(Xext_CFLAGS)"
X_LIBS="\$(X_PRE_LIBS) $X_LIBS \$(LINK_XPM) -lX11 \$(X_EXTRA_LIBS) \$(Xmu_LIBS) \$(Xmuu_LIBS) \$(Xfixes_LIBS) \$(Xext_LIBS)"

AC_CONFIG_FILES([Makefile wmsystemtray.1])
AC_CONFIG_FILES([b0rken/Makefile])
AC_CONFIG_FILES([m4/Makefile])
AC_OUTPUT
