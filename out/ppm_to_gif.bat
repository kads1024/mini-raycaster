@echo off
REM Convert all PPM files in current folder to animated GIF
REM Requires ImageMagick to be installed: https://imagemagick.org/script/download.php#windows

setlocal enabledelayedexpansion

REM Check if ImageMagick is installed
where magick >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: ImageMagick is not installed or not in PATH
    echo Download from: https://imagemagick.org/script/download.php#windows
    echo Make sure to check "Add ImageMagick to system PATH" during installation
    if not defined AUTO_MODE pause
    exit /b 1
)

REM Set output filename (you can change this)
set "output=output.gif"

REM Check if there are any PPM files
if not exist "*.ppm" (
    echo Error: No PPM files found in current directory
    if not defined AUTO_MODE pause
    exit /b 1
)

REM Count total PPM files, used for the progress percentage
set /a total=0
for %%f in (*.ppm) do set /a total+=1

echo Converting !total! PPM files to animated GIF...
echo Output: %output%
echo.

REM ---- progress display: backspace over the old text, then reprint ----
set "BS="

set /a count=0
for %%f in (*.ppm) do (
    set /a count+=1
    set /a percent=!count!*100/!total!
    <nul set /p "=!BS!Progress: !percent!%%   "
)
echo.
echo Encoding GIF, this may take a moment...

REM Convert all PPM files to animated GIF
REM Sorting alphabetically by using a wildcard pattern
magick *.ppm -delay 10 -loop 0 "%output%"

if %errorlevel% equ 0 (
    echo.
    echo Success! Created: %output%
    echo Delay between frames: 10 centiseconds
) else (
    echo Error during conversion
    if not defined AUTO_MODE pause
    exit /b 1
)

if not defined AUTO_MODE pause