/*
 * $Id: pa_beos_mk.cc,v 1.1.1.1 2002/01/22 00:52:09 phil Exp $
 * PortAudio Portable Real-Time Audio Library
 * Latest Version at: http://www.portaudio.com
 * BeOS Media Kit Implementation by Joshua Haberman
 *
 * Copyright (c) 2001 Joshua Haberman <joshua@haberman.com>
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

#include <be/app/Application.h>
#include <be/kernel/OS.h>
#include <be/media/RealtimeAlloc.h>
#include <be/media/MediaRoster.h>
#include <be/media/TimeSource.h>

#include <stdio.h>
#include <string.h>

#include "portaudio.h"
#include "pa_host.h"

#include "PlaybackNode.h"

#define PRINT(x) { printf x; fflush(stdout); }

#ifdef DEBUG
#define DBUG(x)  PRINT(x)
#else
#define DBUG(x)
#endif

typedef struct PaHostSoundControl
{
    /* These members are common to all modes of operation */
    media_node   pahsc_TimeSource;  /* the sound card's DAC. */
    media_format pahsc_Format;

    /* These methods are specific to playing mode */
    media_node   pahsc_OutputNode;  /* output to the mixer */
    media_node   pahsc_InputNode;   /* reads data from user callback -- PA specific */

    media_input  pahsc_MixerInput;  /* input jack on the soundcard's mixer. */
    media_output pahsc_PaOutput;    /* output jack from the PA node */

    PaPlaybackNode *pahsc_InputNodeInstance;

}
PaHostSoundControl;

/*************************************************************************/
PaDeviceID Pa_GetDefaultOutputDeviceID( void )
{
    /* stub */
    return 0;
}

/*************************************************************************/
PaDeviceID Pa_GetDefaultInputDeviceID( void )
{
    /* stub */
    return 0;
}

/*************************************************************************/
const PaDeviceInfo* Pa_GetDeviceInfo( PaDeviceID id )
{
    /* stub */
    return NULL;
}

/*************************************************************************/
int Pa_CountDevices()
{
    /* stub */
    return 1;
}

/*************************************************************************/
PaError PaHost_Init( void )
{
    /* we have to create this in order to use BMediaRoster. I hope it doesn't
     * cause problems */
    be_app = new BApplication("application/x-vnd.portaudio-app");

    return paNoError;
}

PaError PaHost_Term( void )
{
    delete be_app;
    return paNoError;
}

/*************************************************************************/
PaError PaHost_StreamActive( internalPortAudioStream   *past )
{
    PaHostSoundControl *pahsc = (PaHostSoundControl *)past->past_DeviceData;
    DBUG(("IsRunning returning: %s\n",
          pahsc->pahsc_InputNodeInstance->IsRunning() ? "true" : "false"));

    return (PaError)pahsc->pahsc_InputNodeInstance->IsRunning();
}

PaError PaHost_StartOutput( internalPortAudioStream *past )
{
    return paNoError;
}

/*************************************************************************/
PaError PaHost_StartInput( internalPortAudioStream *past )
{
    return paNoError;
}

/*************************************************************************/
PaError PaHost_StopInput( internalPortAudioStream *past, int abort )
{
    return paNoError;
}

/*************************************************************************/
PaError PaHost_StopOutput( internalPortAudioStream *past, int abort )
{
    return paNoError;
}


/*************************************************************************/
PaError PaHost_StartEngine( internalPortAudioStream *past )
{
    bigtime_t very_soon, start_latency;
    status_t err;
    BMediaRoster *roster = BMediaRoster::Roster(&err);
    PaHostSoundControl *pahsc = (PaHostSoundControl *)past->past_DeviceData;

    /* for some reason, err indicates an error (though nothing it wrong)
     * when the DBUG macro in pa_lib.c is enabled. It's reproducably 
     * linked. Weird. */
    if( !roster /* || err != B_OK */ )
    {
        DBUG(("No media server! err=%d, roster=%x\n", err, roster));
        return paHostError;
    }

    /* tell the node when to start -- since there aren't any other nodes
     * starting that we have to wait for, just tell it to start now
     */

    BTimeSource *timeSource = roster->MakeTimeSourceFor(pahsc->pahsc_TimeSource);
    very_soon = timeSource->PerformanceTimeFor( BTimeSource::RealTime() );
    timeSource->Release();

    /* Add the latency of starting the network of nodes */
    err = roster->GetStartLatencyFor( pahsc->pahsc_TimeSource, &start_latency );
    very_soon += start_latency;

    err = roster->StartNode( pahsc->pahsc_InputNode, very_soon );
    /* No need to start the mixer -- it's always running */

    return paNoError;
}


