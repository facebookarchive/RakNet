/* Copyright (C) 2002 Jean-Marc Valin 
   File: ltp.c
   Long-Term Prediction functions

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

#include <math.h>
#include "ltp.h"
#include "stack_alloc.h"
#include "filters.h"
#include <speex/speex_bits.h>
#include "math_approx.h"

#ifndef NULL
#define NULL 0
#endif


#ifdef _USE_SSE
#include "ltp_sse.h"
#elif defined (ARM4_ASM) || defined(ARM5E_ASM)
#include "ltp_arm4.h"
#elif defined (BFIN_ASM)
#include "ltp_bfin.h"
#endif

#ifndef OVERRIDE_INNER_PROD
static spx_word32_t inner_prod(const spx_word16_t *x, const spx_word16_t *y, int len)
{
   spx_word32_t sum=0;
   len >>= 2;
   while(len--)
   {
      spx_word32_t part=0;
      part = MAC16_16(part,*x++,*y++);
      part = MAC16_16(part,*x++,*y++);
      part = MAC16_16(part,*x++,*y++);
      part = MAC16_16(part,*x++,*y++);
      /* HINT: If you had a 40-bit accumulator, you could shift only at the end */
      sum = ADD32(sum,SHR32(part,6));
   }
   return sum;
}
#endif

#ifndef OVERRIDE_PITCH_XCORR
#if 0 /* HINT: Enable this for machines with enough registers (i.e. not x86) */
static void pitch_xcorr(const spx_word16_t *_x, const spx_word16_t *_y, spx_word32_t *corr, int len, int nb_pitch, char *stack)
{
   int i,j;
   for (i=0;i<nb_pitch;i+=4)
   {
      /* Compute correlation*/
      /*corr[nb_pitch-1-i]=inner_prod(x, _y+i, len);*/
      spx_word32_t sum1=0;
      spx_word32_t sum2=0;
      spx_word32_t sum3=0;
      spx_word32_t sum4=0;
      const spx_word16_t *y = _y+i;
      const spx_word16_t *x = _x;
      spx_word16_t y0, y1, y2, y3;
      /*y0=y[0];y1=y[1];y2=y[2];y3=y[3];*/
      y0=*y++;
      y1=*y++;
      y2=*y++;
      y3=*y++;
      for (j=0;j<len;j+=4)
      {
         spx_word32_t part1;
         spx_word32_t part2;
         spx_word32_t part3;
         spx_word32_t part4;
         part1 = MULT16_16(*x,y0);
         part2 = MULT16_16(*x,y1);
         part3 = MULT16_16(*x,y2);
         part4 = MULT16_16(*x,y3);
         x++;
         y0=*y++;
         part1 = MAC16_16(part1,*x,y1);
         part2 = MAC16_16(part2,*x,y2);
         part3 = MAC16_16(part3,*x,y3);
         part4 = MAC16_16(part4,*x,y0);
         x++;
         y1=*y++;
         part1 = MAC16_16(part1,*x,y2);
         part2 = MAC16_16(part2,*x,y3);
         part3 = MAC16_16(part3,*x,y0);
         part4 = MAC16_16(part4,*x,y1);
         x++;
         y2=*y++;
         part1 = MAC16_16(part1,*x,y3);
         part2 = MAC16_16(part2,*x,y0);
         part3 = MAC16_16(part3,*x,y1);
         part4 = MAC16_16(part4,*x,y2);
         x++;
         y3=*y++;
         
         sum1 = ADD32(sum1,SHR32(part1,6));
         sum2 = ADD32(sum2,SHR32(part2,6));
         sum3 = ADD32(sum3,SHR32(part3,6));
         sum4 = ADD32(sum4,SHR32(part4,6));
      }
      corr[nb_pitch-1-i]=sum1;
      corr[nb_pitch-2-i]=sum2;
      corr[nb_pitch-3-i]=sum3;
      corr[nb_pitch-4-i]=sum4;
   }

}
#else
static void pitch_xcorr(const spx_word16_t *_x, const spx_word16_t *_y, spx_word32_t *corr, int len, int nb_pitch, char *stack)
{
   int i;
   for (i=0;i<nb_pitch;i++)
   {
      /* Compute correlation*/
      corr[nb_pitch-1-i]=inner_prod(_x, _y+i, len);
   }

}
#endif
#endif

