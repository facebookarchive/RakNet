RakNet 4.081
============

Copyright (c) 2014, Oculus VR, Inc.

Package notes
------------------------------------------
The Help directory contains index.html, which is full help documentation in HTML format
The Source directory contain all files required for the core of Raknet and is used if you want to use the source in your program or create your own dll
The Samples directory contains code samples and one game using an older version of Raknet.  The code samples each demonstrate one feature of Raknet.  The game samples cover several features.
The lib directory contains libs for debug and release versions of RakNet and RakVoice
There is a make file for linux users in the root directory.  Windows users can use projects under Samples\Project Samples

C# support
------------------------------------------

See Help\swigtutorial.html

Upgrading from version 3
------------------------------------------

See 3.x_to_4.x_upgrade.txt

### Building Static Libraries for C++

1. Open the solution file, RakNet_vc8.sln
2. Right click "LibStatic"
3. Click build.
4. The binary will be under the /Lib directory.
If you want to use source (recommended), just include all header and cpp files under /Source
See Help/compilersetup.html

Windows users (Visual Studio 2008 and 2010)
-----------------------------------------
Load RakNet_VS2008.sln and convert if necessary.  

After the project conversion, if you encounter error MSB4006,
follow the steps below to fix it:

1. Open project properties
2. Click on "Common Properties"
3. Click on "Framework and References"
4. Look under "Build Properties"
5. Change the "Copy Local" flag from "True" to "False"

For other compiler issues, please see Help/compilersetup.html

Windows users (Visual Studio 2005)
-----------------------------------------
Load RakNet_VS2005.sln

Windows users (.NET 2003)
-----------------------------------------
Load RakNet_VS2003.sln

Windows users (VC6)
-----------------------------------------
Not supported, but you can get by if you directly include the source.

CYGWIN users
-----------------------------------------
Copy Include, Source, and whatever you want to run in the home directory.  Then type
`g++ ../../lib/w32api/libws2_32.a *.cpp`
You can run `a.exe`.
You might have to copy `*.dll` from `cygwin\bin` as well.

Linux users
-----------------------------------------
Use `cmake`, or `g++ -lpthread -g *.cpp` in the /Source directory.
With libcat, use `g++ -pthread -g -I./../DependentExtensions *.cpp` in the /Source directory.

64 bit use -m64 command line
Sometimes you need -pthread instead of -lpthread

Command to build 64 bit chat example server from the /Source directory:

    g++ -m64 -g -lpthread -I./ "../Samples/Chat Example/Chat Example Server.cpp" *.cpp

