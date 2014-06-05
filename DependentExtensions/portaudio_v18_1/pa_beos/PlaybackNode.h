/*
 * $Id: PlaybackNode.h,v 1.1.1.1 2002/01/22 00:52:08 phil Exp $
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

#include <be/media/MediaRoster.h>
#include <be/media/MediaEventLooper.h>
#include <be/media/BufferProducer.h>

#include "portaudio.h"

class PaPlaybackNode :
            public BBufferProducer,
            public BMediaEventLooper
{

public:
    PaPlaybackNode( uint32 channels, float frame_rate, uint32 frames_per_buffer,
                    PortAudioCallback *callback, void *user_data );
    ~PaPlaybackNode();


    /* Local methods ******************************************/

    BBuffer *FillNextBuffer(bigtime_t time);
    void SetSampleFormat(PaSampleFormat inFormat, PaSampleFormat outFormat);
    bool IsRunning();
    PaTimestamp GetStreamTime();

    /* BMediaNode methods *************************************/

    BMediaAddOn* AddOn( int32 * ) const;
    status_t HandleMessage( int32 message, const void *data, size_t size );

    /* BMediaEventLooper methods ******************************/

    void HandleEvent( const media_timed_event *event, bigtime_t lateness,
                      bool realTimeEvent );
    void NodeRegistered();

    /* BBufferProducer methods ********************************/

    status_t FormatSuggestionRequested( media_type type, int32 quality,
                                        media_format* format );
    status_t FormatProposal( const media_source& output, media_format* format );
    status_t FormatChangeRequested( const media_source& source,
                                    const media_destination& destination, media_format* io_format, int32* );

    status_t GetNextOutput( int32* cookie, media_output* out_output );
    status_t DisposeOutputCookie( int32 cookie );

    void LateNoticeReceived( const media_source& what, bigtime_t how_much,
                             bigtime_t performance_time );
    void EnableOutput( const media_source& what, bool enabled, int32* _deprecated_ );

    status_t PrepareToConnect( const media_source& what,
                               const media_destination& where, media_format* format,
                               media_source* out_source, char* out_name );
    void Connect(status_t error, const media_source& source,
                 const media_destination& destination, const media_format& format,
                 char* io_name);
    void Disconnect(const media_source& what, const media_destination& where);

    status_t SetBufferGroup(const media_source& for_source, BBufferGroup* newGroup);

    bool         mAborted;

private:
    media_output mOutput;
    media_format mPreferredFormat;
    uint32       mOutputSampleWidth, mFramesPerBuffer;
    BBufferGroup *mBufferGroup;
    bigtime_t    mDownstreamLatency, mInternalLatency, mStartTime;
    uint64       mSamplesSent;
    PortAudioCallback *mCallback;
    void         *mUserData;
    bool         mRunning;

};

