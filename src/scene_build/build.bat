@echo off
setlocal EnableDelayedExpansion

where msbuild >nul 2>nul
if errorlevel 1 (
    echo Loading Visual Studio environment...
    call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
) else (
    echo Visual Studio environment already loaded
)

del /Q "..\jamScenes\compiled_scenes\*"

REM Generate timestamp: YYYYMMDD_HHMMSS
set TS=%date:~-4%%date:~4,2%%date:~7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set TS=!TS: =0!

echo Timestamp = "!TS!"

cmake .. -DBUILD_ID=!TS!
if errorlevel 1 goto :error

REM Build

msbuild Lumaria.sln
if errorlevel 1 goto :error


echo BUILT
exit /b 0

:error
echo BUILD FAILED
pause
exit /b 1


