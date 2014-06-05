# - Try to find Speex
# Once done this will define
#
#  SPEEX_FOUND - system has Speex
#  SPEEX_INCLUDE_DIRS - the Speex include directory
#  SPEEX_LIBRARIES - Link these to use Speex
#  SPEEX_DEFINITIONS - Compiler switches required for using Speex
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (SPEEX_LIBRARIES AND SPEEX_INCLUDE_DIRS)
  # in cache already
  set(SPEEX_FOUND TRUE)
else (SPEEX_LIBRARIES AND SPEEX_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  include(UsePkgConfig)

  pkgconfig(speex _SpeexIncDir _SpeexLinkDir _SpeexLinkFlags _SpeexCflags)

  set(SPEEX_DEFINITIONS ${_SpeexCflags})

  find_path(SPEEX_INCLUDE_DIR
    NAMES
      speex/speex.h
    PATHS
      ${_SpeexIncDir}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(SPEEX_LIBRARY
    NAMES
      speex
    PATHS
      ${_SpeexLinkDir}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (SPEEX_LIBRARY)
    set(SPEEX_FOUND TRUE)
  endif (SPEEX_LIBRARY)

  set(SPEEX_INCLUDE_DIRS
    ${SPEEX_INCLUDE_DIR}
  )

  if (SPEEX_FOUND)
    set(SPEEX_LIBRARIES
      ${SPEEX_LIBRARIES}
      ${SPEEX_LIBRARY}
    )
  endif (SPEEX_FOUND)

  if (SPEEX_INCLUDE_DIRS AND SPEEX_LIBRARIES)
     set(SPEEX_FOUND TRUE)
  endif (SPEEX_INCLUDE_DIRS AND SPEEX_LIBRARIES)

  if (SPEEX_FOUND)
    if (NOT Speex_FIND_QUIETLY)
      message(STATUS "Found Speex: ${SPEEX_LIBRARIES}")
    endif (NOT Speex_FIND_QUIETLY)
  else (SPEEX_FOUND)
    if (Speex_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Speex")
    endif (Speex_FIND_REQUIRED)
  endif (SPEEX_FOUND)

  # show the SPEEX_INCLUDE_DIRS and SPEEX_LIBRARIES variables only in the advanced view
  mark_as_advanced(SPEEX_INCLUDE_DIRS SPEEX_LIBRARIES)

endif (SPEEX_LIBRARIES AND SPEEX_INCLUDE_DIRS)
