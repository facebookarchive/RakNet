/*
 * $Id: PlaybackNode.cc,v 1.1.1.1 2002/01/22 00:52:07 phil Exp $
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
 * ---
 *
 * Significant portions of this file are based on sample code from Be. The
 * Be Sample Code Licence follows:
 *
 *    Copyright 1991-1999, Be Incorporated.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions, and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions, and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *    3. The name of the author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 *    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *    OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *    PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
 */

#include <stdio.h>

#include <be/media/BufferGroup.h>
#include <be/media/Buffer.h>
#include <be/media/TimeSource.h>

#include "PlaybackNode.h"

#define PRINT(x) { printf x; fflush(stdout); }

#ifdef DEBUG
#define DBUG(x)  PRINT(x)
#else
#define DBUG(x)
#endif


PaPlaybackNode::PaPlaybackNode(uint32 channels, float frame_rate, uint32 frames_per_buffer,
                               PortAudioCallback* callback, void *user_data) :
        BMediaNode("PortAudio input node"),
        BBufferProducer(B_MEDIA_RAW_AUDIO),
        BMediaEventLooper(),
        mAborted(false),
        mRunning(false),
        mBufferGroup(NULL),
        mDownstreamLatency(0),
        mStartTime(0),
        mCallback(callback),
        mUserData(user_data),
        mFramesPerBuffer(frames_per_buffer)
{
    DBUG(("Constructor called.\n"));

    mPreferredFormat.type = B_MEDIA_RAW_AUDIO;
    mPreferredFormat.u.raw_audio.channel_count = channels;
    mPreferredFormat.u.raw_audio.frame_rate = frame_rate;
    mPreferredFormat.u.raw_audio.byte_order =
        (B_HOST_IS_BENDIAN) ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN;
    mPreferredFormat.u.raw_audio.buffer_size =
        media_raw_audio_format::wildcard.buffer_size;

    mOutput.destination = media_destination::null;
    mOutput.format = mPreferredFormat;

    /* The amount of time it takes for this node to produce a buffer when
     * asked. Essentially, it is how long the user's callback takes to run.
     * We set this to be the length of the sound data each buffer of the
     * requested size can hold. */
    //mInternalLatency = (bigtime_t)(1000000 * frames_per_buffer / frame_rate);

    /* ACK! it seems that the mixer (at least on my machine) demands that IT
        * specify the buffer size, so for now I'll just make a generic guess here */
    mInternalLatency = 1000000 / 20;
}



PaPlaybackNode::~PaPlaybackNode()
{
    DBUG(("Destructor called.\n"));
    Quit();   /* Stop the BMediaEventLooper thread */
}


/*************************
 *
 *  Local methods
 *
 */

bool PaPlaybackNode::IsRunning()
{
    return mRunning;
}


PaTimestamp PaPlaybackNode::GetStreamTime()
{
    BTimeSource *timeSource = TimeSource();
    PaTimestamp time = (timeSource->Now() - mStartTime) *
                       mPreferredFormat.u.raw_audio.frame_rate / 1000000;
    return time;
}


void PaPlaybackNode::SetSampleFormat(PaSampleFormat inFormat,
                                     PaSampleFormat outFormat)
{
    uint32 beOutFormat;

    switch(outFormat)
    {
    case paFloat32:
        beOutFormat = media_raw_audio_format::B_AUDIO_FLOAT;
        mOutputSampleWidth = 4;
        break;

    case paInt16:
        beOutFormat = media_raw_audio_format::B_AUDIO_SHORT;
        mOutputSampleWidth = 2;
        break;

    case paInt32:
        beOutFormat = media_raw_audio_format::B_AUDIO_INT;
        mOutputSampleWidth = 4;
        break;

    case paInt8:
        beOutFormat = media_raw_audio_format::B_AUDIO_CHAR;
        mOutputSampleWidth = 1;
        break;

    case paUInt8:
        beOutFormat = media_raw_audio_format::B_AUDIO_UCHAR;
        mOutputSampleWidth = 1;
        break;

    case paInt24:
    case paPackedInt24:
    case paCustomFormat:
        DBUG(("Unsupported output format: %x\n", outFormat));
        break;

    default:
        DBUG(("Unknown output format: %x\n", outFormat));
    }

    mPreferredFormat.u.raw_audio.format = beOutFormat;
    mFramesPerBuffer * mPreferredFormat.u.raw_audio.channel_count * mOutputSampleWidth;
}

