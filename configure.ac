#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([F.aceBash], [0.4], [wald.j.chris@gmail.com])
AM_INIT_AUTOMAKE([F.aceBash], [0.4])
AM_PATH_PYTHON([2.7], [], [
   echo "**** Python 2.7 or later is required for this program."
   exit -1])
AC_CONFIG_SRCDIR([src/ArgParse.h])
#AC_CONFIG_HEADERS([src/configure.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcurl':
AC_CHECK_LIB([curl], [curl_easy_init], [], [
   echo "**** libcurl is required for this program."
   exit -1])
# FIXME: Replace `main' with a function in `-lcurlpp':
AC_CHECK_LIB([curlpp], [clone], [], [
   echo "**** curlpp is required for this program."
   exit -1])
# FIXME: Replace `main' with a function in `-ljson':
AC_CHECK_LIB([json], [write], [], [
   echo "**** JsonCpp is required for this program."
   exit -1])
# FIXME: Replace `main' with a function in `-lncurses':
AC_CHECK_LIB([ncurses], [refresh], [], [
   echo "**** ncurses is required for this program."
   exit -1])
# FIXME: Replace `main' with a function in `-lreadline':
AC_CHECK_LIB([readline], [readline], [], [
   echo "**** GNU Readline is required for this program."
   exit -1])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK

AC_CONFIG_FILES([Makefile src/Makefile scripts/Makefile])
AC_OUTPUT