/*************************************************************************/
PaError PaHost_StopEngine( internalPortAudioStream *past, int abort )
{
    PaHostSoundControl *pahsc = (PaHostSoundControl *)past->past_DeviceData;
    BMediaRoster *roster = BMediaRoster::Roster();

    if( !roster )
    {
        DBUG(("No media roster!\n"));
        return paHostError;
    }

    if( !pahsc )
        return paHostError;

    /* this crashes, and I don't know why yet */
    // if( abort )
    //  pahsc->pahsc_InputNodeInstance->mAborted = true;

    roster->StopNode(pahsc->pahsc_InputNode, 0, /* immediate = */ true);

    return paNoError;
}


/*************************************************************************/
PaError PaHost_OpenStream( internalPortAudioStream   *past )
{
    status_t err;
    BMediaRoster *roster = BMediaRoster::Roster(&err);
    PaHostSoundControl *pahsc;

    /* Allocate and initialize host data. */
    pahsc = (PaHostSoundControl *) PaHost_AllocateFastMemory(sizeof(PaHostSoundControl));
    if( pahsc == NULL )
    {
        goto error;
    }
    memset( pahsc, 0, sizeof(PaHostSoundControl) );
    past->past_DeviceData = (void *) pahsc;

    if( !roster /* || err != B_OK */ )
    {
        /* no media server! */
        DBUG(("No media server.\n"));
        goto error;
    }

    if ( past->past_NumInputChannels > 0 && past->past_NumOutputChannels > 0 )
    {
        /* filter -- not implemented yet */
        goto error;
    }
    else if ( past->past_NumInputChannels > 0 )
    {
        /* recorder -- not implemented yet */
        goto error;
    }
    else
    {
        /* player ****************************************************************/

        status_t err;
        int32 num;

        /* First we need to create the three components (like components in a stereo
         * system). The mixer component is our interface to the sound card, data
         * we write there will get played. The BePA_InputNode component is the node
         * which represents communication with the PA client (it is what calls the
         * client's callbacks). The time source component is the sound card's DAC,
         * which allows us to slave the other components to it instead of the system
         * clock. */
        err = roster->GetAudioMixer( &pahsc->pahsc_OutputNode );
        if( err != B_OK )
        {
            DBUG(("Couldn't get default mixer.\n"));
            goto error;
        }

        err = roster->GetTimeSource( &pahsc->pahsc_TimeSource );
        if( err != B_OK )
        {
            DBUG(("Couldn't get time source.\n"));
            goto error;
        }

        pahsc->pahsc_InputNodeInstance = new PaPlaybackNode(2, 44100,
                                         past->past_FramesPerUserBuffer, past->past_Callback, past->past_UserData );
        pahsc->pahsc_InputNodeInstance->SetSampleFormat(0,
                past->past_OutputSampleFormat);
        err = roster->RegisterNode( pahsc->pahsc_InputNodeInstance );
        if( err != B_OK )
        {
            DBUG(("Unable to register node.\n"));
            goto error;
        }

        roster->GetNodeFor( pahsc->pahsc_InputNodeInstance->Node().node,
                            &pahsc->pahsc_InputNode );
        if( err != B_OK )
        {
            DBUG(("Unable to get input node.\n"));
            goto error;
        }

        /* Now we have three components (nodes) sitting next to each other. The
         * next step is to look at them and find their inputs and outputs so we can
         * wire them together. */
        err = roster->GetFreeInputsFor( pahsc->pahsc_OutputNode,
                                        &pahsc->pahsc_MixerInput, 1, &num, B_MEDIA_RAW_AUDIO );
        if( err != B_OK || num < 1 )
        {
            DBUG(("Couldn't get the mixer input.\n"));
            goto error;
        }

        err = roster->GetFreeOutputsFor( pahsc->pahsc_InputNode,
                                         &pahsc->pahsc_PaOutput, 1, &num, B_MEDIA_RAW_AUDIO );
        if( err != B_OK || num < 1 )
        {
            DBUG(("Couldn't get PortAudio output.\n"));
            goto error;
        }


        /* We've found the input and output -- the final step is to run a wire
         * between them so they are connected. */

        /* try to make the mixer input adapt to what PA sends it */
        pahsc->pahsc_Format = pahsc->pahsc_PaOutput.format;
        roster->Connect( pahsc->pahsc_PaOutput.source,
                         pahsc->pahsc_MixerInput.destination, &pahsc->pahsc_Format,
                         &pahsc->pahsc_PaOutput, &pahsc->pahsc_MixerInput );


        /* Actually, there's one final step -- tell them all to sync to the
         * sound card's DAC */
        roster->SetTimeSourceFor( pahsc->pahsc_InputNode.node,
                                  pahsc->pahsc_TimeSource.node );
        roster->SetTimeSourceFor( pahsc->pahsc_OutputNode.node,
                                  pahsc->pahsc_TimeSource.node );

    }

    return paNoError;

error:
    PaHost_CloseStream( past );
    return paHostError;
}

