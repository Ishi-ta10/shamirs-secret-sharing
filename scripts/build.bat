@echo off
echo 🔨 Building Catalog Placements Assignment...
echo.

REM Check if g++ is available
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ Error: g++ compiler not found!
    echo Please install MinGW-w64 or Visual Studio Build Tools
    echo Download from: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

REM Compile the program
echo Compiling...
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude -Isrc -o catalog_assignment.exe src/main.cpp

if %errorlevel% equ 0 (
    echo ✅ Compilation successful!
    echo.
    echo 🚀 Running assignment...
    catalog_assignment.exe
) else (
    echo ❌ Compilation failed!
    pause
    exit /b 1
)

echo.
pause