#ifndef OVERRIDE_COMPUTE_PITCH_ERROR
static inline spx_word32_t compute_pitch_error(spx_word32_t *C, spx_word16_t *g, spx_word16_t pitch_control)
{
   spx_word32_t sum = 0;
   sum = ADD32(sum,MULT16_32_Q15(MULT16_16_16(g[0],pitch_control),C[0]));
   sum = ADD32(sum,MULT16_32_Q15(MULT16_16_16(g[1],pitch_control),C[1]));
   sum = ADD32(sum,MULT16_32_Q15(MULT16_16_16(g[2],pitch_control),C[2]));
   sum = SUB32(sum,MULT16_32_Q15(MULT16_16_16(g[0],g[1]),C[3]));
   sum = SUB32(sum,MULT16_32_Q15(MULT16_16_16(g[2],g[1]),C[4]));
   sum = SUB32(sum,MULT16_32_Q15(MULT16_16_16(g[2],g[0]),C[5]));
   sum = SUB32(sum,MULT16_32_Q15(MULT16_16_16(g[0],g[0]),C[6]));
   sum = SUB32(sum,MULT16_32_Q15(MULT16_16_16(g[1],g[1]),C[7]));
   sum = SUB32(sum,MULT16_32_Q15(MULT16_16_16(g[2],g[2]),C[8]));
   return sum;
}
#endif

void open_loop_nbest_pitch(spx_sig_t *sw, int start, int end, int len, int *pitch, spx_word16_t *gain, int N, char *stack)
{
   int i,j,k;
   VARDECL(spx_word32_t *best_score);
   spx_word32_t e0;
   VARDECL(spx_word32_t *corr);
   VARDECL(spx_word32_t *energy);
   VARDECL(spx_word32_t *score);
   VARDECL(spx_word16_t *swn2);
   spx_word16_t *swn;

   ALLOC(best_score, N, spx_word32_t);
   ALLOC(corr, end-start+1, spx_word32_t);
   ALLOC(energy, end-start+2, spx_word32_t);
   ALLOC(score, end-start+1, spx_word32_t);

#ifdef FIXED_POINT
   ALLOC(swn2, end+len, spx_word16_t);
   normalize16(sw-end, swn2, 16384, end+len);
   swn = swn2 + end;
#else
   swn = sw;
#endif

   for (i=0;i<N;i++)
   {
        best_score[i]=-1;
        pitch[i]=start;
   }


   energy[0]=inner_prod(swn-start, swn-start, len);
   e0=inner_prod(swn, swn, len);
   for (i=start;i<=end;i++)
   {
      /* Update energy for next pitch*/
      energy[i-start+1] = SUB32(ADD32(energy[i-start],SHR32(MULT16_16(swn[-i-1],swn[-i-1]),6)), SHR32(MULT16_16(swn[-i+len-1],swn[-i+len-1]),6));
      if (energy[i-start+1] < 0)
         energy[i-start+1] = 0;
   }

   pitch_xcorr(swn, swn-end, corr, len, end-start+1, stack);

#ifdef FIXED_POINT
   {
      VARDECL(spx_word16_t *corr16);
      VARDECL(spx_word16_t *ener16);
      ALLOC(corr16, end-start+1, spx_word16_t);
      ALLOC(ener16, end-start+1, spx_word16_t);
      normalize16(corr, corr16, 16384, end-start+1);
      normalize16(energy, ener16, 16384, end-start+1);

      for (i=start;i<=end;i++)
      {
         spx_word16_t g;
         spx_word32_t tmp;
         tmp = corr16[i-start];
         if (tmp>0)
         {
            if (SHR16(corr16[i-start],4)>ener16[i-start])
               tmp = SHL32(EXTEND32(ener16[i-start]),14);
            else if (-SHR16(corr16[i-start],4)>ener16[i-start])
               tmp = -SHL32(EXTEND32(ener16[i-start]),14);
            else
               tmp = SHL32(tmp,10);
            g = DIV32_16(tmp, 8+ener16[i-start]);
            score[i-start] = MULT16_16(corr16[i-start],g);
         } else
         {
            score[i-start] = 1;
         }
      }
   }
#else
   for (i=start;i<=end;i++)
   {
      float g = corr[i-start]/(1+energy[i-start]);
      if (g>16)
         g = 16;
      else if (g<-16)
         g = -16;
      score[i-start] = g*corr[i-start];
   }
#endif

   /* Extract best scores */
   for (i=start;i<=end;i++)
   {
      if (score[i-start]>best_score[N-1])
      {
         for (j=0;j<N;j++)
         {
            if (score[i-start] > best_score[j])
            {
               for (k=N-1;k>j;k--)
               {
                  best_score[k]=best_score[k-1];
                  pitch[k]=pitch[k-1];
               }
               best_score[j]=score[i-start];
               pitch[j]=i;
               break;
            }
         }
      }
   }

   /* Compute open-loop gain */
   if (gain)
   {
       for (j=0;j<N;j++)
       {
          spx_word16_t g;
          i=pitch[j];
          g = DIV32(corr[i-start], 10+SHR32(MULT16_16(spx_sqrt(e0),spx_sqrt(energy[i-start])),6));
          /* FIXME: g = max(g,corr/energy) */
                   if (g<0)
                   g = 0;
             gain[j]=g;
       }
   }
}


