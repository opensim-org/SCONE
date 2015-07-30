hg log -r . --template "{rev}" > "%~dp0\.version"
hg log -r . --template "{rev}" > "%~dp0\bin\v100-Release\.version"
hg log -r . --template "{rev}" > "%~dp0\bin\v120-Release\.version"