set DIST_ARTIFACT_NAME=hdimount-win10x64

for /f %%i in ('go env GOPATH') do set GOPATH=%%i
set "PATH=%PATH%;%GOPATH%\bin"
	    
go get github.com/gableroux/github-release

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

echo End of prebuild