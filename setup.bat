@echo off

REM Go to the external folder
cd %~dp0/external

REM Install nuget if it doesn't exist
if not exist nuget (
    mkdir nuget
    cd nuget
    curl -L -o nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
    cd ..
)

REM Remove the old WebView2 folder
rmdir /s /q webview2

REM Use nuget to download WebView2 API, and wait for it to finish
start "" "./nuget/nuget.exe" "install" "Microsoft.Web.WebView2"

REM Check if the process is still running
:LOOP
tasklist | find /i "nuget.exe" >nul
if errorlevel 1 (
    goto CONTINUE
) else (
    timeout /t 1 /nobreak >nul
    goto LOOP
)

:CONTINUE

REM Get the folder name that starts with "Microsoft.Web.WebView2"
for /d %%i in (Microsoft.Web.WebView2*) do set "folder=%%i"

REM Change the folder name to "webview2"
ren "%folder%" "webview2"