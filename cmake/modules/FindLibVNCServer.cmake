# cmake macro to test LIBVNCSERVER LIB

# Copyright (c) 2006, Alessandro Praduroux <pradu@pradu.it>
# Copyright (c) 2007, Urs Wolfer <uwolfer @ kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

INCLUDE(CheckPointerMember)

IF (LIBVNCSERVER_INCLUDE_DIR AND LIBVNCSERVER_LIBRARIES)
    # Already in cache, be silent
    SET(LIBVNCSERVER_FIND_QUIETLY TRUE)
ENDIF (LIBVNCSERVER_INCLUDE_DIR AND LIBVNCSERVER_LIBRARIES)

FIND_PATH(LIBVNCSERVER_INCLUDE_DIR rfb/rfb.h)

FIND_LIBRARY(LIBVNCSERVER_LIBRARIES NAMES vncserver libvncserver)

# libvncserver and libvncclient are in the same package, so it does
# not make sense to add a new cmake script for finding libvncclient.
# instead just find the libvncclient also in this file.
FIND_PATH(LIBVNCCLIENT_INCLUDE_DIR rfb/rfbclient.h)
FIND_LIBRARY(LIBVNCCLIENT_LIBRARIES NAMES vncclient libvncclient)

IF (LIBVNCSERVER_INCLUDE_DIR AND LIBVNCSERVER_LIBRARIES)
   SET(CMAKE_REQUIRED_INCLUDES "${LIBVNCSERVER_INCLUDE_DIR}" "${CMAKE_REQUIRED_INCLUDES}")
   CHECK_POINTER_MEMBER(rfbClient* GotXCutText rfb/rfbclient.h LIBVNCSERVER_FOUND)
ENDIF (LIBVNCSERVER_INCLUDE_DIR AND LIBVNCSERVER_LIBRARIES)

IF (LIBVNCSERVER_FOUND)
  IF (NOT LIBVNCSERVER_FIND_QUIETLY)
    MESSAGE(STATUS "Found LibVNCServer: ${LIBVNCSERVER_LIBRARIES}")
  ENDIF (NOT LIBVNCSERVER_FIND_QUIETLY)
ELSE (LIBVNCSERVER_FOUND)
  IF (LIBVNCSERVER_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could NOT find acceptable version of LibVNCServer (version 0.9 or later required).")
  ENDIF (LIBVNCSERVER_FIND_REQUIRED)
ENDIF (LIBVNCSERVER_FOUND)

MARK_AS_ADVANCED(LIBVNCSERVER_INCLUDE_DIR LIBVNCSERVER_LIBRARIES)
