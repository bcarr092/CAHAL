Synopsis
========
This is the Cross-Platform Audio Hardware Abstraction Library (CAHAL) library. This library supports audio recording and playback on Windows, Mac OS X, iOS, and Android. Moreover, this project can be built for the x86, x64, ARM and ARM64 architectures as well through the use of the [CPCommon](https://github.com/bcarr092/CPCommon) project.

The goal of this project is to provide a cross-platform abstraction layer which allows access to the underlying platform-specific audio library APIs so that application-level developers who are using this library do not have to worry about writing platform-specific audio handling code. This design handles both recording and playback through callbacks and, therefore, performs both functions in asynchronous mode.

Motivation
==========
This is an alternative solution to the PortAudio library and was developed specifically to support mobile platforms (e.g., iOS and Android) as well as the desktop platforms (e.g., Windows and Mac OS X).

Dependencies
============

This project requires the following GitHub projects:
* [CPCommon](https://github.com/bcarr092/CPCommon)
* [DarwinHelper](https://github.com/bcarr092) (If you are building on Mac OS X)

To use an application that uses this library see [pyCovertAudio](https://github.com/bcarr092/pyCovertAudio).

Contributors
============
Brent Carrara (bcarr092@uottawa.ca)

License
=======

   Copyright 2016 Brent Carrara 

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