/** Finds the best quantized 3-tap pitch predictor by analysis by synthesis */
static spx_word64_t pitch_gain_search_3tap(
const spx_sig_t target[],       /* Target vector */
const spx_coef_t ak[],          /* LPCs for this subframe */
const spx_coef_t awk1[],        /* Weighted LPCs #1 for this subframe */
const spx_coef_t awk2[],        /* Weighted LPCs #2 for this subframe */
spx_sig_t exc[],                /* Excitation */
const void *par,
int   pitch,                    /* Pitch value */
int   p,                        /* Number of LPC coeffs */
int   nsf,                      /* Number of samples in subframe */
SpeexBits *bits,
char *stack,
const spx_sig_t *exc2,
const spx_word16_t *r,
spx_sig_t *new_target,
int  *cdbk_index,
int cdbk_offset,
int plc_tuning
)
{
   int i,j;
   VARDECL(spx_sig_t *tmp1);
   VARDECL(spx_sig_t *tmp2);
   spx_sig_t *x[3];
   spx_sig_t *e[3];
   spx_word32_t corr[3];
   spx_word32_t A[3][3];
   int   gain_cdbk_size;
   const signed char *gain_cdbk;
   spx_word16_t gain[3];
   spx_word64_t err;

   const ltp_params *params;
   params = (const ltp_params*) par;
   gain_cdbk_size = 1<<params->gain_bits;
   gain_cdbk = params->gain_cdbk + 3*gain_cdbk_size*cdbk_offset;
   ALLOC(tmp1, 3*nsf, spx_sig_t);
   ALLOC(tmp2, 3*nsf, spx_sig_t);

   x[0]=tmp1;
   x[1]=tmp1+nsf;
   x[2]=tmp1+2*nsf;
   
   e[0]=tmp2;
   e[1]=tmp2+nsf;
   e[2]=tmp2+2*nsf;
   for (i=2;i>=0;i--)
   {
      int pp=pitch+1-i;
      for (j=0;j<nsf;j++)
      {
         if (j-pp<0)
            e[i][j]=exc2[j-pp];
         else if (j-pp-pitch<0)
            e[i][j]=exc2[j-pp-pitch];
         else
            e[i][j]=0;
      }

      if (i==2)
         syn_percep_zero(e[i], ak, awk1, awk2, x[i], nsf, p, stack);
      else {
         for (j=0;j<nsf-1;j++)
            x[i][j+1]=x[i+1][j];
         x[i][0]=0;
         for (j=0;j<nsf;j++)
         {
            x[i][j]=ADD32(x[i][j],SHL32(MULT16_32_Q15(r[j], e[i][0]),1));
         }
      }
   }

#ifdef FIXED_POINT
   {
      /* If using fixed-point, we need to normalize the signals first */
      spx_word16_t *y[3];
      VARDECL(spx_word16_t *ytmp);
      VARDECL(spx_word16_t *t);

      spx_sig_t max_val=1;
      int sig_shift;
      
      ALLOC(ytmp, 3*nsf, spx_word16_t);
#if 0
      ALLOC(y[0], nsf, spx_word16_t);
      ALLOC(y[1], nsf, spx_word16_t);
      ALLOC(y[2], nsf, spx_word16_t);
#else
      y[0] = ytmp;
      y[1] = ytmp+nsf;
      y[2] = ytmp+2*nsf;
#endif
      ALLOC(t, nsf, spx_word16_t);
      for (j=0;j<3;j++)
      {
         for (i=0;i<nsf;i++)
         {
            spx_sig_t tmp = x[j][i];
            if (tmp<0)
               tmp = -tmp;
            if (tmp > max_val)
               max_val = tmp;
         }
      }
      for (i=0;i<nsf;i++)
      {
         spx_sig_t tmp = target[i];
         if (tmp<0)
            tmp = -tmp;
         if (tmp > max_val)
            max_val = tmp;
      }

      sig_shift=0;
      while (max_val>16384)
      {
         sig_shift++;
         max_val >>= 1;
      }

      for (j=0;j<3;j++)
      {
         for (i=0;i<nsf;i++)
         {
            y[j][i] = EXTRACT16(SHR32(x[j][i],sig_shift));
         }
      }
      for (i=0;i<nsf;i++)
      {
         t[i] = EXTRACT16(SHR32(target[i],sig_shift));
      }

      for (i=0;i<3;i++)
         corr[i]=inner_prod(y[i],t,nsf);
      
      for (i=0;i<3;i++)
         for (j=0;j<=i;j++)
            A[i][j]=A[j][i]=inner_prod(y[i],y[j],nsf);
   }
#else
   {
      for (i=0;i<3;i++)
         corr[i]=inner_prod(x[i],target,nsf);
      
      for (i=0;i<3;i++)
         for (j=0;j<=i;j++)
            A[i][j]=A[j][i]=inner_prod(x[i],x[j],nsf);
   }
#endif

   {
      spx_word32_t C[9];
      const signed char *ptr=gain_cdbk;
      int best_cdbk=0;
      spx_word32_t best_sum=0;
      C[0]=corr[2];
      C[1]=corr[1];
      C[2]=corr[0];
      C[3]=A[1][2];
      C[4]=A[0][1];
      C[5]=A[0][2];      
      C[6]=A[2][2];
      C[7]=A[1][1];
      C[8]=A[0][0];
      
      /*plc_tuning *= 2;*/
      if (plc_tuning<2)
         plc_tuning=2;
#ifdef FIXED_POINT
      C[0] = MAC16_32_Q15(C[0],MULT16_16_16(plc_tuning,-327),C[0]);
      C[1] = MAC16_32_Q15(C[1],MULT16_16_16(plc_tuning,-327),C[1]);
      C[2] = MAC16_32_Q15(C[2],MULT16_16_16(plc_tuning,-327),C[2]);
      C[0] = SHL32(C[0],1);
      C[1] = SHL32(C[1],1);
      C[2] = SHL32(C[2],1);
      C[3] = SHL32(C[3],1);
      C[4] = SHL32(C[4],1);
      C[5] = SHL32(C[5],1);
#else
      C[0]*=1-.01*plc_tuning;
      C[1]*=1-.01*plc_tuning;
      C[2]*=1-.01*plc_tuning;
      C[6]*=.5*(1+.01*plc_tuning);
      C[7]*=.5*(1+.01*plc_tuning);
      C[8]*=.5*(1+.01*plc_tuning);
#endif
      for (i=0;i<gain_cdbk_size;i++)
      {
         spx_word32_t sum=0;
         spx_word16_t g[3];
         spx_word16_t pitch_control=64;
         spx_word16_t gain_sum;
         
         ptr = gain_cdbk+3*i;
         g[0]=ADD16((spx_word16_t)ptr[0],32);
         g[1]=ADD16((spx_word16_t)ptr[1],32);
         g[2]=ADD16((spx_word16_t)ptr[2],32);

         /* We favor "safe" pitch values to handle packet loss better */
         gain_sum = ADD16(ADD16(g[1],MAX16(g[0], 0)),MAX16(g[2], 0));
         if (gain_sum > 64)
         {
            gain_sum = SUB16(gain_sum, 64);
            if (gain_sum > 127)
               gain_sum = 127;
#ifdef FIXED_POINT
            pitch_control =  SUB16(64,EXTRACT16(PSHR32(MULT16_16(64,MULT16_16_16(plc_tuning, gain_sum)),10)));
#else
            pitch_control = 64*(1.-.001*plc_tuning*gain_sum);
#endif
            if (pitch_control < 0)
               pitch_control = 0;
         }
         
         sum = compute_pitch_error(C, g, pitch_control);
         
         if (sum>best_sum || i==0)
         {
            best_sum=sum;
            best_cdbk=i;
         }
      }
#ifdef FIXED_POINT
      gain[0] = ADD16(32,(spx_word16_t)gain_cdbk[best_cdbk*3]);
      gain[1] = ADD16(32,(spx_word16_t)gain_cdbk[best_cdbk*3+1]);
      gain[2] = ADD16(32,(spx_word16_t)gain_cdbk[best_cdbk*3+2]);
      /*printf ("%d %d %d %d\n",gain[0],gain[1],gain[2], best_cdbk);*/
#else
      gain[0] = 0.015625*gain_cdbk[best_cdbk*3]  + .5;
      gain[1] = 0.015625*gain_cdbk[best_cdbk*3+1]+ .5;
      gain[2] = 0.015625*gain_cdbk[best_cdbk*3+2]+ .5;
#endif
      *cdbk_index=best_cdbk;
   }

#ifdef FIXED_POINT
   for (i=0;i<nsf;i++)
     exc[i]=SHL32(ADD32(ADD32(MULT16_32_Q15(SHL16(gain[0],7),e[2][i]), MULT16_32_Q15(SHL16(gain[1],7),e[1][i])),
                        MULT16_32_Q15(SHL16(gain[2],7),e[0][i])), 2);
   
   err=0;
   for (i=0;i<nsf;i++)
   {
      spx_word16_t perr2;
      spx_sig_t tmp = SHL32(ADD32(ADD32(MULT16_32_Q15(SHL16(gain[0],7),x[2][i]),MULT16_32_Q15(SHL16(gain[1],7),x[1][i])),
                                  MULT16_32_Q15(SHL16(gain[2],7),x[0][i])),2);
      spx_sig_t perr=SUB32(target[i],tmp);
      new_target[i] = SUB32(target[i], tmp);
      perr2 = EXTRACT16(PSHR32(perr,15));
      err = ADD64(err,MULT16_16(perr2,perr2));
      
   }
#else
   for (i=0;i<nsf;i++)
      exc[i]=gain[0]*e[2][i]+gain[1]*e[1][i]+gain[2]*e[0][i];
   
   err=0;
   for (i=0;i<nsf;i++)
   {
      spx_sig_t tmp = gain[2]*x[0][i]+gain[1]*x[1][i]+gain[0]*x[2][i];
      new_target[i] = target[i] - tmp;
      err+=new_target[i]*new_target[i];
   }
#endif

   return err;
}


