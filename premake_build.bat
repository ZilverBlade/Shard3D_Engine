@echo off

echo 1. vs2017
echo 2. vs2019
echo 3. vs2022
echo 4. gmake

set /p answer=Choose your preferred build system: 
if %answer%==1 goto vs2017_opt
if %answer%==2 goto vs2019_opt
if %answer%==3 goto vs2022_opt
if %answer%==4 goto gmake_opt

:vs2017_opt
cls
premake5.exe vs2017
pause

:vs2019_opt
cls
premake5.exe vs2019
pause

:vs2022_opt
cls
premake5.exe vs2022
pause

:gmake_opt
cls
premake5.exe gmake
pause

