/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "WindowsIncludes.h"
#include "SendFileTo.h"
#include <shlwapi.h>
#include <tchar.h>
#include <stdio.h>
#include <direct.h>

bool CSendFileTo::SendMail(HWND hWndParent, const char *strAttachmentFilePath, const char *strAttachmentFileName, const char *strSubject, const char *strBody, const char *strRecipient)
{
//	if (strAttachmentFileName==0)
//		return false;

//	if (!hWndParent || !::IsWindow(hWndParent))
//		return false;

	HINSTANCE hMAPI = ::LoadLibraryA(_T("MAPI32.DLL"));
	if (!hMAPI)
		return false;

	ULONG (PASCAL *SendMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)SendMail = GetProcAddress(hMAPI, _T("MAPISendMail"));

	if (!SendMail)
		return false;

//	TCHAR szFileName[_MAX_PATH];
//	TCHAR szPath[_MAX_PATH];
	TCHAR szName[_MAX_PATH];
	TCHAR szSubject[_MAX_PATH];
	TCHAR szBody[_MAX_PATH];
	TCHAR szAddress[_MAX_PATH];
	TCHAR szSupport[_MAX_PATH];
	//strcpy(szFileName, (LPCTSTR)strAttachmentFileName);
	//strcpy(szPath, (LPCTSTR)strAttachmentFilePath);
	if (strAttachmentFileName)
		strcpy(szName, (LPCTSTR)strAttachmentFileName);
	strcpy(szSubject, (LPCTSTR)strSubject);
	strcpy(szBody, (LPCTSTR)strBody);
	sprintf(szAddress, "SMTP:%s", strRecipient);
	//strcpy(szSupport, _T("Support"));

	char fullPath[_MAX_PATH];
	if (strAttachmentFileName && strAttachmentFilePath)
	{
		if (strlen(strAttachmentFilePath)<3 ||
			strAttachmentFilePath[1]!=':' ||
			(strAttachmentFilePath[2]!='\\' && 
			strAttachmentFilePath[2]!='/'))
		{
			// Make relative paths absolute
			getcwd(fullPath, _MAX_PATH);
			strcat(fullPath, "/");
			strcat(fullPath, strAttachmentFilePath);
		}
		else
			strcpy(fullPath, strAttachmentFilePath);


		// All slashes have to be \\ and not /
		int len=(unsigned int)strlen(fullPath);
		int i;
		for (i=0; i < len; i++)
		{
			if (fullPath[i]=='/')
				fullPath[i]='\\';
		}
	}


	MapiFileDesc fileDesc;
	if (strAttachmentFileName && strAttachmentFilePath)
	{
		ZeroMemory(&fileDesc, sizeof(fileDesc));
		fileDesc.nPosition = (ULONG)-1;
		fileDesc.lpszPathName = fullPath;
		fileDesc.lpszFileName = szName;
	}
	
	MapiRecipDesc recipDesc;
	ZeroMemory(&recipDesc, sizeof(recipDesc));
	recipDesc.lpszName = szSupport;
	recipDesc.ulRecipClass = MAPI_TO;
	recipDesc.lpszName = szAddress+5;
	recipDesc.lpszAddress = szAddress;

	MapiMessage message;
	ZeroMemory(&message, sizeof(message));
	message.nRecipCount = 1;
	message.lpRecips = &recipDesc;
	message.lpszSubject = szSubject;
	message.lpszNoteText = szBody;
	if (strAttachmentFileName && strAttachmentFilePath)
	{
		message.nFileCount = 1;
		message.lpFiles = &fileDesc;
	}
	
	int nError = SendMail(0, (ULONG_PTR)hWndParent, &message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
		return false;

	return true;
}