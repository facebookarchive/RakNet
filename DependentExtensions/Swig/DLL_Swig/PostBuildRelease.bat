IF EXIST "../SwigWindowsCSharpSample\SwigTestApp\bin" GOTO SKIPMAKEBIN
mkdir "../SwigWindowsCSharpSample\SwigTestApp\bin"
:SKIPMAKEBIN
IF EXIST "../SwigWindowsCSharpSample\SwigTestApp\bin\X86" GOTO SKIPMAKEX86
mkdir "../SwigWindowsCSharpSample\SwigTestApp\bin\X86"
:SKIPMAKEX86
IF EXIST "../SwigWindowsCSharpSample\SwigTestApp\bin\X86\Release" GOTO SKIPMAKEOUTDIR
mkdir "../SwigWindowsCSharpSample\SwigTestApp\bin\X86\Release"
:SKIPMAKEOUTDIR
copy /Y "Release\RakNet.dll"  "../SwigWindowsCSharpSample\SwigTestApp\bin\X86\Release\RakNet.dll"
copy /Y "../SwigOutput\SwigCSharpOutput\*.cs" "../SwigWindowsCSharpSample\SwigTestApp\SwigFiles\"