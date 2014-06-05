
/*
 * PortAudio Portable Real-Time Audio Library
 * PortAudio DLL Header File
 * Latest version available at: http://www.audiomulch.com/portaudio/
 *
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

// changed by zplane.developement in order to generate a DLL

#ifndef __PADLLENTRY_HEADER_INCLUDED__

#define __PADLLENTRY_HEADER_INCLUDED__

typedef int PaError;
typedef enum {
    paNoError = 0,

    paHostError = -10000,
    paInvalidChannelCount,
    paInvalidSampleRate,
    paInvalidDeviceId,
    paInvalidFlag,
    paSampleFormatNotSupported,
    paBadIODeviceCombination,
    paInsufficientMemory,
    paBufferTooBig,
    paBufferTooSmall,
    paNullCallback,
    paBadStreamPtr,
    paTimedOut,
    paInternalError
} PaErrorNum;

typedef unsigned long PaSampleFormat;
#define paFloat32      ((PaSampleFormat) (1<<0)) /*always available*/
#define paInt16        ((PaSampleFormat) (1<<1)) /*always available*/
#define paInt32        ((PaSampleFormat) (1<<2)) /*always available*/
#define paInt24        ((PaSampleFormat) (1<<3))
#define paPackedInt24  ((PaSampleFormat) (1<<4))
#define paInt8         ((PaSampleFormat) (1<<5))
#define paUInt8        ((PaSampleFormat) (1<<6))    /* unsigned 8 bit, 128 is "ground" */
#define paCustomFormat ((PaSampleFormat) (1<<16))


typedef int PaDeviceID;
#define paNoDevice -1

typedef struct
{
    int structVersion;
    const char *name;
    int maxInputChannels;
    int maxOutputChannels;
    /* Number of discrete rates, or -1 if range supported. */
    int numSampleRates;
    /* Array of supported sample rates, or {min,max} if range supported. */
    const double *sampleRates;
    PaSampleFormat nativeSampleFormats;
}
PaDeviceInfo;


typedef double PaTimestamp;


typedef int (PortAudioCallback)(
    void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    PaTimestamp outTime, void *userData );


#define   paNoFlag      (0)
#define   paClipOff     (1<<0)   /* disable default clipping of out of range samples */
#define   paDitherOff   (1<<1)   /* disable default dithering */
#define   paPlatformSpecificFlags (0x00010000)
typedef   unsigned long PaStreamFlags;

typedef void PortAudioStream;
#define PaStream PortAudioStream

extern  PaError (__cdecl* Pa_Initialize)( void );



extern  PaError (__cdecl* Pa_Terminate)( void );


extern  long (__cdecl* Pa_GetHostError)( void );


extern  const char* (__cdecl* Pa_GetErrorText)( PaError );



extern  int (__cdecl* Pa_CountDevices)(void);

extern  PaDeviceID (__cdecl* Pa_GetDefaultInputDeviceID)( void );

extern  PaDeviceID (__cdecl* Pa_GetDefaultOutputDeviceID)( void );


extern  const PaDeviceInfo* (__cdecl* Pa_GetDeviceInfo)( PaDeviceID);



extern  PaError (__cdecl* Pa_OpenStream)(
        PortAudioStream ** ,
        PaDeviceID ,
        int ,
        PaSampleFormat ,
        void *,
        PaDeviceID ,
        int ,
        PaSampleFormat ,
        void *,
        double ,
        unsigned long ,
        unsigned long ,
        unsigned long ,
        PortAudioCallback *,
        void * );



extern  PaError (__cdecl* Pa_OpenDefaultStream)( PortAudioStream** stream,
            int numInputChannels,
            int numOutputChannels,
            PaSampleFormat sampleFormat,
            double sampleRate,
            unsigned long framesPerBuffer,
            unsigned long numberOfBuffers,
            PortAudioCallback *callback,
            void *userData );


extern  PaError (__cdecl* Pa_CloseStream)( PortAudioStream* );


extern  PaError (__cdecl* Pa_StartStream)( PortAudioStream *stream );

extern  PaError (__cdecl* Pa_StopStream)( PortAudioStream *stream );

extern  PaError (__cdecl* Pa_AbortStream)( PortAudioStream *stream );

extern  PaError (__cdecl* Pa_StreamActive)( PortAudioStream *stream );

extern  PaTimestamp (__cdecl* Pa_StreamTime)( PortAudioStream *stream );

extern  double (__cdecl* Pa_GetCPULoad)( PortAudioStream* stream );

extern  int (__cdecl* Pa_GetMinNumBuffers)( int framesPerBuffer, double sampleRate );

extern  void (__cdecl* Pa_Sleep)( long msec );

extern  PaError (__cdecl* Pa_GetSampleSize)( PaSampleFormat format );

#endif // __PADLLENTRY_HEADER_INCLUDED__

