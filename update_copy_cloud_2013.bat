set cloudtarget=%~dp0..\..\My Documents\OneDrive
set dropbox=%~dp0..\..\My Documents\DropBox\Development

xcopy "%~dp0\bin\v120-Release\*" "%cloudtarget%\Scone\bin\Release\*" /S /Y
xcopy "%~dp0\config\*" "%cloudtarget%\Scone\config\*" /S /Y
xcopy "%~dp0\models\*" "%cloudtarget%\Scone\models\*" /S /Y
xcopy "%~dp0.version" "%cloudtarget%\Scone" /S /Y

xcopy "%~dp0\bin\v120-Release\*" "%dropbox%\Scone\bin\Release\*" /S /Y
xcopy "%~dp0\config\*" "%dropbox%\Scone\config\*" /S /Y
xcopy "%~dp0\models\*" "%dropbox%\Scone\models\*" /S /Y
xcopy "%~dp0.version" "%dropbox%\Scone" /S /Y
