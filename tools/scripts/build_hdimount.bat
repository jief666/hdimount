msbuild "%~dp0..\..\msvc\hdimount.sln" /t:hdimount /p:configuration=Release;Platform=x64

@IF %ERRORLEVEL% NEQ 0 (
  exit /B %ERRORLEVEL%
)

@if exist "%~dp0..\..\dist" (
    rem nothing to do
) else (
    mkdir dist
)

@if exist "%~dp0..\..\msvc\x64\hdimount\Release\hdimount.exe" (
    echo copy "%~dp0..\..\msvc\x64\hdimount\Release\hdimount.exe" to dist\%DIST_ARTIFACT_NAME%
    copy "%~dp0..\..\msvc\x64\hdimount\Release\hdimount.exe" dist\%DIST_ARTIFACT_NAME%
) else (
    echo File '"%~dp0..\..\msvc\x64\hdimount\Release\hdimount.exe"' doesn't exist, copy to dist
    exit /B 1
)
