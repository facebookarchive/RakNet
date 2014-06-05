/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "EmailSender.h"
#include "FileList.h"
#include <stdio.h>
#include "Gets.h"

int main()
{
	printf("A C++ class used to send email, such as for servers.\n");
	printf("TLS support (such as for Gmail) requires OPEN_SSL_CLIENT_SUPPORT to be defined\nin RakNetDefines.h.\n");
	printf("Difficulty: Beginner\n\n");

	RakNet::FileList fileList;
	RakNet::EmailSender emailSender;
	const char *quote = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
//	const char base64Map[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//	char output[1024];
//	emailSender.Base64Encoding(quote, strlen(quote), output, base64Map);
//	printf("%s", output);

	char mailServer[128], senderUsername[128], receiver[128], password[128];
	printf("Tests sending email.\n");
	printf("Enter mail server: ");
	Gets(mailServer,sizeof(mailServer));
	if (mailServer[0]==0)
		strcpy(mailServer, "smtp.gmail.com");
	printf("Enter email account username: ");
	Gets(senderUsername,sizeof(senderUsername));
	if (senderUsername[0]==0)
		strcpy(senderUsername, "subspacegod@gmail.com");
	printf("Enter receiver email address: ");
	Gets(receiver,sizeof(receiver));
	if (receiver[0]==0)
		strcpy(receiver, "rakkar@rakkar.org");
	printf("Enter password needed to send: ");
	Gets(password,sizeof(password));


	// http://mail.google.com/support/bin/answer.py?hl=en&answer=13287
	unsigned short hostPort;
	if (strcmp(mailServer,"smtp.gmail.com")==0)
		hostPort=465;
	else
		hostPort=25;

	fileList.AddFile("quote.txt", "quote.txt", quote, (const unsigned int) strlen(quote), (const unsigned int) strlen(quote), FileListNodeContext(0,0,0,0), false);
	const char *sendResult=emailSender.Send(mailServer,
		hostPort,
		senderUsername,
		receiver,
		senderUsername,
		receiver,
		"Test subject.",
		"Test attachment body :).\n.\n..\n.\n(Should be .,.,..,.)\r\n.\r\n.\r\n..\r\n.\r\n(Should be .,.,..,.)12345\r\n.\r\n",
		&fileList,
		true,
		password);
	if (sendResult!=0)
		printf("Send Failed! %s", sendResult);
	else
		printf("Success (probably).\n");
	printf("Press enter to quit.\n");
	char buff[256];
	Gets(buff,sizeof(buff));

	return 0;
}
