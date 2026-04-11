@echo off
echo Cleaning up previous Geth instances...
taskkill /F /IM geth.exe > nul 2>&1

echo Starting Geth (dev mode, custom IPC)...
REM Run geth in background. 
start /B geth --dev --ipcpath \\.\pipe\geth_custom.ipc --verbosity 4 > geth_huge.log 2>&1

echo Waiting for Geth to initialize (5 seconds)...
timeout /t 5 /nobreak > nul

echo Attaching to Geth and starting transaction generator...
REM Use 'type' to pipe script to geth attach
type tx_gen.js | geth attach \\.\pipe\geth_custom.ipc
