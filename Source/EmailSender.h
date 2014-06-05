/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file EmailSender.h
/// \brief Rudimentary class to send email from code.  Don't expect anything fancy.
///

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_EmailSender==1 && _RAKNET_SUPPORT_TCPInterface==1 && _RAKNET_SUPPORT_FileOperations==1

#ifndef __EMAIL_SENDER_H
#define __EMAIL_SENDER_H

#include "RakNetTypes.h"
#include "RakMemoryOverride.h"
#include "Export.h"
#include "Rand.h"
#include "TCPInterface.h"

namespace RakNet
{
/// Forward declarations
class FileList;
class TCPInterface;

/// \brief Rudimentary class to send email from code.
class RAK_DLL_EXPORT EmailSender
{
public:
	// GetInstance() and DestroyInstance(instance*)
	STATIC_FACTORY_DECLARATIONS(EmailSender)

	/// \brief Sends an email.
	/// \param[in] hostAddress The address of the email server.
	/// \param[in] hostPort The port of the email server (usually 25)
	/// \param[in] sender The email address you are sending from.
	/// \param[in] recipient The email address you are sending to.
	/// \param[in] senderName The email address you claim to be sending from
	/// \param[in] recipientName The email address you claim to be sending to
	/// \param[in] subject Email subject
	/// \param[in] body Email body
	/// \param[in] attachedFiles List of files to attach to the email. (Can be 0 to send none).
	/// \param[in] doPrintf true to output SMTP info to console(for debugging?)
	/// \param[in] password Used if the server uses AUTHENTICATE PLAIN over TLS (such as gmail)
	/// \return 0 on success, otherwise a string indicating the error message
	const char *Send(const char *hostAddress, unsigned short hostPort, const char *sender, const char *recipient, const char *senderName, const char *recipientName, const char *subject, const char *body, FileList *attachedFiles, bool doPrintf, const char *password);

protected:
	const char *GetResponse(TCPInterface *tcpInterface, const SystemAddress &emailServer, bool doPrintf);
	RakNetRandom rakNetRandom;
};

} // namespace RakNet

#endif


#endif // _RAKNET_SUPPORT_*
