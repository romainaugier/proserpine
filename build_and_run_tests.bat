@echo off

rem SPDX-License-Identifier: BSD-3-Clause
rem Copyright (c) 2026 - Present Romain Augier MIT License
rem All rights reserved

set CONFIG=RelWithDebInfo
set RMBUILD=0
set BUILDDIR=build

for %%x in (%*) do (
    call :ParseArg %%~x
)

echo Looking for Renderdoc to append in PATH

set RENDERDOC_INSTALL_DIR=%PROGRAMFILES%\RenderDoc

if exist %RENDERDOC_INSTALL_DIR% (
    echo Found RenderDoc installation dir: %RENDERDOC_INSTALL_DIR%

    echo.!PATH! | findstr /C:"%RENDERDOC_INSTALL_DIR%" 1>nul

    if %ERRORLEVEL% equ 1 (
        echo Appending RenderDoc installation dir to PATH
        set "PATH=%PATH%;%RENDERDOC_INSTALL_DIR%"
    )
)

if %RMBUILD% equ 1 (
    if exist %BUILDDIR% (
        echo Removing old build directory
        rmdir /s /q %BUILDDIR%
    )
)

cmake -S . -B %BUILDDIR%

if %ERRORLEVEL% neq 0 (
    echo Error during CMake configuration
    exit /B 1
)

cd %BUILDDIR%

cmake --build . --config %CONFIG%

if %ERRORLEVEL% neq 0 (
    cd ..
    echo Error during CMake build
    exit /B 1
)

ctest --output-on-failure -C %CONFIG%

if %ERRORLEVEL% neq 0 (
    cd ..
    echo Error during CMake test
    exit /B 1
)

echo \n
echo Build and test successful!

cd ..

exit /B 0

rem ////////////////////////////////////////////////////////////////////////////
:ParseArg

if "%~1" equ "clean" set RMBUILD=1

exit /B 0
rem ////////////////////////////////////////////////////////////////////////////
