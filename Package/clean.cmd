@echo off
setlocal

REM Clean up
if exist .\Win32 rmdir /S /Q .\Win32
if exist .\x64 rmdir /S /Q .\x64
if exist .\Release rmdir /S /Q .\Release
if exist ..\MediaCaptureWPF\bin rmdir /S /Q ..\MediaCaptureWPF\bin
if exist ..\MediaCaptureWPF\obj rmdir /S /Q ..\MediaCaptureWPF\obj
if exist ..\Debug rmdir /S /Q ..\Debug
if exist ..\x64 rmdir /S /Q ..\x64
if exist ..\Release rmdir /S /Q ..\Release
if exist ..\MediaCaptureWPF.Native\Debug rmdir /S /Q ..\MediaCaptureWPF.Native\Debug
if exist ..\MediaCaptureWPF.Native\Release rmdir /S /Q ..\MediaCaptureWPF.Native\Release
if exist ..\MediaCaptureWPF.Native\x64 rmdir /S /Q ..\MediaCaptureWPF.Native\x64
if exist ..\MediaSink\Debug rmdir /S /Q ..\MediaSink\Debug
if exist ..\MediaSink\Release rmdir /S /Q ..\MediaSink\Release
if exist ..\MediaSink\x64 rmdir /S /Q ..\MediaSink\x64
