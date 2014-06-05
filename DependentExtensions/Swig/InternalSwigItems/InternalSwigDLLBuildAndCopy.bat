@echo off
if "%1"=="" goto :SKIPSETPATH
set swigPath=%1
if "%swigPath:~-1%" NEQ "\" goto :SKIPREMOVESLASH
set swigPath=%swigPath:~0,-1%
:SKIPREMOVESLASH
set PATH=%PATH%;%swigPath%
:SKIPSETPATH
if "%VS80COMNTOOLS%"=="" goto :DONTUSEVS8
set toolDir=%VS80COMNTOOLS%
goto :ENDFINDTOOLS
:DONTUSEVS8
if "%VS90COMNTOOLS%"=="" goto :DONTUSEVS9
set toolDir=%VS90COMNTOOLS%
goto :ENDFINDTOOLS
:DONTUSEVS9
:ENDFINDTOOLS
call "%toolDir%\vsvars32.bat"
"%DevEnvDir%\devenv" /rebuild Debug "../DLL_Swig/RakNet.sln" /project RakNet
call CopyFilesToInternalProject.bat Debug
"%DevEnvDir%\devenv" /rebuild Release "../DLL_Swig/RakNet.sln" /project RakNet

call CopyFilesToInternalProject.bat Release