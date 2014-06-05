/* Copyright (C) 2002 Jean-Marc Valin 
   File: speex_jitter.h

   Adaptive jitter buffer for Speex

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef NULL
#define NULL 0
#endif

#include "misc.h"
#include <speex/speex.h>
#include <speex/speex_bits.h>
#include <speex/speex_jitter.h>
#include <stdio.h>

#define LATE_BINS 4

void speex_jitter_init(SpeexJitter *jitter, void *decoder, int sampling_rate)
{
   int i;
   for (i=0;i<SPEEX_JITTER_MAX_BUFFER_SIZE;i++)
   {
      jitter->len[i]=-1;
      jitter->timestamp[i]=-1;
   }

   jitter->dec = decoder;
   speex_decoder_ctl(decoder, SPEEX_GET_FRAME_SIZE, &jitter->frame_size);
   jitter->frame_time = jitter->frame_size;

   speex_bits_init(&jitter->current_packet);
   jitter->valid_bits = 0;

   jitter->buffer_size = 4;

   jitter->pointer_timestamp = -jitter->frame_time * jitter->buffer_size;
   jitter->reset_state = 1;
   jitter->lost_count = 0;
   jitter->loss_rate = 0;
}

void speex_jitter_destroy(SpeexJitter *jitter)
{
   speex_bits_destroy(&jitter->current_packet);
}


void speex_jitter_put(SpeexJitter *jitter, char *packet, int len, int timestamp)
{
   int i,j;
   int arrival_margin;

   if (jitter->reset_state)
   {
      jitter->reset_state=0;
      jitter->pointer_timestamp = timestamp-jitter->frame_time * jitter->buffer_size;
      for (i=0;i<MAX_MARGIN;i++)
      {
         jitter->shortterm_margin[i] = 0;
         jitter->longterm_margin[i] = 0;
      }
      for (i=0;i<SPEEX_JITTER_MAX_BUFFER_SIZE;i++)
      {
         jitter->len[i]=-1;
         jitter->timestamp[i]=-1;
      }
      fprintf(stderr, "reset to %d\n", timestamp);
   }
   
   /* Cleanup buffer (remove old packets that weren't played) */
   for (i=0;i<SPEEX_JITTER_MAX_BUFFER_SIZE;i++)
   {
      if (jitter->timestamp[i]<jitter->pointer_timestamp)
      {
         jitter->len[i]=-1;
         /*if (jitter->timestamp[i] != -1)
            fprintf (stderr, "discarding %d %d\n", jitter->timestamp[i], jitter->pointer_timestamp);*/
      }
   }

   /*Find an empty slot in the buffer*/
   for (i=0;i<SPEEX_JITTER_MAX_BUFFER_SIZE;i++)
   {
      if (jitter->len[i]==-1)
         break;
   }

   /*fprintf(stderr, "%d %d %f\n", timestamp, jitter->pointer_timestamp, jitter->drift_average);*/
   if (i==SPEEX_JITTER_MAX_BUFFER_SIZE)
   {
      int earliest=jitter->timestamp[0];
      i=0;
      for (j=1;j<SPEEX_JITTER_MAX_BUFFER_SIZE;j++)
      {
         if (jitter->timestamp[j]<earliest)
         {
            earliest = jitter->timestamp[j];
            i=j;
         }
      }
      /*fprintf (stderr, "Buffer is full, discarding earliest frame %d (currently at %d)\n", timestamp, jitter->pointer_timestamp);*/
      /*No place left in the buffer*/
      
      /*skip some frame(s) */
      /*return;*/
   }
   
   /* Copy packet in buffer */
   if (len>SPEEX_JITTER_MAX_PACKET_SIZE)
      len=SPEEX_JITTER_MAX_PACKET_SIZE;
   for (j=0;j<len/BYTES_PER_CHAR;j++)
      jitter->buf[i][j]=packet[j];
   jitter->timestamp[i]=timestamp;
   jitter->len[i]=len;
   
   /* Don't count late packets when adjusting the synchro (we're taking care of them elsewhere) */
   /*if (timestamp <= jitter->pointer_timestamp)
   {
      fprintf (stderr, "frame for timestamp %d arrived too late (at time %d)\n", timestamp, jitter->pointer_timestamp);
   }*/

   /* Adjust the buffer size depending on network conditions */
   arrival_margin = (timestamp - jitter->pointer_timestamp - jitter->frame_time);
   
   if (arrival_margin >= -LATE_BINS*jitter->frame_time)
   {
      int int_margin;
      for (i=0;i<MAX_MARGIN;i++)
      {
         jitter->shortterm_margin[i] *= .98;
         jitter->longterm_margin[i] *= .995;
      }
      int_margin = (arrival_margin + LATE_BINS*jitter->frame_time)/jitter->frame_time;
      if (int_margin>MAX_MARGIN-1)
         int_margin = MAX_MARGIN-1;
      if (int_margin>=0)
      {
         jitter->shortterm_margin[int_margin] += .02;
         jitter->longterm_margin[int_margin] += .005;
      }
   }
   
   /*fprintf (stderr, "margin : %d %d %f %f %f %f\n", arrival_margin, jitter->buffer_size, 100*jitter->loss_rate, 100*jitter->late_ratio, 100*jitter->ontime_ratio, 100*jitter->early_ratio);*/
}

