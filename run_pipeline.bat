@echo off
setlocal enabledelayedexpansion

REM ============================================================
REM  This script:
REM   1. Builds main.exe from main.cpp with clang++
REM   2. Runs the exe and waits for it to finish
REM   3. Runs ppm_to_gif.bat inside the ppm subfolder
REM   4. Moves the resulting gif up into this folder
REM
REM  EDIT THE LINES BELOW to match your setup:
REM ============================================================
set "SRC_NAME=main.cpp framebuffer.cpp map.cpp utils.cpp texture.cpp"
set "EXE_NAME=main.exe"
set "PPM_SUBDIR=out"
set "GIF_NAME=output.gif"
REM ============================================================

cd /d "%~dp0"

REM ---- Step 1: build ----
where clang++ >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: clang++ not found in PATH
    echo Install LLVM/clang and make sure clang++ is on PATH.
    pause
    exit /b 1
)

for %%f in (%SRC_NAME%) do (
    if not exist "%%f" (
        echo Error: "%%f" not found in %cd%
        echo Edit SRC_NAME at the top of run_pipeline.bat to match your source files.
        pause
        exit /b 1
    )
)

echo Building %EXE_NAME% from %SRC_NAME% ...
clang++ %SRC_NAME% -o "%EXE_NAME%"
if %errorlevel% neq 0 (
    echo.
    echo Error: build failed. Fix the errors above and re-run.
    pause
    exit /b 1
)
echo Build succeeded.
echo.

REM ---- Step 2: run the exe ----
if not exist "%EXE_NAME%" (
    echo Error: "%EXE_NAME%" not found in %cd% after build
    pause
    exit /b 1
)

echo Running %EXE_NAME% ...
"%EXE_NAME%"
if %errorlevel% neq 0 echo Warning: %EXE_NAME% exited with code %errorlevel%. Continuing anyway...
echo %EXE_NAME% finished.
echo.

if not exist "%PPM_SUBDIR%\" (
    echo Error: subfolder "%PPM_SUBDIR%" not found in %cd%
    echo Edit PPM_SUBDIR at the top of run_pipeline.bat to match your ppm folder's name.
    pause
    exit /b 1
)

set /a ppmcount=0
for %%f in ("%PPM_SUBDIR%\*.ppm") do set /a ppmcount+=1
echo Found !ppmcount! PPM files in %PPM_SUBDIR%.
if !ppmcount! neq 360 echo Warning: expected 360 PPM files but found !ppmcount!. Continuing anyway...
echo.

if not exist "%PPM_SUBDIR%\ppm_to_gif.bat" (
    echo Error: ppm_to_gif.bat not found inside "%PPM_SUBDIR%"
    pause
    exit /b 1
)

pushd "%PPM_SUBDIR%"
set "AUTO_MODE=1"
call ppm_to_gif.bat
popd

if not exist "%PPM_SUBDIR%\%GIF_NAME%" (
    echo Error: "%GIF_NAME%" was not created inside "%PPM_SUBDIR%"
    echo Check the ppm_to_gif.bat output above for details.
    pause
    exit /b 1
)

move /Y "%PPM_SUBDIR%\%GIF_NAME%" "%GIF_NAME%" >nul
echo.
echo Done. %GIF_NAME% is now in %cd%
pause