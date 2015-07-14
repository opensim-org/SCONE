set cloudtarget=%~dp0..\..\My Documents\OneDrive

xcopy "%~dp0\bin\Release\*" "%cloudtarget%\Scone\bin\Release\*" /S /D /Y
xcopy "%~dp0\config\*" "%cloudtarget%\Scone\config\*" /S /D /Y
xcopy "%~dp0\models\*" "%cloudtarget%\Scone\models\*" /S /D /Y
