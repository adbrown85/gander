# Check for Autoconf
AC_PREREQ([2.67])

# Initialize package information
define([MY_NAME], [Gander])
define([MY_MAJOR_VERSION], [0])
define([MY_MINOR_VERSION], [3])
define([MY_INCREMENTAL_VERSION], [1])
define([MY_VERSION], MY_MAJOR_VERSION.MY_MINOR_VERSION.MY_INCREMENTAL_VERSION)
define([MY_EMAIL], [adb1413@rit.edu])
define([MY_TARNAME], [gander])
define([MY_URL], [http://github.com/adbrown85/gander])
define([MY_DESCRIPTION], ['Take a gander with Gander!'])
define([MY_NAMESPACE], [Gander])
AC_INIT(MY_NAME, MY_VERSION, MY_EMAIL, MY_TARNAME, MY_URL)
AC_CONFIG_SRCDIR([gander.cxx])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_SUBST([PACKAGE_MAJOR_VERSION], MY_MAJOR_VERSION)
AC_SUBST([PACKAGE_MINOR_VERSION], MY_MINOR_VERSION)
AC_SUBST([PACKAGE_INCREMENTAL_VERSION], MY_INCREMENTAL_VERSION)
AC_SUBST([PACKAGE_DESCRIPTION], MY_DESCRIPTION)
AC_SUBST([PACKAGE_NAMESPACE], MY_NAMESPACE)

# Check for system
AC_CANONICAL_HOST

# Check for compilers and languages
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
if test "$host_vendor" = 'apple'; then
    AC_PROG_OBJCXX
fi

# Check for common headers
AC_HEADER_STDBOOL

# Check for tools
AC_PROG_INSTALL
AC_PROG_SED
PKG_PROG_PKG_CONFIG

# Check for GLFW
error_no_glfw() {
    AC_MSG_RESULT([no])
    echo "------------------------------------------------------------"
    echo " GLFW is needed to build MY_NAME."
    echo " Please visit 'http://glfw.org/'."
    echo "------------------------------------------------------------"
    (exit 1); exit 1;
}
AC_MSG_CHECKING([for GLFW])
PKG_CHECK_EXISTS([libglfw], [AC_MSG_RESULT([yes])], [error_no_glfw])
AC_DEFINE([GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED], [1], [Required for GLFW])

# Check for Gloop
define([GLOOP_ID], [gloop-0])
define([GLOOP_VERSION], [0.7.0])
define([GLOOP_REQUIREMENT], [GLOOP_ID >= GLOOP_VERSION])
PACKAGE_REQUIREMENTS+='GLOOP_REQUIREMENT '
error_no_gloop() {
    AC_MSG_RESULT([no])
    echo "------------------------------------------------------------"
    echo " Gloop GLOOP_VERSION is needed."
    echo " Please visit 'http://github.com/adbrown85/gloop'"
    echo "------------------------------------------------------------"
    (exit 1); exit 1;
}
AC_MSG_CHECKING([for Gloop])
PKG_CHECK_EXISTS([GLOOP_REQUIREMENT], [AC_MSG_RESULT([yes])], [error_no_gloop])

# Check for M3d
define([M3D_ID], [m3d-0])
define([M3D_VERSION], [0.3.0])
define([M3D_REQUIREMENT], [M3D_ID >= M3D_VERSION])
PACKAGE_REQUIREMENTS+='M3D_REQUIREMENT '
error_no_m3d() {
    AC_MSG_RESULT([no])
    echo "------------------------------------------------------------"
    echo " M3d M3D_VERSION is needed."
    echo " Please visit 'http://github.com/adbrown85/m3d'"
    echo "------------------------------------------------------------"
    (exit 1); exit 1;
}
AC_MSG_CHECKING([for M3d])
PKG_CHECK_EXISTS([M3D_REQUIREMENT], [AC_MSG_RESULT([yes])], [error_no_m3d])

# Check for Glycerin
define([GLYCERIN_ID], [glycerin-0])
define([GLYCERIN_VERSION], [0.8.0])
define([GLYCERIN_REQUIREMENT], [GLYCERIN_ID >= GLYCERIN_VERSION])
PACKAGE_REQUIREMENTS+='GLYCERIN_REQUIREMENT '
error_no_glycerin() {
    AC_MSG_RESULT([no])
    echo "------------------------------------------------------------"
    echo " Glycerin GLYCERIN_VERSION is needed."
    echo " Please visit 'http://github.com/adbrown85/glycerin'"
    echo "------------------------------------------------------------"
    (exit 1); exit 1;
}
AC_MSG_CHECKING([for Glycerin])
PKG_CHECK_EXISTS([GLYCERIN_REQUIREMENT] [AC_MSG_RESULT([yes])], [error_no_glycerin])

# Check for RapidGL
define([RAPIDGL_ID], [rapidgl-0])
define([RAPIDGL_VERSION], [0.11.1])
define([RAPIDGL_REQUIREMENT], [RAPIDGL_ID >= RAPIDGL_VERSION])
PACKAGE_REQUIREMENTS+='RAPIDGL_REQUIREMENT '
error_no_rapidgl() {
    AC_MSG_RESULT([no])
    echo "------------------------------------------------------------"
    echo " RapidGL RAPIDGL_VERSION is needed."
    echo " Please visit 'http://github.com/adbrown85/rapidgl'"
    echo "------------------------------------------------------------"
    (exit 1); exit 1;
}
AC_MSG_CHECKING([for RapidGL])
PKG_CHECK_EXISTS([RAPIDGL_REQUIREMENT] [AC_MSG_RESULT([yes])], [error_no_rapidgl])

# Check for Poco Foundation
error_no_poco_foundation() {
    echo "------------------------------------------------------------"
    echo " Poco Foundation is needed to build MY_NAME."
    echo " Please visit 'http://www.pocoproject.org'"
    echo "------------------------------------------------------------"
    (exit 1); exit 1;
}
AC_CHECK_HEADER([Poco/String.h], , [error_no_poco_foundation])
AC_CHECK_LIB([PocoFoundation], [exit], , [error_no_poco_foundation])

# Define flags required for OpenGL 3
if test "$host_vendor" = 'apple'; then
    AC_DEFINE([GL3_PROTOTYPES], [1], [Required for using OpenGL 3 on Mac])
else
    AC_DEFINE([GL_GLEXT_PROTOTYPES], [1], [Required for using OpenGL 3 on Linux])
fi

# Finish
AC_CONFIG_FILES([Makefile])
PKG_CHECK_MODULES([DEPS], libglfw GLOOP_ID M3D_ID GLYCERIN_ID RAPIDGL_ID)
AC_SUBST([PACKAGE_REQUIREMENTS])
AC_OUTPUT

# Report
echo \
"--------------------------------------------------------------
 ${PACKAGE_NAME} ${PACKAGE_VERSION}
 ${PACKAGE_DESCRIPTION}

 prefix      ${prefix}
 CXX         ${CXX}
 CPPFLAGS    ${CPPFLAGS}
 CXXFLAGS    ${CXXFLAGS}
 LDFLAGS     ${LDFLAGS}
 DEFS        ${DEFS}

 Now type 'make' then 'sudo make install'
--------------------------------------------------------------"
