@echo off
echo 🔨 Building Shamir's Secret Sharing Algorithm...
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

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Compile the program
echo Compiling...
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude -Isrc -o shamirs_secret.exe src/main.cpp

if %errorlevel% equ 0 (
    echo ✅ Compilation successful!
    echo.
    echo 🚀 Running with default input...
    shamirs_secret.exe examples/input.json
) else (
    echo ❌ Compilation failed!
    pause
    exit /b 1
)

echo.
echo 📝 Usage: shamirs_secret.exe [input_file.json] [options]
echo Options: --verbose, --time, --help
pause