/*************************************************************************/
PaError PaHost_CloseStream( internalPortAudioStream   *past )
{
    PaHostSoundControl *pahsc = (PaHostSoundControl *)past->past_DeviceData;
    status_t err;
    BMediaRoster *roster = BMediaRoster::Roster(&err);

    if( !roster )
    {
        DBUG(("Couldn't get media roster\n"));
        return paHostError;
    }

    if( !pahsc )
        return paHostError;

    /* Disconnect all the connections we made when opening the stream */

    roster->Disconnect(pahsc->pahsc_InputNode.node, pahsc->pahsc_PaOutput.source,
                       pahsc->pahsc_OutputNode.node, pahsc->pahsc_MixerInput.destination);

    DBUG(("Calling ReleaseNode()"));
    roster->ReleaseNode(pahsc->pahsc_InputNode);

    /* deleting the node shouldn't be necessary -- it is reference counted, and will
     * delete itself when its references drop to zero. the call to ReleaseNode()
     * above  should decrease its reference count */
    pahsc->pahsc_InputNodeInstance = NULL;

    return paNoError;
}

/*************************************************************************/
PaTimestamp Pa_StreamTime( PortAudioStream *stream )
{
    internalPortAudioStream *past = (internalPortAudioStream *) stream;
    PaHostSoundControl *pahsc = (PaHostSoundControl *)past->past_DeviceData;

    return pahsc->pahsc_InputNodeInstance->GetStreamTime();
}

/*************************************************************************/
void Pa_Sleep( long msec )
{
    /* snooze() takes microseconds */
    snooze( msec * 1000 );
}

/*************************************************************************
 * Allocate memory that can be accessed in real-time.
 * This may need to be held in physical memory so that it is not
 * paged to virtual memory.
 * This call MUST be balanced with a call to PaHost_FreeFastMemory().
 * Memory will be set to zero.
 */
void *PaHost_AllocateFastMemory( long numBytes )
{
    /* BeOS supports non-pagable memory through pools -- a pool is an area
     * of physical memory that is locked. It would be best to pre-allocate
     * that pool and then hand out memory from it, but we don't know in
     * advance how much we'll need. So for now, we'll allocate a pool
     * for every request we get, storing a pointer to the pool at the
     * beginning of the allocated memory */
    rtm_pool *pool;
    void *addr;
    long size = numBytes + sizeof(rtm_pool *);
    static int counter = 0;
    char pool_name[100];

    /* Every pool needs a unique name. */
    sprintf(pool_name, "PaPoolNumber%d", counter++);

    if( rtm_create_pool( &pool, size, pool_name ) != B_OK )
        return 0;

    addr = rtm_alloc( pool, size );
    if( addr == NULL )
        return 0;

    memset( addr, 0, numBytes );
    *((rtm_pool **)addr) = pool;    // store the pointer to the pool
    addr = (rtm_pool **)addr + 1;   // and return the next location in memory

    return addr;
}

/*************************************************************************
 * Free memory that could be accessed in real-time.
 * This call MUST be balanced with a call to PaHost_AllocateFastMemory().
 */
void PaHost_FreeFastMemory( void *addr, long numBytes )
{
    rtm_pool *pool;

    if( addr == NULL )
        return;

    addr = (rtm_pool **)addr - 1;
    pool = *((rtm_pool **)addr);

    rtm_free( addr );
    rtm_delete_pool( pool );
}
