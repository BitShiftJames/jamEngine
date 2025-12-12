@echo off
setlocal EnableDelayedExpansion

del /Q "..\jamScenes\compiled_scenes\*"

REM Generate timestamp: YYYYMMDD_HHMMSS
set TS=%date:~-4%%date:~4,2%%date:~7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set TS=!TS: =0!

echo Timestamp = "!TS!"

cmake .. -DBUILD_ID=!TS!

REM Build

msbuild Lumaria.sln
