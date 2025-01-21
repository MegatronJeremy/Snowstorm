@echo off
REM Change directory to the script's location
cd /d %~dp0

REM Run the Python script for setup and configuration
py Setup-Project.py

REM Pause to allow the user to view any output
pause
