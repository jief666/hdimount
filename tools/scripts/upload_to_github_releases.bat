    
    for /f "delims=" %%x in (%~dp0..\..\version.txt) do set VERSION=%%x
    for /f "delims=" %%x in ('git rev-parse --short  HEAD') do set SHORTSHA=%%x
    
rem    for /f %%i in ('go env GOPATH') do set GOPATH=%%i
rem set DIST_ARTIFACT_NAME=hdimount-win10x64

    if exist "%~dp0..\..\dist\%DIST_ARTIFACT_NAME%" (
        echo pushing "%~dp0..\..\dist\%DIST_ARTIFACT_NAME%" to github release under name "%VERSION%__%SHORTSHA%" for branch "%APPVEYOR_REPO_BRANCH%"
 
        %GOPATH%\bin\github-release release  --user "%GITHUB_USER%"  --repo "%GITHUB_REPO%"  --target "%APPVEYOR_REPO_BRANCH%"  --tag "%VERSION%__%SHORTSHA%"  --pre-release
        %GOPATH%\bin\github-release upload   --user "%GITHUB_USER%"  --repo "%GITHUB_REPO%"  --tag "%VERSION%__%SHORTSHA%"  --name "%DIST_ARTIFACT_NAME%"  --file "%~dp0..\..\dist\%DIST_ARTIFACT_NAME%" --replace

        if errorlevel 1 (
           %GOPATH%\bin\github-release upload   --user "%GITHUB_USER%"  --repo "%GITHUB_REPO%"  --tag "%VERSION%__%SHORTSHA%"  --name "%DIST_ARTIFACT_NAME%"  --file "%~dp0..\..\dist\%DIST_ARTIFACT_NAME%"  --replace
        )
    ) else (
        echo "%~dp0..\..\dist\%DIST_ARTIFACT_NAME%" doesn\'t exist, cannot upload
    )

