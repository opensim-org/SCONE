set target_folder=%USERPROFILE%\Dropbox\Development\scone

xcopy "%~dp0\bin\msvc2015_64\Release\*" "%target_folder%\bin\msvc2015_64\Release\*" /S /D /Y
xcopy "%~dp0\scenarios\*" "%target_folder%\scenarios\*" /S /Y
xcopy "%~dp0\models\*" "%target_folder%\models\*" /S /Y
xcopy "%~dp0\resources\*" "%target_folder%\resources\*" /S /Y
xcopy "%~dp0\*" "%target_folder%\*" /S /Y
