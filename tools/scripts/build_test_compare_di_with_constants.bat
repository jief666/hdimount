msbuild "%~dp0..\..\msvc\hdimount.sln" /t:compare_di_with_constants /p:configuration=Release;Platform=x64 
