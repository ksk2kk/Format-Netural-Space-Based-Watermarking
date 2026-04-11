@echo off
setlocal

echo [INFO] Setting up Visual Studio environment...
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

if not exist bin (
    mkdir bin
)

echo [INFO] Compiling advanced_main.cpp...
cl /EHsc /utf-8 /std:c++17 src\advanced_main.cpp shell32.lib /Fo:bin\ /Fe:bin\advanced_main.exe

if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] Build completed successfully. Executable is in bin\advanced_main.exe
) else (
    echo [ERROR] Build failed.
)
