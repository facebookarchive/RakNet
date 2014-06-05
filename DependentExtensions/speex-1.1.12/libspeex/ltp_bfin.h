/* Copyright (C) 2005 Analog Devices */
/**
   @file ltp_bfin.h
   @author Jean-Marc Valin
   @brief Long-Term Prediction functions (Blackfin version)
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

#define OVERRIDE_INNER_PROD
static spx_word32_t inner_prod(const spx_word16_t *x, const spx_word16_t *y, int len)
{
   spx_word32_t sum=0;
   __asm__ __volatile__ (
      "P0 = %3;\n\t"
      "P1 = %1;\n\t"
      "P2 = %2;\n\t"
      "I0 = P1;\n\t"
      "I1 = P2;\n\t"
      "L0 = 0;\n\t"
      "L1 = 0;\n\t"
      "A0 = 0;\n\t"
      "R0.L = W[I0++] || R1.L = W[I1++];\n\t"
      "LOOP inner%= LC0 = P0;\n\t"
      "LOOP_BEGIN inner%=;\n\t"
         "A0 += R0.L*R1.L (IS) || R0.L = W[I0++] || R1.L = W[I1++];\n\t"
      "LOOP_END inner%=;\n\t"
      "A0 += R0.L*R1.L (IS);\n\t"
      "A0 = A0 >>> 6;\n\t"
      "R0 = A0;\n\t"
      "%0 = R0;\n\t"
   : "=m" (sum)
   : "m" (x), "m" (y), "d" (len-1)
   : "P0", "P1", "P2", "R0", "R1", "A0", "I0", "I1", "L0", "L1", "R3"
   );
   return sum;
}

#define OVERRIDE_PITCH_XCORR
static void pitch_xcorr(const spx_word16_t *_x, const spx_word16_t *_y, spx_word32_t *corr, int len, int nb_pitch, char *stack)
{
   corr += nb_pitch - 1;
   __asm__ __volatile__ (
      "P2 = %0;\n\t"
      "I0 = P2;\n\t" /* x in I0 */
      "B0 = P2;\n\t" /* x in B0 */
      "R0 = %3;\n\t" /* len in R0 */
      "P3 = %3;\n\t"
      "P3 += -2;\n\t" /* len in R0 */
      "P4 = %4;\n\t" /* nb_pitch in R0 */
      "R1 = R0 << 1;\n\t" /* number of bytes in x */
      "L0 = R1;\n\t"
      "P0 = %1;\n\t"

      "P1 = %2;\n\t"
      "B1 = P1;\n\t"
      "L1 = 0;\n\t" /*Disable looping on I1*/

      "r0 = [I0++];\n\t"
      "LOOP pitch%= LC0 = P4 >> 1;\n\t"
      "LOOP_BEGIN pitch%=;\n\t"
         "I1 = P0;\n\t"
         "A1 = A0 = 0;\n\t"
         "R1 = [I1++];\n\t"
         "LOOP inner_prod%= LC1 = P3 >> 1;\n\t"
         "LOOP_BEGIN inner_prod%=;\n\t"
            "A1 += R0.L*R1.H, A0 += R0.L*R1.L (IS) || R1.L = W[I1++];\n\t"
            "A1 += R0.H*R1.L, A0 += R0.H*R1.H (IS) || R1.H = W[I1++] || R0 = [I0++];\n\t"
         "LOOP_END inner_prod%=;\n\t"
         "A1 += R0.L*R1.H, A0 += R0.L*R1.L (IS) || R1.L = W[I1++];\n\t"
         "A1 += R0.H*R1.L, A0 += R0.H*R1.H (IS) || R0 = [I0++];\n\t"
         "A0 = A0 >>> 6;\n\t"
         "A1 = A1 >>> 6;\n\t"
         "R2 = A0, R3 = A1;\n\t"
         "[P1--] = r2;\n\t"
         "[P1--] = r3;\n\t"
         "P0 += 4;\n\t"
      "LOOP_END pitch%=;\n\t"
      "L0 = 0;\n\t"
   : : "m" (_x), "m" (_y), "m" (corr), "m" (len), "m" (nb_pitch)
   : "A0", "A1", "P0", "P1", "P2", "P3", "P4", "R0", "R1", "R2", "R3", "I0", "I1", "L0", "L1", "B0", "B1", "memory"
   );
}

#define OVERRIDE_COMPUTE_PITCH_ERROR
static inline spx_word32_t compute_pitch_error(spx_word32_t *C, spx_word16_t *g, spx_word16_t pitch_control)
{
   spx_word32_t sum;
   __asm__ __volatile__
         (
         "A1 = A0 = 0;\n\t"
         
         "R0 = [%1++];\n\t"
         "R1.L = %2.L*%5.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 += R1.L*R0.L (M), A0 += R1.L*R0.H (IS) || R0 = [%1++];\n\t"
         
         "R1.L = %3.L*%5.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 += R1.L*R0.L (M), A0 += R1.L*R0.H (IS) || R0 = [%1++];\n\t"
         
         "R1.L = %4.L*%5.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 += R1.L*R0.L (M), A0 += R1.L*R0.H (IS) || R0 = [%1++];\n\t"
         
         "R1.L = %2.L*%3.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 -= R1.L*R0.L (M), A0 -= R1.L*R0.H (IS) || R0 = [%1++];\n\t"

         "R1.L = %4.L*%3.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 -= R1.L*R0.L (M), A0 -= R1.L*R0.H (IS) || R0 = [%1++];\n\t"
         
         "R1.L = %4.L*%2.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 -= R1.L*R0.L (M), A0 -= R1.L*R0.H (IS) || R0 = [%1++];\n\t"
         
         "R1.L = %2.L*%2.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 -= R1.L*R0.L (M), A0 -= R1.L*R0.H (IS) || R0 = [%1++];\n\t"

         "R1.L = %3.L*%3.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 -= R1.L*R0.L (M), A0 -= R1.L*R0.H (IS) || R0 = [%1++];\n\t"
         
         "R1.L = %4.L*%4.L (IS);\n\t"
         "R0 <<= 1;\n\t"
         "A1 -= R1.L*R0.L (M), A0 -= R1.L*R0.H (IS);\n\t"
         
         "A1 = A1 >>> 16;\n\t"
         "A0 += A1;\n\t"
         "%0 = A0;\n\t"
   : "=&D" (sum), "=a" (C)
   : "d" (g[0]), "d" (g[1]), "d" (g[2]), "d" (pitch_control), "1" (C)
   : "R0", "R1", "R2", "A0"
         );
   return sum;
}

