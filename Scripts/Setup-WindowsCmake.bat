@echo off

REM Check if CMake is installed
cmake --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed. Please install CMake to continue.
    pause
    exit /b 1
)

REM Set variables
set "BUILD_DIR=..\Build"

REM Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

REM Run CMake to generate the project files
pushd "%BUILD_DIR%"
cmake -G "Visual Studio 17 2022" ..
popd

pause