BBuffer *PaPlaybackNode::FillNextBuffer(bigtime_t time)
{
    /* Get a buffer from the buffer group */
    BBuffer *buf = mBufferGroup->RequestBuffer(
                       mOutput.format.u.raw_audio.buffer_size, BufferDuration());
    unsigned long frames = mOutput.format.u.raw_audio.buffer_size /
                           mOutputSampleWidth / mOutput.format.u.raw_audio.channel_count;
    bigtime_t start_time;
    int ret;

    if( !buf )
    {
        DBUG(("Unable to allocate a buffer\n"));
        return NULL;
    }

    start_time = mStartTime +
                 (bigtime_t)((double)mSamplesSent /
                             (double)mOutput.format.u.raw_audio.frame_rate /
                             (double)mOutput.format.u.raw_audio.channel_count *
                             1000000.0);

    /* Now call the user callback to get the data */
    ret = mCallback(NULL,       /* Input buffer */
                    buf->Data(),      /* Output buffer */
                    frames,           /* Frames per buffer */
                    mSamplesSent / mOutput.format.u.raw_audio.channel_count, /* timestamp */
                    mUserData);

    if( ret )
        mAborted = true;

    media_header *hdr = buf->Header();

    hdr->type = B_MEDIA_RAW_AUDIO;
    hdr->size_used = mOutput.format.u.raw_audio.buffer_size;
    hdr->time_source = TimeSource()->ID();
    hdr->start_time = start_time;

    return buf;
}




/*************************
 *
 *  BMediaNode methods
 *
 */

BMediaAddOn *PaPlaybackNode::AddOn( int32 * ) const
{
    DBUG(("AddOn() called.\n"));
    return NULL;  /* we don't provide service to outside applications */
}


status_t PaPlaybackNode::HandleMessage( int32 message, const void *data,
                                        size_t size )
{
    DBUG(("HandleMessage() called.\n"));
    return B_ERROR;  /* we don't define any custom messages */
}




/*************************
 *
 *  BMediaEventLooper methods
 *
 */

void PaPlaybackNode::NodeRegistered()
{
    DBUG(("NodeRegistered() called.\n"));

    /* Start the BMediaEventLooper thread */
    SetPriority(B_REAL_TIME_PRIORITY);
    Run();

    /* set up as much information about our output as we can */
    mOutput.source.port = ControlPort();
    mOutput.source.id = 0;
    mOutput.node = Node();
    ::strcpy(mOutput.name, "PortAudio Playback");
}


void PaPlaybackNode::HandleEvent( const media_timed_event *event,
                                  bigtime_t lateness, bool realTimeEvent )
{
    // DBUG(("HandleEvent() called.\n"));
    status_t err;

    switch(event->type)
    {
    case BTimedEventQueue::B_START:
        DBUG(("   Handling a B_START event\n"));
        if( RunState() != B_STARTED )
        {
            mStartTime = event->event_time + EventLatency();
            mSamplesSent = 0;
            mAborted = false;
            mRunning = true;
            media_timed_event firstEvent( mStartTime,
                                          BTimedEventQueue::B_HANDLE_BUFFER );
            EventQueue()->AddEvent( firstEvent );
        }
        break;

    case BTimedEventQueue::B_STOP:
        DBUG(("   Handling a B_STOP event\n"));
        mRunning = false;
        EventQueue()->FlushEvents( 0, BTimedEventQueue::B_ALWAYS, true,
                                   BTimedEventQueue::B_HANDLE_BUFFER );
        break;

    case BTimedEventQueue::B_HANDLE_BUFFER:
        //DBUG(("   Handling a B_HANDLE_BUFFER event\n"));

        /* make sure we're started and connected */
        if( RunState() != BMediaEventLooper::B_STARTED ||
                mOutput.destination == media_destination::null )
            break;

        BBuffer *buffer = FillNextBuffer(event->event_time);

        /* make sure we weren't aborted while this routine was running.
         * this can happen in one of two ways: either the callback returned
         * nonzero (in which case mAborted is set in FillNextBuffer() ) or
         * the client called AbortStream */
        if( mAborted )
        {
            if( buffer )
                buffer->Recycle();
            Stop(0, true);
            break;
        }

        if( buffer )
        {
            err = SendBuffer(buffer, mOutput.destination);
            if( err != B_OK )
                buffer->Recycle();
        }

        mSamplesSent += mOutput.format.u.raw_audio.buffer_size / mOutputSampleWidth;

        /* Now schedule the next buffer event, so we can send another
         * buffer when this one runs out. We calculate when it should
         * happen by calculating when the data we just sent will finish
         * playing.
         *
         * NOTE, however, that the event will actually get generated
         * earlier than we specify, to account for the latency it will
         * take to produce the buffer. It uses the latency value we
         * specified in SetEventLatency() to determine just how early
         * to generate it. */

        /* totalPerformanceTime includes the time represented by the buffer
         * we just sent */
        bigtime_t totalPerformanceTime = (bigtime_t)((double)mSamplesSent /
                                         (double)mOutput.format.u.raw_audio.channel_count /
                                         (double)mOutput.format.u.raw_audio.frame_rate * 1000000.0);

        bigtime_t nextEventTime = mStartTime + totalPerformanceTime;

        media_timed_event nextBufferEvent(nextEventTime,
                                          BTimedEventQueue::B_HANDLE_BUFFER);
        EventQueue()->AddEvent(nextBufferEvent);

        break;

    }
}




/*************************
 *
 *  BBufferProducer methods
 *
 */