Command to build NATCompleteServer from the Samples/NATCompleteServer directory:

    g++ -g -lpthread -I./ -I./../../Source main.cpp -I./../CloudServer ./../../Source/*.cpp ./../CloudServer/CloudServerHelper.cpp

Command to build autopatcher server from /Source directory:

    g++ -lpthread -lpq -lssl -lbz2 -lssl -lcrypto -L/opt/PostgreSQL/9.0/lib -L../DependentExtensions/bzip2-1.0.6 -I/opt/PostgreSQL/9.0/include -I../DependentExtensions/bzip2-1.0.6 -I./ -I../DependentExtensions/Autopatcher -I../DependentExtensions/Autopatcher/AutopatcherPostgreRepository -I../DependentExtensions/PostgreSQLInterface -g *.cpp ../DependentExtensions/Autopatcher/AutopatcherServer.cpp ../DependentExtensions/Autopatcher/CreatePatch.cpp ../DependentExtensions/Autopatcher/MemoryCompressor.cpp ../DependentExtensions/Autopatcher/AutopatcherPostgreRepository/AutopatcherPostgreRepository.cpp ../DependentExtensions/PostgreSQLInterface/PostgreSQLInterface.cpp ../Samples/AutopatcherServer/AutopatcherServerTest.cpp

Command to build NATCompleteServer from /Source directory:

    g++ -lpthread -I./ -I../Samples/CloudServer ../Samples/CloudServer/CloudServerHelper.cpp ../Samples/NATCompleteServer/main.cpp *.cpp

Command to build BigPacketTest from /Source directory:

    g++ -lpthread -I./ ../Samples/BigPacketTest/BigPacketTest.cpp *.cpp

Or with debugging info on:

    g++ -g -lpthread -I./ ../Samples/BigPacketTest/BigPacketTest.cpp *.cpp

If you get /usr/local/lib/libraknet.so: undefined reference to \`__sync_fetch_and_add_4 then build with `-march=i686`

To debug:
http://www.unknownroad.com/rtfm/gdbtut/gdbstack.html
http://cs.baylor.edu/~donahoo/tools/gdb/tutorial.html
http://linux.bytesex.org/gdb.html
http://www.delorie.com/gnu/docs/gdb/gdb_29.html

    gdb ./a.out

Set breakpoint:

    b file:line

Disable a breakpoint:

    disable <breakpointNumber>

Delete a breakpoint:

    delete <breakpointNumber>

Get a list of breakpoints:

    info breakpoints

St breakpoint to be ignored that number of times

    ignore <breakpointNumber> <count>
    run

Other useful commands:

    info stack
    info locals
    delete (Clears all breakpoints)
    step (step into)
    next (step over)
    finish (step out)
    continue to keep going after step or next
    p <variableName>
    For example: p users.orderedList.listArray[0].guid

Command to install g++

    sudo apt-get install gcc-c++
    sudo apt-get install build-essential
Or:

    yum install gcc-c++
Or:

    sudo apt-get update
    sudo apt-get install g++

Command to install gdb

    sudo apt-get install gdb

Command to install wget, used to download files from webpages
sudo apt-get install wget

Series of commands for a new server:
    sudo apt-get install wget
    sudo apt-get update
    sudo apt-get install --fix-missing g++
    sudo apt-get install gdb
    cd RakNet_Install_Directory\Source
    g++ -m64 -g -pthread -I./ "../Samples/Chat Example/Chat Example Server.cpp" *.cpp
    ./a.out

DevCPP Users
-----------------------------------------
Load RakNet.dev

CodeBlocks Users
-----------------------------------------
Load RakNet.cbp

Mac Users
-----------------------------------------
Open a Terminal window and type:

    cd ~/Desktop/RakNet/Source
    g++ -c -DNDEBUG -I -isysroot /Developer/SDKs/MacOSX10.5u.sdk/ -arch i386 *.cpp

Use whichever SDK you have. However, the 10.4 SDK is bugged and will not compile unless you use GCC 4.0 from inside XCODE

The sources should build cleanly. This gives you a bunch of PowerPC binaries, compiled against the 10.3.9 SDK which is a good thing.

Give the following command:

    libtool -static -o raknetppc.a *.o

This will stitch together a static library for the PowerPC architecture. There may be warnings that some .o files do not have any symbols. If you want to be prudent, remove the named files (the .o files, not the .cpp files!) and re-run the libtool command.

Now, we build the source files for Intel:

    gcc -c -I ../Include -isysroot /Developer/SDKs/MacOSX10.4u.sdk/ -arch i386 *.cpp

..and stitch it into a i386 library:

    libtool -static -o rakneti386.a *.o

Now, type:

    ls *.a

which should list the two .a files. Now, we make them into a universal binary:

    lipo -create *.a -o libraknet.a

You now have a file named libraknet.a. This is the RakNet library, built to run on both PowerPC and Intel Macs. Enjoy! ;-)

IPod
-----------------------------------------
Depending on what version you target, you may have to change two defines to not use 64 bit integers and floats or doubles.

Android
-----------------------------------------

You will need the latest CYWGIN and the android SDK to build native code on the android. Under CYWGIN, you will need to run ndk-build on a directory for RakNet.

1. Under cygwin, create the RakNet directory somewhere, such as under samples.
For example, if you create the path `\cygwin\home\Kevin\android-ndk-r4b\samples\RakNet`

2. I copied the Android.Manifest.xml and other files from another sample

3. Under jni, you will need the following Android.mk

    LOCAL_PATH := $(call my-dir)
    include $(CLEAR_VARS)
    LOCAL_MODULE    := RakNet
    MY_PREFIX := $(LOCAL_PATH)/RakNetSources/
    MY_SOURCES := $(wildcard $(MY_PREFIX)*.cpp)
    LOCAL_SRC_FILES += $(MY_SOURCES:$(MY_PREFIX)%=RakNetSources/%)
    include $(BUILD_SHARED_LIBRARY)

This version of Android.mk assumes there is a directory called RakNetSources, for example
`cygwin/home/Kevin/android-ndk-r4b/samples/RakNet/jni/RakNetSources`

Under RakNetSources should be the /Source directory to RakNet. Rather than copy the files I used junction.exe
http://technet.microsoft.com/en-us/sysinternals/bb896768.aspx

The command I used to create the junction was:

    D:/cygwin/home/Kevin/android-ndk-r4b/samples/RakNet/jni/junction.exe -s D:/cygwin/home/Kevin/android-ndk-r4b/samples/RakNet/jni/RakNetSources D:/RakNet4/Source

To unjunction I used:

    D:/cygwin/home/Kevin/android-ndk-r4b/samples/RakNet/jni/junction.exe -d D:/cygwin/home/Kevin/android-ndk-r4b/samples/RakNet/jni/RakNetSources

From within the CYWGIN enviroment, navigate to home/Kevin/android-ndk-r4b/samples/RakNet. Then type

    ../../ndk-build

Everything should build and you should end up with a .so file.

You should then be able to create a project in eclipse, and import cygwin/home/Kevin/android-ndk-r4b/samples/RakNet

Native client
-----------------------------------------
Solution: RakNet_NativeClient_VS2010. See Samples\nacl_sdk\RakNet_NativeClient_VS2010\HowToSetup.txt for detailed instructions on setup.

Windows Phone 8
-----------------------------------------
Example solution: RakNet_WinPhone8_VS2012.
Add to your project DependentExtensions\WinPhone8\ThreadEmulation.cpp
Add DependentExtensions\WinPhone8\ to your include paths
Define _CRT_SECURE_NO_WARNINGS and WINDOWS_PHONE_8

Windows Store 8
-----------------------------------------
Example solution: RakNet_WindowsStore8_VS2012.sln
Add to your project DependentExtensions\WinPhone8\ThreadEmulation.cpp
Add DependentExtensions\WinPhone8\ and DependentExtensions\WinRT to your include paths
TCP is not supported, only UDP (RakPeer).
IPV4 only (not hard to also add IPV6 upon request).
Define:
_CRT_SECURE_NO_WARNINGS
WINDOWS_STORE_RT
_RAKNET_SUPPORT_TCPInterface=0
_RAKNET_SUPPORT_PacketizedTCP=0
_RAKNET_SUPPORT_EmailSender=0
_RAKNET_SUPPORT_HTTPConnection=0
_RAKNET_SUPPORT_HTTPConnection2=0
_RAKNET_SUPPORT_TelnetTransport=0
_RAKNET_SUPPORT_NatTypeDetectionServer=0
_RAKNET_SUPPORT_UDPProxyServer=0
_RAKNET_SUPPORT_UDPProxyCoordinator=0
_RAKNET_SUPPORT_UDPForwarder=0


Unreal engine
-----------------------------------------
See https://udn.epicgames.com/lists/showpost.php?list=unprog3&id=37697&lessthan=&show=20


