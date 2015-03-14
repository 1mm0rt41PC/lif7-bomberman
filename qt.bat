@echo off
rem
rem This file is generated
rem
echo Setting up a MinGW/Qt only environment...
echo -- QTDIR set to c:\Qt\2009.01\qt
echo -- PATH set to c:\Qt\2009.01\qt\bin
echo -- Adding c:\Qt\2009.01\bin to PATH
echo -- Adding %SystemRoot%\System32 to PATH
echo -- QMAKESPEC set to win32-g++
set QTDIR=C:\Qt\qt
set PATH=C:\Qt\qt\bin;%PATH%
set PATH=%PATH%;%SystemRoot%\System32;C:\shell;C:\shell\compilo\bin
set QMAKESPEC=win32-g++
prompt :\\$G

start console.exe
exit