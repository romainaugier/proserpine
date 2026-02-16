@echo off

rem SPDX-License-Identifier: BSD-3-Clause
rem Copyright (c) 2026 - Present Romain Augier MIT License
rem All rights reserved

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

if exist build (
    echo Removing old build directory
    rem rmdir /s /q build
)

cmake -S . -B build

if %ERRORLEVEL% neq 0 (
    echo Error during CMake configuration
    exit /B 1
)

cd build

cmake --build . --config RelWithDebInfo

if %ERRORLEVEL% neq 0 (
    cd ..
    echo Error during CMake build
    exit /B 1
)

ctest --output-on-failure -C RelWithDebInfo

if %ERRORLEVEL% neq 0 (
    cd ..
    echo Error during CMake test
    exit /B 1
)

echo \n
echo Build and test successful!

cd ..

exit /B 0
