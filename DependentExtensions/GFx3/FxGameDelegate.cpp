/**********************************************************************

Filename    :   FxGameDelegate.cpp
Content     :   Communication logic for CLIK GameDelegate
Created     :
Authors     :   Prasad Silva

Copyright   :   (c) 2005-2009 Scaleform Corp. All Rights Reserved.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

**********************************************************************/

#include "FxGameDelegate.h"

//
// Visitor to register callbacks
//
class AddCallbackVisitor : public FxDelegateHandler::CallbackProcessor
{
public:
    AddCallbackVisitor(FxDelegateHandler* pthis, FxDelegate::CallbackHash* phash)
        : pThis(pthis), pHash(phash) {}

    void Process(const GString& methodName, FxDelegateHandler::CallbackFn method)
    {
        FxDelegate::CallbackDefn cbt;
        cbt.pThis = pThis;
        cbt.pCallback = method;
        pHash->Add(methodName, cbt);
    }

private:
    FxDelegateHandler*          pThis;
    FxDelegate::CallbackHash*   pHash;
};


//
// Visitor to unregister callbacks
//
class RemoveCallbackVisitor : public FxDelegateHandler::CallbackProcessor
{
public:
    RemoveCallbackVisitor(FxDelegateHandler* pthis, FxDelegate::CallbackHash* phash)
        : pThis(pthis), pHash(phash) {}

    void Process(const GString& methodName, FxDelegateHandler::CallbackFn method)
    {
        GUNUSED(method);
        pHash->Remove(methodName);
    }

private:
    FxDelegateHandler*          pThis;
    FxDelegate::CallbackHash*   pHash;
};

//////////////////////////////////////////////////////////////////////////

FxDelegate::FxDelegate()
{
     
}

void FxDelegate::RegisterHandler(FxDelegateHandler* callback)
{
    AddCallbackVisitor reg(callback, &Callbacks);
    callback->Accept(&reg);
}

void FxDelegate::UnregisterHandler(FxDelegateHandler* callback)
{
    RemoveCallbackVisitor reg(callback, &Callbacks);
    callback->Accept(&reg);
}

void FxDelegate::Invoke(GFxMovieView* pmovieView, const char* methodName, 
                            FxResponseArgsBase& args)
{
    GFxValue* pv = NULL;
    UInt nv = args.GetValues(&pv);
    pv[0] = methodName;
    pmovieView->Invoke("call", pv, nv);
}
void FxDelegate::Invoke2(GFxMovieView* pmovieView, const char* methodName, 
						FxResponseArgsBase& args)
{
	GFxValue* pv = NULL;
	UInt nv = args.GetValues(&pv);
	pmovieView->Invoke(methodName, NULL, pv+1, nv-1);
}

void FxDelegate::Callback(GFxMovieView* pmovieView, const char* methodName, const GFxValue* args, UInt argCount)
{
	// KevinJ: With calling ExternalInterface from flash, this is apparently obsolete now
 //   GASSERT(argCount > 0);  // Must at least have a uid parameter

    CallbackDefn* pcb = Callbacks.GetAlt(methodName);
    if (pcb != NULL) 
    {
		// KevinJ: With calling ExternalInterface from flash, this is apparently obsolete now
//         FxDelegateArgs params(args[0], 
//                               pcb->pThis, 
//                               pmovieView, 
//                               &args[1], 
//                               argCount - 1);
		FxDelegateArgs params(GFxValue(), 
			pcb->pThis, 
			pmovieView, 
			argCount>=1 ? &args[0] : NULL, 
			argCount);
        pcb->pCallback(params);
    }
}


//////////////////////////////////////////////////////////////////////////


void FxDelegateArgs::Respond(FxResponseArgsBase& params) const
{
    GFxValue* pv = NULL;
    UInt nv = params.GetValues(&pv);
    pv[0] = ResponseID;
    pMovieView->Invoke("respond", pv, nv);
}
