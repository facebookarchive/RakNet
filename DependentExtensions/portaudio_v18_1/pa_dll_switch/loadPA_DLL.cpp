//////////////////////////////////////////////////////////////////////////


HINSTANCE   pPaDll;

/*
 the function pointers to the PortAudio DLLs
*/

PaError (__cdecl* Pa_Initialize)( void );



PaError (__cdecl* Pa_Terminate)( void );


long (__cdecl* Pa_GetHostError)( void );


const char* (__cdecl* Pa_GetErrorText)( PaError );


int (__cdecl* Pa_CountDevices)(void);

PaDeviceID (__cdecl* Pa_GetDefaultInputDeviceID)( void );

PaDeviceID (__cdecl* Pa_GetDefaultOutputDeviceID)( void );


const PaDeviceInfo* (__cdecl* Pa_GetDeviceInfo)( PaDeviceID);



PaError (__cdecl* Pa_OpenStream)(
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



PaError (__cdecl* Pa_OpenDefaultStream)( PortAudioStream** stream,
        int numInputChannels,
        int numOutputChannels,
        PaSampleFormat sampleFormat,
        double sampleRate,
        unsigned long framesPerBuffer,
        unsigned long numberOfBuffers,
        PortAudioCallback *callback,
        void *userData );


PaError (__cdecl* Pa_CloseStream)( PortAudioStream* );


PaError (__cdecl* Pa_StartStream)( PortAudioStream *stream );

PaError (__cdecl* Pa_StopStream)( PortAudioStream *stream );

PaError (__cdecl* Pa_AbortStream)( PortAudioStream *stream );

PaError (__cdecl* Pa_StreamActive)( PortAudioStream *stream );

PaTimestamp (__cdecl* Pa_StreamTime)( PortAudioStream *stream );

double (__cdecl* Pa_GetCPULoad)( PortAudioStream* stream );

int (__cdecl* Pa_GetMinNumBuffers)( int framesPerBuffer, double sampleRate );

void (__cdecl* Pa_Sleep)( long msec );

PaError (__cdecl* Pa_GetSampleSize)( PaSampleFormat format );


//////////////////////////////////////////////////////////////////////////

...

ZERROR AudioEngine::DirectXSupport(ZBOOL bSupDX)
{
    if (bSupDX)
        if (CheckForDirectXSupport())
            bSupportDirectX = _TRUE;
        else
            return _NO_SOUND;
    else
        bSupportDirectX  = _FALSE;
    return _NO_ERROR;
}



ZBOOL AudioEngine::CheckForDirectXSupport()
{
    HMODULE pTestDXLib;
    FARPROC pFunctionality;

    pTestDXLib=LoadLibrary("DSOUND");
    if (pTestDXLib!=NULL)  // check if there is a DirectSound
    {
        pFunctionality = GetProcAddress(pTestDXLib, (char*) 7);
        if (pFunctionality!=NULL)
        {
            FreeLibrary(pTestDXLib);
            return _TRUE;
        }
        else
        {
            FreeLibrary(pTestDXLib);
            return _FALSE;
        }
    }
    else
        return _FALSE;
}


ZERROR AudioEngine::LoadPALib()
{
#ifdef _DEBUG
    if (bSupportDirectX)
        pPaDll  = LoadLibrary("PA_DXD");
    else
        pPaDll  = LoadLibrary("PA_MMED");
#else
    if (bSupportDirectX)
        pPaDll  = LoadLibrary("PA_DX");
    else
        pPaDll  = LoadLibrary("PA_MME");
#endif
    if (pPaDll!=NULL)
    {

        Pa_Initialize    = (int (__cdecl*)(void))GetProcAddress(pPaDll,"Pa_Initialize");
        Pa_Terminate    = (int (__cdecl*)(void))GetProcAddress(pPaDll,"Pa_Terminate");
        Pa_GetHostError    = (long (__cdecl* )( void )) GetProcAddress(pPaDll,"Pa_GetHostError");
        Pa_GetErrorText    = (const char* (__cdecl* )( PaError )) GetProcAddress(pPaDll,"Pa_GetErrorText");
        Pa_CountDevices    = (int (__cdecl*)(void))GetProcAddress(pPaDll,"Pa_CountDevices");
        Pa_GetDefaultInputDeviceID = (int (__cdecl*)(void))GetProcAddress(pPaDll,"Pa_GetDefaultInputDeviceID");
        Pa_GetDefaultOutputDeviceID = (int (__cdecl*)(void))GetProcAddress(pPaDll,"Pa_GetDefaultOutputDeviceID");
        Pa_GetDeviceInfo   = (const PaDeviceInfo* (__cdecl* )( PaDeviceID)) GetProcAddress(pPaDll,"Pa_GetDeviceInfo");
        Pa_OpenStream    = ( PaError (__cdecl* )(
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
                                 void * )) GetProcAddress(pPaDll,"Pa_OpenStream");

        Pa_OpenDefaultStream  = (PaError (__cdecl* )( PortAudioStream** ,
                                 int ,
                                 int ,
                                 PaSampleFormat ,
                                 double ,
                                 unsigned long ,
                                 unsigned long ,
                                 PortAudioCallback *,
                                 void * )) GetProcAddress(pPaDll,"Pa_OpenDefaultStream");
        Pa_CloseStream    = (PaError (__cdecl* )( PortAudioStream* )) GetProcAddress(pPaDll,"Pa_CloseStream");
        Pa_StartStream    = (PaError (__cdecl* )( PortAudioStream* )) GetProcAddress(pPaDll,"Pa_StartStream");
        Pa_StopStream    = (PaError (__cdecl* )( PortAudioStream* ))GetProcAddress(pPaDll,"Pa_StopStream");
        Pa_AbortStream    = (PaError (__cdecl* )( PortAudioStream* )) GetProcAddress(pPaDll,"Pa_AbortStream");
        Pa_StreamActive    = (PaError (__cdecl* )( PortAudioStream* )) GetProcAddress(pPaDll,"Pa_StreamActive");
        Pa_StreamTime    = (PaTimestamp (__cdecl* )( PortAudioStream *))GetProcAddress(pPaDll,"Pa_StreamTime");
        Pa_GetCPULoad    = (double (__cdecl* )( PortAudioStream* ))GetProcAddress(pPaDll,"Pa_GetCPULoad");
        Pa_GetMinNumBuffers   = (int (__cdecl* )( int , double )) GetProcAddress(pPaDll,"Pa_GetMinNumBuffers");
        Pa_Sleep     = (void (__cdecl* )( long )) GetProcAddress(pPaDll,"Pa_Sleep");
        Pa_GetSampleSize   = (PaError (__cdecl* )( PaSampleFormat )) GetProcAddress(pPaDll,"Pa_GetSampleSize");

        return _NO_ERROR;
    }
    else
        return _DLL_NOT_FOUND;
}

ZERROR AudioEngine::UnLoadPALib()
{
    if (pPaDll!=NULL)
        FreeLibrary(pPaDll);
    return _NO_ERROR;
}

...