/** Finds the best quantized 3-tap pitch predictor by analysis by synthesis */
int pitch_search_3tap(
spx_sig_t target[],                 /* Target vector */
spx_sig_t *sw,
spx_coef_t ak[],                     /* LPCs for this subframe */
spx_coef_t awk1[],                   /* Weighted LPCs #1 for this subframe */
spx_coef_t awk2[],                   /* Weighted LPCs #2 for this subframe */
spx_sig_t exc[],                    /* Excitation */
const void *par,
int   start,                    /* Smallest pitch value allowed */
int   end,                      /* Largest pitch value allowed */
spx_word16_t pitch_coef,               /* Voicing (pitch) coefficient */
int   p,                        /* Number of LPC coeffs */
int   nsf,                      /* Number of samples in subframe */
SpeexBits *bits,
char *stack,
spx_sig_t *exc2,
spx_word16_t *r,
int complexity,
int cdbk_offset,
int plc_tuning
)
{
   int i,j;
   int cdbk_index, pitch=0, best_gain_index=0;
   VARDECL(spx_sig_t *best_exc);
   VARDECL(spx_sig_t *new_target);
   VARDECL(spx_sig_t *best_target);
   int best_pitch=0;
   spx_word64_t err, best_err=-1;
   int N;
   const ltp_params *params;
   VARDECL(int *nbest);

   N=complexity;
   if (N>10)
      N=10;
   if (N<1)
      N=1;

   ALLOC(nbest, N, int);
   params = (const ltp_params*) par;

   if (end<start)
   {
      speex_bits_pack(bits, 0, params->pitch_bits);
      speex_bits_pack(bits, 0, params->gain_bits);
      for (i=0;i<nsf;i++)
         exc[i]=0;
      return start;
   }
   
   ALLOC(best_exc, nsf, spx_sig_t);
   ALLOC(new_target, nsf, spx_sig_t);
   ALLOC(best_target, nsf, spx_sig_t);
   
   if (N>end-start+1)
      N=end-start+1;
   if (end != start)
      open_loop_nbest_pitch(sw, start, end, nsf, nbest, NULL, N, stack);
   else
      nbest[0] = start;
   for (i=0;i<N;i++)
   {
      pitch=nbest[i];
      for (j=0;j<nsf;j++)
         exc[j]=0;
      err=pitch_gain_search_3tap(target, ak, awk1, awk2, exc, par, pitch, p, nsf,
                                 bits, stack, exc2, r, new_target, &cdbk_index, cdbk_offset, plc_tuning);
      if (err<best_err || best_err<0)
      {
         for (j=0;j<nsf;j++)
            best_exc[j]=exc[j];
         for (j=0;j<nsf;j++)
            best_target[j]=new_target[j];
         best_err=err;
         best_pitch=pitch;
         best_gain_index=cdbk_index;
      }
   }
   
   /*printf ("pitch: %d %d\n", best_pitch, best_gain_index);*/
   speex_bits_pack(bits, best_pitch-start, params->pitch_bits);
   speex_bits_pack(bits, best_gain_index, params->gain_bits);
   /*printf ("encode pitch: %d %d\n", best_pitch, best_gain_index);*/
   for (i=0;i<nsf;i++)
      exc[i]=best_exc[i];
   for (i=0;i<nsf;i++)
      target[i]=best_target[i];

   return pitch;
}

