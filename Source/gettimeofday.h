/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
 
#ifndef __GET_TIME_OF_DAY_H
#define __GET_TIME_OF_DAY_H



namespace RakNet {
    struct TimeZone
    {
        int  tz_minuteswest; /* minutes W of Greenwich */
        int  tz_dsttime;     /* type of dst correction */
    };

    struct TimeVal {
        long    tv_sec;
        long    tv_usec;
    };

    int gettimeofday(TimeVal *tv, TimeZone *tz);
}

#endif
