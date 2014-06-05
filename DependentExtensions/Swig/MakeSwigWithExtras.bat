@echo off
SetLocal EnableDelayedExpansion
set sourceDir=%1
CALL :dequote sourceDir
if "%sourceDir%"=="" goto :NOVARS
echo Performing Swig build
set swigPath=%2
CALL :dequote swigPath
if "%swigPath%"=="" goto :SKIPADDSLASH
if "%swigPath:~-1%"=="\" goto :SKIPADDSLASH 
SET swigPath=%swigPath%\
:SKIPADDSLASH
del /F /Q SwigOutput\SwigCSharpOutput\*
set option=%4
CALL :dequote option
set dependentDir=%3
CALL :dequote dependentDir
if "%option%"=="MYSQL_AUTOPATCHER" goto :MYSQLAUTOPATCHER
echo Unsupported option
GOTO END
:MYSQLAUTOPATCHER
%swigPath%swig -c++ -csharp -namespace RakNet -I"%1" -I"SwigInterfaceFiles" -I"%3" -DSWIG_ADDITIONAL_AUTOPATCHER_MYSQL -outdir SwigOutput\SwigCSharpOutput -o SwigOutput\CplusDLLIncludes\RakNet_wrap.cxx SwigInterfaceFiles\RakNet.i
:ENDSWIG
if errorlevel 1 GOTO :SWIGERROR
echo Swig build complete
GOTO END
:NOVARS
echo Invalid number of parameters, Usage: MakeSwigWithExtras.bat ^<Directory^> ^<Swig location or ""^> ^<Dependent Extensions Path^> ^<Option1 Example: MYSQL_AUTOPATCHER^>
PAUSE
GOTO END
:SWIGERROR
echo Swig had an error during build
:END
GOTO FILEEND

:DeQuote

SET _DeQuoteVar=%1
CALL SET _DeQuoteString=%%!_DeQuoteVar!%%
IF [!_DeQuoteString:~0^,1!]==[^"] (
IF [!_DeQuoteString:~-1!]==[^"] (
SET _DeQuoteString=!_DeQuoteString:~1,-1!
) ELSE (GOTO :EOF)
) ELSE (GOTO :EOF)
SET !_DeQuoteVar!=!_DeQuoteString!
SET _DeQuoteVar=
SET _DeQuoteString=
GOTO :EOF

:FILEEND