status_t PaPlaybackNode::FormatSuggestionRequested( media_type type,
        int32 /*quality*/, media_format* format )
{
    /* the caller wants to know this node's preferred format and provides
     * a suggestion, asking if we support it */
    DBUG(("FormatSuggestionRequested() called.\n"));

    if(!format)
        return B_BAD_VALUE;

    *format = mPreferredFormat;

    /* we only support raw audio (a wildcard is okay too) */
    if ( type == B_MEDIA_UNKNOWN_TYPE || type == B_MEDIA_RAW_AUDIO )
        return B_OK;
    else
        return B_MEDIA_BAD_FORMAT;
}


status_t PaPlaybackNode::FormatProposal( const media_source& output,
        media_format* format )
{
    /* This is similar to FormatSuggestionRequested(), but it is actually part
     * of the negotiation process. We're given the opportunity to specify any
     * properties that are wildcards (ie. properties that the other node doesn't
     * care one way or another about) */
    DBUG(("FormatProposal() called.\n"));

    /* Make sure this proposal really applies to our output */
    if( output != mOutput.source )
        return B_MEDIA_BAD_SOURCE;

    /* We return two things: whether we support the proposed format, and our own
     * preferred format */
    *format = mPreferredFormat;

    if( format->type == B_MEDIA_UNKNOWN_TYPE || format->type == B_MEDIA_RAW_AUDIO )
        return B_OK;
    else
        return B_MEDIA_BAD_FORMAT;
}


status_t PaPlaybackNode::FormatChangeRequested( const media_source& source,
        const media_destination& destination, media_format* io_format, int32* )
{
    /* we refuse to change formats, supporting only 1 */
    DBUG(("FormatChangeRequested() called.\n"));

    return B_ERROR;
}


status_t PaPlaybackNode::GetNextOutput( int32* cookie, media_output* out_output )
{
    /* this is where we allow other to enumerate our outputs -- the cookie is
     * an integer we can use to keep track of where we are in enumeration. */
    DBUG(("GetNextOutput() called.\n"));

    if( *cookie == 0 )
    {
        *out_output = mOutput;
        *cookie = 1;
        return B_OK;
    }

    return B_BAD_INDEX;
}


status_t PaPlaybackNode::DisposeOutputCookie( int32 cookie )
{
    DBUG(("DisposeOutputCookie() called.\n"));
    return B_OK;
}


void PaPlaybackNode::LateNoticeReceived( const media_source& what,
        bigtime_t how_much, bigtime_t performance_time )
{
    /* This function is called as notification that a buffer we sent wasn't
     * received by the time we stamped it with -- it got there late. Basically,
     * it means we underestimated our own latency, so we should increase it */
    DBUG(("LateNoticeReceived() called.\n"));

    if( what != mOutput.source )
        return;

    if( RunMode() == B_INCREASE_LATENCY )
    {
        mInternalLatency += how_much;
        SetEventLatency( mDownstreamLatency + mInternalLatency );
        DBUG(("Increasing latency to %Ld\n", mDownstreamLatency + mInternalLatency));
    }
    else
        DBUG(("I don't know what to do with this notice!"));
}


void PaPlaybackNode::EnableOutput( const media_source& what, bool enabled,
                                   int32* )
{
    DBUG(("EnableOutput() called.\n"));
    /* stub -- we don't support this yet */
}


status_t PaPlaybackNode::PrepareToConnect( const media_source& what,
        const media_destination& where, media_format* format,
        media_source* out_source, char* out_name )
{
    /* the final stage of format negotiations. here we _must_ make specific any
     * remaining wildcards */
    DBUG(("PrepareToConnect() called.\n"));

    /* make sure this really refers to our source */
    if( what != mOutput.source )
        return B_MEDIA_BAD_SOURCE;

    /* make sure we're not already connected */
    if( mOutput.destination != media_destination::null )
        return B_MEDIA_ALREADY_CONNECTED;

    if( format->type != B_MEDIA_RAW_AUDIO )
        return B_MEDIA_BAD_FORMAT;

    if( format->u.raw_audio.format != mPreferredFormat.u.raw_audio.format )
        return B_MEDIA_BAD_FORMAT;

    if( format->u.raw_audio.buffer_size ==
            media_raw_audio_format::wildcard.buffer_size )
    {
        DBUG(("We were left to decide buffer size: choosing 2048"));
        format->u.raw_audio.buffer_size = 2048;
    }
    else
        DBUG(("Using consumer specified buffer size of %lu.\n",
              format->u.raw_audio.buffer_size));

    /* Reserve the connection, return the information */
    mOutput.destination = where;
    mOutput.format      = *format;
    *out_source         = mOutput.source;
    strncpy( out_name, mOutput.name, B_MEDIA_NAME_LENGTH );

    return B_OK;
}


void PaPlaybackNode::Connect(status_t error, const media_source& source,
                             const media_destination& destination, const media_format& format, char* io_name)
{
    DBUG(("Connect() called.\n"));

