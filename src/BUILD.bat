@echo off
echo.
echo ========================================
echo   BUILDING UNTURNED OFFLINE DEV TOOL
echo ========================================
echo.

cl /EHsc /std:c++17 src\main.cpp /link user32.lib kernel32.lib gdi32.lib gdiplus.lib

if %errorlevel% == 0 (
    echo.
    echo SUCCESS! UnturnedDevTool.exe is ready.
    echo Run in Singleplayer/LAN as Admin.
    echo.
) else (
    echo.
    echo FAILED. Install Visual Studio 2022.
    echo.
)
pause
