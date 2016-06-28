set target_folder=D:\tgeijtenbeek\DropBox\Development\Scone

xcopy "%~dp0\bin\msvc2013\Release\*" "%target_folder%\bin\Release\*" /S /D /Y
xcopy "%~dp0\config\*" "%target_folder%\config\*" /S /Y
xcopy "%~dp0\models\*" "%target_folder%\models\*" /S /Y
xcopy "%~dp0.version" "%target_folder%" /S /Y
