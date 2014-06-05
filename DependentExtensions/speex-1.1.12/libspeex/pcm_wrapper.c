/* Copyright (C) 2005 Jean-Marc Valin */
/**
  @file pcm_wrapper.c
  @brief PCM encoding wrapped as a Speex mode
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "speex/pcm_wrapper.h"
#include "misc.h"


typedef struct {
   const SpeexMode *mode;
   int frame_size;
   int format;
} PCMState;

/** Initializes encoder state*/
static void *pcm_encoder_init(const SpeexMode *m)
{
   PCMState *st = (PCMState*)speex_alloc(sizeof(PCMState));
   st->mode = m;
   st->frame_size = 64;
   return st;
}

/** De-allocates encoder state resources*/
static void pcm_encoder_destroy(void *state)
{
   speex_free(state);
}

/** Encodes one frame*/
static int pcm_encode(void *state, void *vin, SpeexBits *bits)
{
   int i;
   PCMState *st = (PCMState*)state;
   spx_word16_t *in = vin;
   for (i=0;i<st->frame_size;i++)
   {
      spx_int16_t x;
      x = in[i];
      speex_bits_pack(bits, x, 16);
   }
   return 0;
}

/** Initializes decoder state*/
static void *pcm_decoder_init(const SpeexMode *m)
{
   PCMState *st = (PCMState*)speex_alloc(sizeof(PCMState));
   st->mode = m;
   st->frame_size = 64;
   return st;
}

/** De-allocates decoder state resources*/
static void pcm_decoder_destroy(void *state)
{
   speex_free(state);
}

/** Decodes one frame*/
static int pcm_decode(void *state, SpeexBits *bits, void *vout)
{
   int i;
   PCMState *st = (PCMState*)state;
   spx_word16_t *out = vout;
   for (i=0;i<st->frame_size;i++)
   {
      spx_int16_t x;
      x = speex_bits_unpack_signed(bits, 16);
      out[i] = x;
   }
   return 0;
}

/** ioctl-like function for controlling a narrowband encoder */
static int pcm_encoder_ctl(void *state, int request, void *ptr)
{
   PCMState *st;
   st=(PCMState*)state;
   switch(request)
   {
      case PCM_SET_FRAME_SIZE:
         st->frame_size = (*(int*)ptr);
         break;
      case PCM_GET_FRAME_SIZE:
         (*(int*)ptr) = st->frame_size;
         break;
      default:
         speex_warning_int("Unknown nb_ctl request: ", request);
         return -1;
   }
   return 0;
}

/** ioctl-like function for controlling a narrowband decoder */
static int pcm_decoder_ctl(void *state, int request, void *ptr)
{
   PCMState *st;
   st=(PCMState*)state;
   switch(request)
   {
      case PCM_SET_FRAME_SIZE:
         st->frame_size = (*(int*)ptr);
         break;
      case PCM_GET_FRAME_SIZE:
         (*(int*)ptr) = st->frame_size;
         break;
      default:
         speex_warning_int("Unknown nb_ctl request: ", request);
         return -1;
   }
   return 0;
}

typedef struct {
} PCMMode;

static PCMMode pcmmode;

int pcm_mode_query(const void *mode, int request, void *ptr)
{
   /*const PCMMode *m = (const PCMMode*)mode;*/
   
   switch (request)
   {
      default:
         speex_warning_int("Unknown nb_mode_query request: ", request);
         return -1;
   }
   return 0;
}
/* Default mode for narrowband */
const SpeexMode pcm_wrapper_mode = {
   &pcmmode,
   pcm_mode_query,
   "PCM",
   0,
   4,
   &pcm_encoder_init,
   &pcm_encoder_destroy,
   &pcm_encode,
   &pcm_decoder_init,
   &pcm_decoder_destroy,
   &pcm_decode,
   &pcm_encoder_ctl,
   &pcm_decoder_ctl,
};

const SpeexMode *speex_pcm_wrapper = &pcm_wrapper_mode;
