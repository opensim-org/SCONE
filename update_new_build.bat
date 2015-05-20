# Build
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
msbuild "%~dp0\build\vc2010\scone.sln" /p:Configuration=Release

# Export
xcopy "%~dp0\bin\Release\*" "%~dp0\bin\ReleaseCopy\*" /S /D /Y

# Export cloud
xcopy "%~dp0\bin\Release\*" "%~dp0\..\..\OneDrive\Scone\bin\Release\*" /S /D /Y
xcopy "%~dp0\config\*" "%~dp0\..\..\OneDrive\Scone\config\*" /S /D /Y
xcopy "%~dp0\models\*" "%~dp0\..\..\OneDrive\Scone\models\*" /S /D /Y
