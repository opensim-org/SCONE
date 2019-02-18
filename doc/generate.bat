del xml\*.* /Q
del ref\*.* /Q
"C:\Program Files\doxygen\bin\doxygen.exe" Doxyfile
"..\..\dokugen\bin\msvc2017_64\Release\dokugen.exe" xml ref -r classscone_1_1_ -r structscone_1_1_