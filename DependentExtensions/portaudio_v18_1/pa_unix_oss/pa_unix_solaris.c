/*
 * PortAudio Portable Real-Time Audio Library
 * Latest Version at: http://www.portaudio.com
 * Linux OSS Implementation by douglas repetto and Phil Burk
 *
 * Copyright (c) 1999-2000 Phil Burk
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

/* Modification history:
   20020621: Initial cut at Solaris modifications jointly by Sam Bayer
             and Augustus Saunders.
   20030206 - Martin Rohrbach - various mods for Solaris
 */

#define __solaris_native__

#include "pa_unix.h"

/* SAM 6/2/02: Docs say we should include sys/audio.h, but
   that doesn't exist pre Solaris 2.8. These headers work fine. */

#include <sys/audioio.h>
#include <sys/stropts.h>

/*********************************************************************
 * Try to open the named device.
 * If it opens, try to set various rates and formats and fill in
 * the device info structure.
 */
PaError Pa_QueryDevice( const char *deviceName, internalPortAudioDevice *pad )
{
    int result = paHostError;
    int tempDevHandle;
    int numChannels, maxNumChannels;
    int numSampleRates;
    int sampleRate;
    int numRatesToTry;
    int ratesToTry[9] = {96000, 48000, 44100, 32000, 24000, 22050, 16000, 11025, 8000};
    int i;
    audio_info_t solaris_info;
    audio_device_t device_info;

    /* douglas:
     we have to do this querying in a slightly different order. apparently
     some sound cards will give you different info based on their settins.
     e.g. a card might give you stereo at 22kHz but only mono at 44kHz.
     the correct order for OSS is: format, channels, sample rate

    */
	/*
	 to check a device for it's capabilities, it's probably better to use the
	 equivalent "-ctl"-descriptor - MR
	*/
    char devname[strlen(deviceName) + 4];
    if ( (tempDevHandle = open(strcat(strcpy(devname, deviceName), "ctl"), O_WRONLY|O_NONBLOCK))  == -1 )
    {
        DBUG(("Pa_QueryDevice: could not open %s\n", deviceName ));
        return paHostError;
    }

    /*  Ask OSS what formats are supported by the hardware. */
    pad->pad_Info.nativeSampleFormats = 0;
    AUDIO_INITINFO(&solaris_info);

    /* SAM 12/31/01: Sparc native does mulaw, alaw and PCM.
       I think PCM is signed. */

    for (i = 8; i <= 32; i += 8) {
      solaris_info.play.precision = i;
      solaris_info.play.encoding = AUDIO_ENCODING_LINEAR;
      /* If there are no errors, add the format. */
      if (ioctl(tempDevHandle, AUDIO_SETINFO, &solaris_info) > -1) {
	switch (i) {
	case 8:
	  pad->pad_Info.nativeSampleFormats |= paInt8;
	  break;
	case 16:
	  pad->pad_Info.nativeSampleFormats |= paInt16;
	  break;
	case 24:
	  pad->pad_Info.nativeSampleFormats |= paInt24;
	  break;
	case 32:
	  pad->pad_Info.nativeSampleFormats |= paInt32;
	  break;
	}
      }
    }

    maxNumChannels = 0;
    for( numChannels = 1; numChannels <= 16; numChannels++ )
      {
	int temp = numChannels;
	DBUG(("Pa_QueryDevice: use SNDCTL_DSP_CHANNELS, numChannels = %d\n", numChannels ))
	  AUDIO_INITINFO(&solaris_info);
	solaris_info.play.channels = temp;
	if (ioctl(tempDevHandle, AUDIO_SETINFO, &solaris_info) < 0)
	  {
	    /* ioctl() failed so bail out if we already have stereo */
	    if( numChannels > 2 ) break;
	  }
	else
	  {
	    /* ioctl() worked but bail out if it does not support numChannels.
	     * We don't want to leave gaps in the numChannels supported.
	     */
	    if( (numChannels > 2) && (temp != numChannels) ) break;
	    DBUG(("Pa_QueryDevice: temp = %d\n", temp ))
	      if( temp > maxNumChannels ) maxNumChannels = temp; /* Save maximum. */
	  }
      }

    pad->pad_Info.maxOutputChannels = maxNumChannels;
    DBUG(("Pa_QueryDevice: maxNumChannels = %d\n", maxNumChannels))

    /* FIXME - for now, assume maxInputChannels = maxOutputChannels.
     *    Eventually do separate queries for O_WRONLY and O_RDONLY
    */
    pad->pad_Info.maxInputChannels = pad->pad_Info.maxOutputChannels;

    DBUG(("Pa_QueryDevice: maxInputChannels = %d\n",
          pad->pad_Info.maxInputChannels))


    /* Determine available sample rates by trying each one and seeing result.
     */
    numSampleRates = 0;

    AUDIO_INITINFO(&solaris_info);

    numRatesToTry = sizeof(ratesToTry)/sizeof(int);
    for (i = 0; i < numRatesToTry; i++)
    {
        sampleRate = ratesToTry[i];

	solaris_info.play.sample_rate = sampleRate; /* AS: We opened for Write, so set play */
        if (ioctl(tempDevHandle, AUDIO_SETINFO, &solaris_info) >= 0 ) /* PLB20010817 */
        {
            if (sampleRate == ratesToTry[i])
            {
                DBUG(("Pa_QueryDevice: got sample rate: %d\n", sampleRate))
                pad->pad_SampleRates[numSampleRates] = (float)ratesToTry[i];
                numSampleRates++;
            }
        }
    }

    DBUG(("Pa_QueryDevice: final numSampleRates = %d\n", numSampleRates))
    if (numSampleRates==0)   /* HP20010922 */
    {
        ERR_RPT(("Pa_QueryDevice: no supported sample rate (or SNDCTL_DSP_SPEED ioctl call failed).\n" ));
        goto error;
    }

    pad->pad_Info.numSampleRates = numSampleRates;
    pad->pad_Info.sampleRates = pad->pad_SampleRates;

    /* query for the device name instead of using the filesystem-device - MR */
	if (ioctl(tempDevHandle, AUDIO_GETDEV, &device_info) == -1) {
      pad->pad_Info.name = deviceName;
    } else {
      char *pt = (char *)PaHost_AllocateFastMemory(strlen(device_info.name));
      strcpy(pt, device_info.name);
      pad->pad_Info.name = pt;
    }

    result = paNoError;

error:
    /* We MUST close the handle here or we won't be able to reopen it later!!!  */
    close(tempDevHandle);

    return result;
}

