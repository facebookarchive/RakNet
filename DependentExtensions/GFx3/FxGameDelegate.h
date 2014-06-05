/**********************************************************************

Filename    :   FxGameDelegate.h
Content     :   Communication logic for CLIK GameDelegate
Created     :
Authors     :   Prasad Silva

Copyright   :   (c) 2005-2009 Scaleform Corp. All Rights Reserved.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

**********************************************************************/

#ifndef INC_FxGameDelegateHandler_H
#define INC_FxGameDelegateHandler_H

#include "GFxPlayer.h"
#include "GMemory.h"

class FxDelegate;
class FxDelegateArgs;


//
// Interface implemented by all callback handlers. These handlers register 
// callbacks with the FxGameDelegate.
//
class FxDelegateHandler : public GRefCountBase<FxDelegateHandler, GStat_Default_Mem>
{
public:
    virtual ~FxDelegateHandler() {}

    //
    // All callback methods must have the following signature. To produce a response
    // to the callback, push parameters to the game delegate.
    //
    typedef void (*CallbackFn)(const FxDelegateArgs& params);

    //
    // Interface implemented by callback registrars. The handler should 
    // pass the appropriate parameters to the Visit method.
    //
    class CallbackProcessor
    {
    public:
        virtual ~CallbackProcessor() {}
        virtual void    Process(const GString& methodName, CallbackFn method) = 0;
    };

    //
    // Callback registrar visitor method
    // Implementations are expected to call the registrar's Process method
    // for all callbacks.
    //
    virtual void        Accept(CallbackProcessor* cbreg) = 0;
};


//////////////////////////////////////////////////////////////////////////


//
// Callback response parameters
// The 
//
class FxResponseArgsBase
{
public:
    virtual ~FxResponseArgsBase() {}
    virtual UInt GetValues(GFxValue** pparams) = 0;
};

//
// Callback response that uses stack based storage
//
template <int N>
class FxResponseArgs : public FxResponseArgsBase
{
public:
    FxResponseArgs() : Index(1) {}  
    void    Add(const GFxValue& v)
    {
        if (Index > N) 
        {
            GFC_DEBUG_WARNING(1, "Adding parameter out of bounds!");
            return;
        }
        Values[Index++] = v;
    }
    UInt    GetValues(GFxValue** pparams) { *pparams = Values; return Index; }
private:
    GFxValue    Values[N+1];    // Space for response data
    UInt        Index;
};

//
// Callback response that uses dynamically allocated storage
//
class FxResponseArgsList : public FxResponseArgsBase
{
public:
    FxResponseArgsList()                    { Args.PushBack(GFxValue::VT_Null); }   // Space for response data
    void    Add(const GFxValue& v)          { Args.PushBack(v); }
    UInt    GetValues(GFxValue** pparams)    { *pparams = &Args[0]; return (UInt)Args.GetSize(); }
private:
    GArray<GFxValue>        Args;
};


//////////////////////////////////////////////////////////////////////////


//
// Parameters passed to the callback handler
//
class FxDelegateArgs
{
public:
    FxDelegateArgs(GFxValue responseid, FxDelegateHandler* pthis, GFxMovieView* pmovie, 
        const GFxValue* vals, UInt nargs) : ResponseID(responseid), pThis(pthis), 
        pMovieView(pmovie), Args(vals), NArgs(nargs) {}

    void Respond(FxResponseArgsBase& params) const;

    FxDelegateHandler*  GetHandler() const      { return pThis; }
    GFxMovieView*       GetMovie() const        { return pMovieView; }

    const GFxValue&     operator[](UPInt i) const
    { 
        GASSERT(i >= 0 && i < NArgs);
        return Args[i]; 
    }
    UInt                GetArgCount() const     { return NArgs; }

private:
    GFxValue                ResponseID;
    FxDelegateHandler*      pThis;
    GFxMovieView*           pMovieView;
    const GFxValue*         Args;
    UInt                    NArgs;
};


//////////////////////////////////////////////////////////////////////////


//
// Callback manager that marshals calls from ActionScript 
//
class FxDelegate : public GFxExternalInterface
{
public:
    //
    // Callback target
    //
    struct CallbackDefn
    {
        GPtr<FxDelegateHandler>         pThis;
        FxDelegateHandler::CallbackFn   pCallback;
    };

    //
    // Callback hash
    //
    struct CallbackHashFunctor
    {
        UPInt  operator()(const char* data) const
        {
            UPInt  size = G_strlen(data);
            return GString::BernsteinHashFunction(data, size);
        }
    };
    typedef GHash<GString, CallbackDefn, CallbackHashFunctor> CallbackHash;


    FxDelegate();

    //
    // Install and uninstall callbacks
    //
    void            RegisterHandler(FxDelegateHandler* callback);
    void            UnregisterHandler(FxDelegateHandler* callback);

    //
    // Call a method registered with the AS2 GameDelegate instance
    //
    static void    Invoke(GFxMovieView* pmovieView, const char* methodName, 
                            FxResponseArgsBase& args);
	
	// KevinJ: Workaround interface changes
	static void    Invoke2(GFxMovieView* pmovieView, const char* methodName, 
		FxResponseArgsBase& args);

    //
    // ExternalInterface callback entry point
    //
    void            Callback(GFxMovieView* pmovieView, const char* methodName, 
                                const GFxValue* args, UInt argCount);

private:
    //
    // Callbacks installed with the game delegate
    //
    CallbackHash    Callbacks;
};





#endif // INC_FxGameDelegateHandler_H
