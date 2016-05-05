/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "gettimeofday.h"

// From http://www.openasthra.com/c-tidbits/gettimeofday-function-for-windows/
#if defined(_MSC_VER)

#include "WindowsIncludes.h"

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
#endif

#include <time.h>

int RakNet::gettimeofday(TimeVal *tv, TimeZone *tz)
{
#if defined(WINDOWS_PHONE_8) || defined(WINDOWS_STORE_RT)
    if (tv)
    {
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);

        struct tm tTm;
        tTm.tm_year = wtm.wYear - 1900;
        tTm.tm_mon = wtm.wMonth - 1;
        tTm.tm_mday = wtm.wDay;
        tTm.tm_hour = wtm.wHour;
        tTm.tm_min = wtm.wMinute;
        tTm.tm_sec = wtm.wSecond;
        tTm.tm_isdst = -1;

        tv->tv_sec = (long)mktime(&tTm);       // time_t is 64-bit on win32
        tv->tv_usec = wtm.wMilliseconds * 1000;
    }
#elif defined(_WIN32)

  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
#else
    return ::gettimeofday((struct timeval*)tv, (struct timezone*)tz);
#endif

  return 0;
}