/*******************************************************************************************/

PaError Pa_SetupInputDeviceFormat( int devHandle, int numChannels, int sampleRate )
{
    audio_info_t solaris_info;
    AUDIO_INITINFO(&solaris_info);

    /* Sam Bayer/Bryan George 1/10/02: Various folks have
       reported that on Solaris Ultra II, the not-right thing
       happens on read unless you make sure the audio device is
       flushed. The folks who wrote the Robust Audio Tool say:
       + XXX driver issue - on Ultra II's if you don't drain
       * the device before reading commences then the device
       * reads in blocks of 500ms irrespective of the
       * blocksize set. After a minute or so it flips into the
       * correct mode, but obviously this is too late to be + * useful for most apps. grrr.
       */
    /* AS: And the Solaris man audio pages say you should flush before changing formats
       anyway.  So there you go. */
    if (Pa_FlushStream(devHandle) != paNoError)
      return paHostError;

    solaris_info.record.encoding = AUDIO_ENCODING_LINEAR;
    solaris_info.record.sample_rate = sampleRate;
    solaris_info.record.precision = 16;
    solaris_info.record.channels = numChannels;

    if (ioctl(devHandle, AUDIO_SETINFO, &solaris_info) == -1)
      {
        ERR_RPT(("Pa_SetupDeviceFormat: could not set audio info\n" ));
        return paHostError;
      }

    return paNoError;
}

PaError Pa_SetupOutputDeviceFormat( int devHandle, int numChannels, int sampleRate )
{
    audio_info_t solaris_info;
    AUDIO_INITINFO(&solaris_info);

    /* Sam Bayer/Bryan George 1/10/02: Various folks have
       reported that on Solaris Ultra II, the not-right thing
       happens on read unless you make sure the audio device is
       flushed. The folks who wrote the Robust Audio Tool say:
       + XXX driver issue - on Ultra II's if you don't drain
       * the device before reading commences then the device
       * reads in blocks of 500ms irrespective of the
       * blocksize set. After a minute or so it flips into the
       * correct mode, but obviously this is too late to be + * useful for most apps. grrr.
       */
    /* AS: And the Solaris man audio pages say you should flush before changing formats
       anyway.  So there you go. */
    if (Pa_FlushStream(devHandle) != paNoError)
      return paHostError;

    solaris_info.play.encoding = AUDIO_ENCODING_LINEAR;
    solaris_info.play.sample_rate = sampleRate;
    solaris_info.play.precision = 16;
    solaris_info.play.channels = numChannels;

    if (ioctl(devHandle, AUDIO_SETINFO, &solaris_info) == -1)
      {
        ERR_RPT(("Pa_SetupDeviceFormat: could not set audio info\n" ));
        return paHostError;
      }

    return paNoError;
}

