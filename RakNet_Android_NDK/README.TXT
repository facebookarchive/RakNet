Android project for Oculus RakNet library.
Initial project version: Viktor Korsun, bitekas@gmail.com

Copyright (c) 2015, Oculus VR, Inc.
All rights reserved.

This source code is licensed under the BSD-style license found in the
LICENSE file in the root directory of this source tree. An additional grant 
of patent rights can be found in the PATENTS file in the same directory.
---------------------------------------------------------------------------

To build Android native libraries run "ndk-build" from THIS directory, or
merge the content with your project.

Windows build command example: 
D:\Soft\android-ndk-r10e-64\bin\ndk-build

Linux/OS X build command example: 
$ ~/soft/android-ndk-r10e-32/ndk-build 

This will produce "libs" folder with shared libraries for platforms:
    armeabi
    armeabi-v7a
    x86

The project uses android-9 API, that covers almost 100% of Android devices
by the end of 2015. To raise API (and possibly build additional ABIs)
please edit jni/Application.mk.

The project has been tested with Android NDK 10e both 32- and 64- bit 
versions.