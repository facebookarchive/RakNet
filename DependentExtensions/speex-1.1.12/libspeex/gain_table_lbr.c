/* Copyright (C) 2002 Jean-Marc Valin 
   File: gain_table_lbr.c
   Codebook for 3-tap pitch prediction gain (32 entries)
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.  

   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

const signed char gain_cdbk_lbr[96] = {
-32,-32,-32,
-31,-58,-16,
-41,-24,-43,
-56,-22,-55,
-13,33,-41,
-4,-39,-9,
-41,15,-12,
-8,-15,-12,
1,2,-44,
-22,-66,-42,
-38,28,-23,
-21,14,-37,
0,21,-50,
-53,-71,-27,
-37,-1,-19,
-19,-5,-28,
6,65,-44,
-33,-48,-33,
-40,57,-14,
-17,4,-45,
-31,38,-33,
-23,28,-40,
-43,29,-12,
-34,13,-23,
-16,15,-27,
-14,-82,-15,
-31,25,-32,
-21,5,-5,
-47,-63,-51,
-46,12,3,
-28,-17,-29,
-10,14,-40};