void speex_jitter_get(SpeexJitter *jitter, short *out, int *current_timestamp)
{
   int i;
   int ret;
   float late_ratio_short;
   float late_ratio_long;
   float ontime_ratio_short;
   float ontime_ratio_long;
   float early_ratio_short;
   float early_ratio_long;
   
   late_ratio_short = 0;
   late_ratio_long = 0;
   for (i=0;i<LATE_BINS;i++)
   {
      late_ratio_short += jitter->shortterm_margin[i];
      late_ratio_long += jitter->longterm_margin[i];
   }
   ontime_ratio_short = jitter->shortterm_margin[LATE_BINS];
   ontime_ratio_long = jitter->longterm_margin[LATE_BINS];
   early_ratio_short = early_ratio_long = 0;
   for (i=LATE_BINS+1;i<MAX_MARGIN;i++)
   {
      early_ratio_short += jitter->shortterm_margin[i];
      early_ratio_long += jitter->longterm_margin[i];
   }
   if (0&&jitter->pointer_timestamp%1000==0)
   {
      fprintf (stderr, "%f %f %f %f %f %f\n", early_ratio_short, early_ratio_long, ontime_ratio_short, ontime_ratio_long, late_ratio_short, late_ratio_long);
      /*fprintf (stderr, "%f %f\n", early_ratio_short + ontime_ratio_short + late_ratio_short, early_ratio_long + ontime_ratio_long + late_ratio_long);*/
   }
   
   if (late_ratio_short > .1 || late_ratio_long > .03)
   {
      jitter->shortterm_margin[MAX_MARGIN-1] += jitter->shortterm_margin[MAX_MARGIN-2];
      jitter->longterm_margin[MAX_MARGIN-1] += jitter->longterm_margin[MAX_MARGIN-2];
      for (i=MAX_MARGIN-3;i>=0;i--)
      {
         jitter->shortterm_margin[i+1] = jitter->shortterm_margin[i];
         jitter->longterm_margin[i+1] = jitter->longterm_margin[i];         
      }
      jitter->shortterm_margin[0] = 0;
      jitter->longterm_margin[0] = 0;            
      /*fprintf (stderr, "interpolate frame\n");*/
      speex_decode_int(jitter->dec, NULL, out);
      if (current_timestamp)
         *current_timestamp = jitter->pointer_timestamp;
      return;
   }
   
   /* Increment timestamp */
   jitter->pointer_timestamp += jitter->frame_time;
   
   if (late_ratio_short + ontime_ratio_short < .005 && late_ratio_long + ontime_ratio_long < .01 && early_ratio_short > .8)
   {
      jitter->shortterm_margin[0] += jitter->shortterm_margin[1];
      jitter->longterm_margin[0] += jitter->longterm_margin[1];
      for (i=1;i<MAX_MARGIN-1;i++)
      {
         jitter->shortterm_margin[i] = jitter->shortterm_margin[i+1];
         jitter->longterm_margin[i] = jitter->longterm_margin[i+1];         
      }
      jitter->shortterm_margin[MAX_MARGIN-1] = 0;
      jitter->longterm_margin[MAX_MARGIN-1] = 0;      
      /*fprintf (stderr, "drop frame\n");*/
      jitter->pointer_timestamp += jitter->frame_time;
   }

   if (current_timestamp)
      *current_timestamp = jitter->pointer_timestamp;

   /* Send zeros while we fill in the buffer */
   if (jitter->pointer_timestamp<0)
   {
      for (i=0;i<jitter->frame_size;i++)
         out[i]=0;
      return;
   }
   
   /* Search the buffer for a packet with the right timestamp */
   for (i=0;i<SPEEX_JITTER_MAX_BUFFER_SIZE;i++)
   {
      if (jitter->len[i]!=-1 && jitter->timestamp[i]==jitter->pointer_timestamp)
         break;
   }
   
   if (i==SPEEX_JITTER_MAX_BUFFER_SIZE)
   {
      /* No packet found */
      if (jitter->valid_bits)
      {
         /* Try decoding last received packet */
         ret = speex_decode_int(jitter->dec, &jitter->current_packet, out);
         if (ret == 0)
         {
            jitter->lost_count = 0;
            return;
         } else {
            jitter->valid_bits = 0;
         }
      }

      /*fprintf (stderr, "lost/late frame %d\n", jitter->pointer_timestamp);*/
      /*Packet is late or lost*/
      speex_decode_int(jitter->dec, NULL, out);
      jitter->lost_count++;
      if (jitter->lost_count>=25)
      {
         jitter->lost_count = 0;
         jitter->reset_state = 1;
         speex_decoder_ctl(jitter->dec, SPEEX_RESET_STATE, NULL);
      }
      jitter->loss_rate = .999*jitter->loss_rate + .001;
   } else {
      jitter->lost_count = 0;
      /* Found the right packet */
      speex_bits_read_from(&jitter->current_packet, jitter->buf[i], jitter->len[i]);
      jitter->len[i]=-1;
      /* Decode packet */
      ret = speex_decode_int(jitter->dec, &jitter->current_packet, out);
      if (ret == 0)
      {
         jitter->valid_bits = 1;
      } else {
         /* Error while decoding */
         for (i=0;i<jitter->frame_size;i++)
            out[i]=0;
      }
      jitter->loss_rate = .999*jitter->loss_rate;
   }


}

int speex_jitter_get_pointer_timestamp(SpeexJitter *jitter)
{
   return jitter->pointer_timestamp;
}
