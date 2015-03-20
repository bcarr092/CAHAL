Synopsis
========
This is the Cross-Platform Audio Hardware Abstraction Library (CAHAL) library that supports audio functionality (i.e. record and playback) on Windows, Mac OS X, iOS, and Android. Furthermore, this project also supports x86, x64, ARM and ARM64 as well through the use of the CPCommon project.

The goal of this project is to provide a cross-platform abstraction layer that allows access to the platform-specific audio library APIs without having to write platform-specific code. The architecture behind this library is very simple: setup record or playback callbacks and register them with the library. The library will then start the appropriate function and call you back when data is available (record) or data is required (playback).

Note that currently Windows has not been implmented, which is a major TODO.

Code Example
============

Motivation
==========
This is an alternative solution to the PortAudio library and was developed to specifically support mobile platforms (e.g. iOS and Android) in addition to the desktop platforms (e.g. Windows and Mac OS X).

Installation
============
Cmake is required to build this project and furthermore the following other libraries are required as well:

- cpcommon
- darwinhelper

API Reference
=============
Using Cmake Doxygen style documentation can be generate by executing the 'make doc' command.

Tests
=====
Full API tests are available on all platforms and are supported using Python and the unittest framework. To execute tests run the 'make test' command in the build directory.

Contributors
============
Brent Carrara (bcarr092@uottawa.ca)

License
=======
A licensing model needs to be established for this library.
