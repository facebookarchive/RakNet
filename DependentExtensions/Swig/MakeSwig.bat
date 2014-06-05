@echo off
if "%1"=="" goto :NOVARS
echo Performing Swig build
set swigPath=%2
if "%swigPath%"=="" goto :SKIPADDSLASH
if "%swigPath:~-1%"=="\" goto :SKIPADDSLASH 
SET swigPath=%swigPath%\
:SKIPADDSLASH
del /F /Q SwigOutput\SwigCSharpOutput\*
if "%3"=="" goto :NOSQL
%swigPath%swig -c++ -csharp -namespace RakNet -I"%1" -I"SwigInterfaceFiles" -I"%3" -DSWIG_ADDITIONAL_SQL_LITE -outdir SwigOutput\SwigCSharpOutput -o SwigOutput\CplusDLLIncludes\RakNet_wrap.cxx SwigInterfaceFiles\RakNet.i
copy /Y SwigOutput\SwigCSharpOutput\* SwigWindowsCSharpSample\SwigTestApp\SwigFiles\*
GOTO ENDSWIG
:NOSQL
%swigPath%swig -c++ -csharp -namespace RakNet -I"%1" -I"SwigInterfaceFiles" -outdir SwigOutput\SwigCSharpOutput -o SwigOutput\CplusDLLIncludes\RakNet_wrap.cxx SwigInterfaceFiles\RakNet.i
copy /Y SwigOutput\SwigCSharpOutput\* SwigWindowsCSharpSample\SwigTestApp\SwigFiles\*
:ENDSWIG
if errorlevel 1 GOTO :SWIGERROR
echo Swig build complete
GOTO END
:NOVARS
echo Invalid number of parameters, Usage: MakeSwig.bat PATH_TO_RAKNETSOURCE PATH_TO_SWIG
PAUSE
GOTO END
:SWIGERROR
echo Swig had an error during build
:END