void pitch_unquant_3tap(
spx_sig_t exc[],                    /* Excitation */
int   start,                    /* Smallest pitch value allowed */
int   end,                      /* Largest pitch value allowed */
spx_word16_t pitch_coef,               /* Voicing (pitch) coefficient */
const void *par,
int   nsf,                      /* Number of samples in subframe */
int *pitch_val,
spx_word16_t *gain_val,
SpeexBits *bits,
char *stack,
int count_lost,
int subframe_offset,
spx_word16_t last_pitch_gain,
int cdbk_offset
)
{
   int i;
   int pitch;
   int gain_index;
   spx_word16_t gain[3];
   const signed char *gain_cdbk;
   int gain_cdbk_size;
   const ltp_params *params;

   params = (const ltp_params*) par;
   gain_cdbk_size = 1<<params->gain_bits;
   gain_cdbk = params->gain_cdbk + 3*gain_cdbk_size*cdbk_offset;

   pitch = speex_bits_unpack_unsigned(bits, params->pitch_bits);
   pitch += start;
   gain_index = speex_bits_unpack_unsigned(bits, params->gain_bits);
   /*printf ("decode pitch: %d %d\n", pitch, gain_index);*/
#ifdef FIXED_POINT
   gain[0] = ADD16(32,(spx_word16_t)gain_cdbk[gain_index*3]);
   gain[1] = ADD16(32,(spx_word16_t)gain_cdbk[gain_index*3+1]);
   gain[2] = ADD16(32,(spx_word16_t)gain_cdbk[gain_index*3+2]);
#else
   gain[0] = 0.015625*gain_cdbk[gain_index*3]+.5;
   gain[1] = 0.015625*gain_cdbk[gain_index*3+1]+.5;
   gain[2] = 0.015625*gain_cdbk[gain_index*3+2]+.5;
#endif

   if (count_lost && pitch > subframe_offset)
   {
      spx_word16_t gain_sum;
      if (1) {
#ifdef FIXED_POINT
         spx_word16_t tmp = count_lost < 4 ? last_pitch_gain : SHR16(last_pitch_gain,1);
         if (tmp>62)
            tmp=62;
#else
         spx_word16_t tmp = count_lost < 4 ? last_pitch_gain : 0.5 * last_pitch_gain;
         if (tmp>.95)
            tmp=.95;
#endif
         gain_sum = gain_3tap_to_1tap(gain);

         if (gain_sum > tmp)
         {
            spx_word16_t fact = DIV32_16(SHL32(EXTEND32(tmp),14),gain_sum);
            for (i=0;i<3;i++)
               gain[i]=MULT16_16_Q14(fact,gain[i]);
         }

      }

   }

   *pitch_val = pitch;
   gain_val[0]=gain[0];
   gain_val[1]=gain[1];
   gain_val[2]=gain[2];

   {
      spx_sig_t *e[3];
      VARDECL(spx_sig_t *tmp2);
      ALLOC(tmp2, 3*nsf, spx_sig_t);
      e[0]=tmp2;
      e[1]=tmp2+nsf;
      e[2]=tmp2+2*nsf;
      
      for (i=0;i<3;i++)
      {
         int j;
         int pp=pitch+1-i;
#if 0
         for (j=0;j<nsf;j++)
         {
            if (j-pp<0)
               e[i][j]=exc[j-pp];
            else if (j-pp-pitch<0)
               e[i][j]=exc[j-pp-pitch];
            else
               e[i][j]=0;
         }
#else
         {
            int tmp1, tmp3;
            tmp1=nsf;
            if (tmp1>pp)
               tmp1=pp;
            for (j=0;j<tmp1;j++)
               e[i][j]=exc[j-pp];
            tmp3=nsf;
            if (tmp3>pp+pitch)
               tmp3=pp+pitch;
            for (j=tmp1;j<tmp3;j++)
               e[i][j]=exc[j-pp-pitch];
            for (j=tmp3;j<nsf;j++)
               e[i][j]=0;
         }
#endif
      }

#ifdef FIXED_POINT
      {
         for (i=0;i<nsf;i++)
            exc[i]=SHL32(ADD32(ADD32(MULT16_32_Q15(SHL16(gain[0],7),e[2][i]), MULT16_32_Q15(SHL16(gain[1],7),e[1][i])),
                               MULT16_32_Q15(SHL16(gain[2],7),e[0][i])), 2);
      }
#else
      for (i=0;i<nsf;i++)
         exc[i]=VERY_SMALL+gain[0]*e[2][i]+gain[1]*e[1][i]+gain[2]*e[0][i];
#endif
   }
}


