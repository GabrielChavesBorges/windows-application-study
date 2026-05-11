@echo off
setlocal
cd /d "%~dp0"

gcc -Wall -Wextra -std=c99 -o "app.exe" "main.c" -lgdi32 -luser32 -lkernel32
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)
echo Build OK: app.exe