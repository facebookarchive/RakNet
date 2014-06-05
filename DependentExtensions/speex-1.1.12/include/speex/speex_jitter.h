/* Copyright (C) 2002 Jean-Marc Valin */
/**
   @file speex_jitter.h
   @brief Adaptive jitter buffer for Speex
*/
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef SPEEX_JITTER_H
#define SPEEX_JITTER_H

#include "speex.h"
#include "speex_bits.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPEEX_JITTER_MAX_PACKET_SIZE 1500 /**< Maximum number of bytes per packet         */
#define SPEEX_JITTER_MAX_BUFFER_SIZE 20   /**< Maximum number of packets in jitter buffer */

#define MAX_MARGIN 12  /**< Number of bins in margin histogram */

/** Speex jitter-buffer state. */
typedef struct SpeexJitter {
   int buffer_size;                                                       /**< Buffer size                         */
   int pointer_timestamp;                                                 /**< Pointer timestamp                   */

   SpeexBits current_packet;                                              /**< Current Speex packet                */
   int valid_bits;                                                        /**< True if Speex bits are valid        */

   char buf[SPEEX_JITTER_MAX_BUFFER_SIZE][SPEEX_JITTER_MAX_PACKET_SIZE];  /**< Buffer of packets                   */
   int timestamp[SPEEX_JITTER_MAX_BUFFER_SIZE];                           /**< Timestamp of packet                 */
   int len[SPEEX_JITTER_MAX_BUFFER_SIZE];                                 /**< Number of bytes in packet           */

   void *dec;                                                             /**< Pointer to Speex decoder            */
   int frame_size;                                                        /**< Frame size of Speex decoder         */
   int frame_time;                                                        /**< Frame time in [ms] of Speex decoder */
   int reset_state;                                                       /**< True if Speex state was reset       */
   
   int lost_count;                                                        /**< Number of lost packets              */
   float shortterm_margin[MAX_MARGIN];                                    /**< Short term margins                  */
   float longterm_margin[MAX_MARGIN];                                     /**< Long term margins                   */
   float loss_rate;                                                       /**< Loss rate                           */
} SpeexJitter;

/** Initialise jitter buffer */
void speex_jitter_init(SpeexJitter *jitter, void *decoder, int sampling_rate);

/** Destroy jitter buffer */
void speex_jitter_destroy(SpeexJitter *jitter);

/** Put one packet into the jitter buffer */
void speex_jitter_put(SpeexJitter *jitter, char *packet, int len, int timestamp);

/** Get one packet from the jitter buffer */
void speex_jitter_get(SpeexJitter *jitter, short *out, int *current_timestamp);

/** Get pointer timestamp of jitter buffer */
int speex_jitter_get_pointer_timestamp(SpeexJitter *jitter);

#ifdef __cplusplus
}
#endif


#endif
