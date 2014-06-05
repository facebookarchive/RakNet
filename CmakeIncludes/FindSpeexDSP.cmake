# - Try to find Speex
# Once done this will define
#
#  SPEEXDSP_FOUND - system has Speex
#  SPEEXDSP_INCLUDE_DIRS - the SpeexDSP include directory
#  SPEEXDSP_LIBRARIES - Link these to use Speex
#  SPEEXDSP_DEFINITIONS - Compiler switches required for using Speex
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

#This has been modified from the original


if (SPEEXDSP_LIBRARIES AND SPEEXDSP_INCLUDE_DIRS)
  # in cache already
  set(SPEEXDSP_FOUND TRUE)
else (SPEEXDSP_LIBRARIES AND SPEEXDSP_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  include(UsePkgConfig)

  pkgconfig(speex _SpeexDSPIncDir _SpeexDSPLinkDir _SpeexDSPLinkFlags _SpeexDSPCflags)

  set(SPEEXDSP_DEFINITIONS ${_SpeexDSPCflags})

  find_path(SPEEXDSP_INCLUDE_DIR
    NAMES
      speex/speex_preprocess.h
    PATHS
      ${_SpeexDSPIncDir}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(SPEEXDSP_LIBRARY
    NAMES
      speexdsp
    PATHS
      ${_SpeexDSPLinkDir}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (SPEEXDSP_LIBRARY)
    set(SPEEXDSP_FOUND TRUE)
  endif (SPEEXDSP_LIBRARY)

  set(SPEEXDSP_INCLUDE_DIRS
    ${SPEEXDSP_INCLUDE_DIR}
  )

  if (SPEEXDSP_FOUND)
    set(SPEEXDSP_LIBRARIES
      ${SPEEXDSP_LIBRARIES}
      ${SPEEXDSP_LIBRARY}
    )
  endif (SPEEXDSP_FOUND)

  if (SPEEXDSP_INCLUDE_DIRS AND SPEEXDSP_LIBRARIES)
     set(SPEEXDSP_FOUND TRUE)
  endif (SPEEXDSP_INCLUDE_DIRS AND SPEEXDSP_LIBRARIES)

  if (SPEEXDSP_FOUND)
    if (NOT SPEEXDSP_FIND_QUIETLY)
      message(STATUS "Found SpeexDSP: ${SPEEXDSP_LIBRARIES}")
    endif (NOT SPEEXDSP_FIND_QUIETLY)
  else (SPEEXDSP_FOUND)
    if (SPEEXDSP_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find SpeexDSP")
    endif (SPEEXDSP_FIND_REQUIRED)
  endif (SPEEXDSP_FOUND)

  # show the SPEEXDSP_INCLUDE_DIRS and SPEEXDSP_LIBRARIES variables only in the advanced view
  mark_as_advanced(SPEEXDSP_INCLUDE_DIRS SPEEXDSP_LIBRARIES)

endif (SPEEXDSP_LIBRARIES AND SPEEXDSP_INCLUDE_DIRS)
 
