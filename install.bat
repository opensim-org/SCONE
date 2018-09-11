set target_folder=%Dropbox%\Development\scone
call %~dp0.updateversion.bat
xcopy "%~dp0bin\msvc2015_64\Release\*" "%target_folder%\bin\msvc2015_64\Release\*" /S /D /Y
xcopy "%~dp0bin\msvc2017_64\Release\*" "%target_folder%\bin\msvc2017_64\Release\*" /S /D /Y
xcopy "%~dp0tutorials\*" "%target_folder%\scenarios\tutorials\*" /S /D /Y
xcopy "%~dp0resources\*" "%target_folder%\resources\*" /S /D /Y
xcopy "%~dp0.version" "%target_folder%" /D /Y
