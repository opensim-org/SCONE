set target_folder=%USERPROFILE%\Dropbox\Development\scone

xcopy "%~dp0bin\msvc2015_64\Release\*" "%target_folder%\bin\msvc2015_64\Release\*" /S /D /Y
xcopy "%~dp0scenarios\*" "%target_folder%\scenarios\*" /S /Y
xcopy "%~dp0models\*" "%target_folder%\models\*" /S /Y
xcopy "%~dp0resources\*" "%target_folder%\resources\*" /S /Y
xcopy "%~dp0.version" "%target_folder%" /Y
