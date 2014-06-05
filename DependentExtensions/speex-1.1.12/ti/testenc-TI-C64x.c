/* Copyright (C) 2005 Psi Systems, Inc.
   Author:  Jean-Marc Valin 
   File: testenc-TI-C64x.c
   Encoder/Decoder Loop Main file for TI TMS320C64xx processor
   for use with TI Code Composer (TM) DSP development tools.
   Modified from speexlib/testenc.c


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

#include <speex/speex.h>
#include <stdio.h>
#include <stdlib.h>
#include <speex/speex_callbacks.h>

#ifdef FIXED_DEBUG
extern long long spx_mips;
#endif

#ifdef MANUAL_ALLOC
#pragma DATA_SECTION(spxHeap, ".myheap"); 
static char spxHeap[SPEEX_PERSIST_STACK_SIZE];

#pragma DATA_SECTION(spxScratch, ".myheap"); 
static char spxScratch[SPEEX_SCRATCH_STACK_SIZE];

char *spxGlobalHeapPtr, *spxGlobalHeapEnd;
char *spxGlobalScratchPtr, *spxGlobalScratchEnd;
#endif    /* MANUAL_ALLOC */

#include <math.h>
void main()
{
   char *outFile, *bitsFile;
   FILE *fout, *fbits=NULL;
#ifndef DECODE_ONLY
   char *inFile;
   FILE *fin;
#endif
#if 0
   char *dbgoutFile;
   FILE *fdbgout;
#endif
   short out_short[FRAME_SIZE];
#ifndef DECODE_ONLY
   short in_short[FRAME_SIZE];
   float sigpow,errpow,snr, seg_snr=0;
   int snr_frames = 0;
   int nbBits;
   int i;
#endif
   char cbits[200];
   void *st;
   void *dec;
   SpeexBits bits;
   int tmp;
   int bitCount=0;
   int skip_group_delay;
   SpeexCallback callback;

#ifndef DECODE_ONLY
   sigpow = 0;
   errpow = 0;
#endif

#ifdef MANUAL_ALLOC
	spxGlobalHeapPtr = spxHeap;
	spxGlobalHeapEnd = spxHeap + sizeof(spxHeap);

	spxGlobalScratchPtr = spxScratch;
	spxGlobalScratchEnd = spxScratch + sizeof(spxScratch);
#endif
   st = speex_encoder_init(&speex_nb_mode);
#ifdef MANUAL_ALLOC
	spxGlobalScratchPtr = spxScratch;		/* Reuse scratch for decoder */
#endif
   dec = speex_decoder_init(&speex_nb_mode);

   callback.callback_id = SPEEX_INBAND_CHAR;
   callback.func = speex_std_char_handler;
   callback.data = stderr;
   speex_decoder_ctl(dec, SPEEX_SET_HANDLER, &callback);

   callback.callback_id = SPEEX_INBAND_MODE_REQUEST;
   callback.func = speex_std_mode_request_handler;
   callback.data = st;
   speex_decoder_ctl(dec, SPEEX_SET_HANDLER, &callback);

   tmp=0;
   speex_decoder_ctl(dec, SPEEX_SET_ENH, &tmp);
   tmp=0;
   speex_encoder_ctl(st, SPEEX_SET_VBR, &tmp);
   tmp=4;
   speex_encoder_ctl(st, SPEEX_SET_QUALITY, &tmp);
   tmp=1;
   speex_encoder_ctl(st, SPEEX_SET_COMPLEXITY, &tmp);

   speex_mode_query(&speex_nb_mode, SPEEX_MODE_FRAME_SIZE, &tmp);
   fprintf (stderr, "frame size: %d\n", tmp);
   skip_group_delay = tmp / 2;

#ifdef DECODE_ONLY
   bitsFile = "e:\\speextrunktest\\samples\\malebitsin54.dat";
   fbits = fopen(bitsFile, "rb");
#else
   bitsFile = "e:\\speextrunktest\\samples\\malebits.dat";
   fbits = fopen(bitsFile, "wb");
#endif
   inFile = "e:\\speextrunktest\\samples\\male.snd";
   fin = fopen(inFile, "rb");
   outFile = "e:\\speextrunktest\\samples\\maleout.snd";
   fout = fopen(outFile, "wb+");
#if 0
   dbgoutFile = "e:\\speextrunktest\\samples\\maledbgout.snd";
   fdbgout = fopen(dbgoutFile, "wb+");
#endif
 
   speex_bits_init(&bits);
#ifndef DECODE_ONLY
   while (!feof(fin))
   {
      fread(in_short, sizeof(short), FRAME_SIZE, fin);
#if 0
      fwrite(in_short, sizeof(short), FRAME_SIZE, fdbgout);
#endif
      if (feof(fin))
         break;
      speex_bits_reset(&bits);

      speex_encode_int(st, in_short, &bits);
      nbBits = speex_bits_write(&bits, cbits, 200);
      bitCount+=bits.nbBits;

      fwrite(cbits, 1, nbBits, fbits);
      speex_bits_rewind(&bits);

#else /* DECODE_ONLY */
   while (!feof(fbits))
   {
      fread(cbits, 1, 20, fbits);

      if (feof(fbits))
         break;

      speex_bits_read_from(&bits, cbits, 20);
      bitCount+=160;
#endif
      
      speex_decode_int(dec, &bits, out_short);
      speex_bits_reset(&bits);

      fwrite(&out_short[skip_group_delay], sizeof(short), FRAME_SIZE-skip_group_delay, fout);
      skip_group_delay = 0;
#if 1
   fprintf (stderr, "Bits so far: %d \n", bitCount);
#endif
   }
   fprintf (stderr, "Total encoded size: %d bits\n", bitCount);
   speex_encoder_destroy(st);
   speex_decoder_destroy(dec);

#ifndef DECODE_ONLY
   rewind(fin);
   rewind(fout);

   while ( FRAME_SIZE == fread(in_short, sizeof(short), FRAME_SIZE, fin) 
           &&
           FRAME_SIZE ==  fread(out_short, sizeof(short), FRAME_SIZE,fout) )
   {
	float s=0, e=0;
        for (i=0;i<FRAME_SIZE;++i) {
            s += (float)in_short[i] * in_short[i];
            e += ((float)in_short[i]-out_short[i]) * ((float)in_short[i]-out_short[i]);
        }
	seg_snr += 10*log10((s+160)/(e+160));
	sigpow += s;
	errpow += e;
	snr_frames++;
   }
   fclose(fin);
#endif
   fclose(fout);
   fclose(fbits);

#ifndef DECODE_ONLY
   snr = 10 * log10( sigpow / errpow );
   seg_snr /= snr_frames;
   fprintf(stderr,"SNR = %f\nsegmental SNR = %f\n",snr, seg_snr);

#ifdef FIXED_DEBUG
   printf ("Total: %f MIPS\n", (float)(1e-6*50*spx_mips/snr_frames));
#endif
#endif   
}
