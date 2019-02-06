del xml\*.* /Q
del dokuwiki\*.* /Q
"C:\Program Files\doxygen\bin\doxygen.exe" config\Doxygen\Doxyfile
"X:\dokugen\bin\msvc2017_64\Release\dokugen.exe" xml ref -r classscone_1_1_ -r structscone_1_1_