PaError Pa_SetupDeviceFormat( int devHandle, int numChannels, int sampleRate )
{
        PaError result = paNoError;

	result = Pa_SetupOutputDeviceFormat(devHandle, numChannels, sampleRate);
	if (result != paNoError)
	  return result;
	return Pa_SetupInputDeviceFormat(devHandle, numChannels, sampleRate);
}

/*******************************************************************************************
** Set number of fragments and size of fragments to achieve desired latency.
*/

static PaError Pa_Unpause(int devHandle);
static PaError Pa_PauseAndFlush(int devHandle);

void Pa_SetLatency( int devHandle, int numBuffers, int framesPerBuffer, int channelsPerFrame  )
{
  int     bufferSize;
  audio_info_t solaris_info;

  /* Increase size of buffers and reduce number of buffers to reduce latency inside driver. */
  while( numBuffers > 8 )
    {
      numBuffers = (numBuffers + 1) >> 1;
      framesPerBuffer = framesPerBuffer << 1;
    }

  /* calculate size of buffers in bytes */
  bufferSize = framesPerBuffer * channelsPerFrame * sizeof(short); /* FIXME - other sizes? */

  DBUG(("Pa_SetLatency: numBuffers = %d, framesPerBuffer = %d\n",
	numBuffers, framesPerBuffer));

  /* SAM 6/6/02: Documentation says to pause and flush before
     changing buffer size. */

  if (Pa_PauseAndFlush(devHandle) != paNoError) {
    ERR_RPT(("Pa_SetLatency: could not pause audio\n" ));
    return;
  }

  AUDIO_INITINFO(&solaris_info);

  /* AS: Doesn't look like solaris has multiple buffers,
     so I'm being conservative and
     making one buffer.  Might not be what we want... */

  solaris_info.play.buffer_size = solaris_info.record.buffer_size = bufferSize;

  if (ioctl(devHandle, AUDIO_SETINFO, &solaris_info) == -1)
    {
      ERR_RPT(("Pa_SetLatency: could not set audio info\n" ));
    }
  Pa_Unpause(devHandle);
}

/***********************************************************************/
PaTimestamp Pa_StreamTime( PortAudioStream *stream )
{
    internalPortAudioStream *past = (internalPortAudioStream *) stream;
    PaHostSoundControl *pahsc;
    audio_info_t solaris_info;

    if( past == NULL ) return paBadStreamPtr;

    pahsc = (PaHostSoundControl *) past->past_DeviceData;

    ioctl(pahsc->pahsc_OutputHandle, AUDIO_GETINFO, &solaris_info);
    return solaris_info.play.samples;
}

void Pa_UpdateStreamTime(PaHostSoundControl *pahsc)
{
  /* AS: Don't need to do anytying for this under Solaris.*/
}

static PaError Pa_PauseAndFlush(int devHandle)
{
  audio_info_t solaris_info;
  AUDIO_INITINFO(&solaris_info);

  solaris_info.play.pause = solaris_info.record.pause = 1;

  if (ioctl(devHandle, AUDIO_SETINFO, &solaris_info) == -1)
    {
      ERR_RPT(("Pa_FlushStream failed.\n"));
      return paHostError;
    }

  if (ioctl(devHandle, I_FLUSH, FLUSHRW) == -1)
    {
      ERR_RPT(("Pa_FlushStream failed.\n"));

      /* Unpause! */
      AUDIO_INITINFO(&solaris_info);
      solaris_info.play.pause = solaris_info.record.pause = 0;
      ioctl(devHandle, AUDIO_SETINFO, &solaris_info);

      return paHostError;
    }
  return paNoError;
}

static PaError Pa_Unpause(int devHandle)
{
  audio_info_t solaris_info;
  AUDIO_INITINFO(&solaris_info);

  solaris_info.play.pause = solaris_info.record.pause = 0;

  if (ioctl(devHandle, AUDIO_SETINFO, &solaris_info) == -1)
    {
      ERR_RPT(("Pa_FlushStream failed.\n"));
      return paHostError;
    }

  return paNoError;
}

PaError Pa_FlushStream(int devHandle)
{
  PaError res = Pa_PauseAndFlush(devHandle);
  if (res == paNoError)
    return Pa_Unpause(devHandle);
  else return res;
}
