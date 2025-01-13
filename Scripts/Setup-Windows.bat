@echo off

REM Check if Python is installed
python --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Python is not installed. Please install Python to continue.
    pause
    exit /b 1
)

REM Run Premake to generate the project files
pushd ..
Vendor\Binaries\Premake\Windows\premake5.exe --file=Build.lua vs2022
popd
pause