/** Forced pitch delay and gain */
int forced_pitch_quant(
spx_sig_t target[],                 /* Target vector */
spx_sig_t *sw,
spx_coef_t ak[],                     /* LPCs for this subframe */
spx_coef_t awk1[],                   /* Weighted LPCs #1 for this subframe */
spx_coef_t awk2[],                   /* Weighted LPCs #2 for this subframe */
spx_sig_t exc[],                    /* Excitation */
const void *par,
int   start,                    /* Smallest pitch value allowed */
int   end,                      /* Largest pitch value allowed */
spx_word16_t pitch_coef,               /* Voicing (pitch) coefficient */
int   p,                        /* Number of LPC coeffs */
int   nsf,                      /* Number of samples in subframe */
SpeexBits *bits,
char *stack,
spx_sig_t *exc2,
spx_word16_t *r,
int complexity,
int cdbk_offset,
int plc_tuning
)
{
   int i;
   float coef = GAIN_SCALING_1*pitch_coef;
   if (coef>.99)
      coef=.99;
   for (i=0;i<nsf;i++)
   {
      exc[i]=exc[i-start]*coef;
   }
   return start;
}

/** Unquantize forced pitch delay and gain */
void forced_pitch_unquant(
spx_sig_t exc[],                    /* Excitation */
int   start,                    /* Smallest pitch value allowed */
int   end,                      /* Largest pitch value allowed */
spx_word16_t pitch_coef,               /* Voicing (pitch) coefficient */
const void *par,
int   nsf,                      /* Number of samples in subframe */
int *pitch_val,
spx_word16_t *gain_val,
SpeexBits *bits,
char *stack,
int count_lost,
int subframe_offset,
spx_word16_t last_pitch_gain,
int cdbk_offset
)
{
   int i;
   float coef = GAIN_SCALING_1*pitch_coef;
   if (coef>.99)
      coef=.99;
   for (i=0;i<nsf;i++)
   {
      exc[i]=exc[i-start]*coef;
   }
   *pitch_val = start;
   gain_val[0]=gain_val[2]=0;
   gain_val[1] = pitch_coef;
}
