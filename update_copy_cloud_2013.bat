set cloudtarget=%~dp0..\..\My Documents\OneDrive

xcopy "%~dp0\bin\v120-Release\*" "%cloudtarget%\Scone\bin\Release\*" /S /Y
xcopy "%~dp0\config\*" "%cloudtarget%\Scone\config\*" /S /Y
xcopy "%~dp0\models\*" "%cloudtarget%\Scone\models\*" /S /Y
xcopy "%~dp0.version" "%cloudtarget%\Scone" /S /Y
