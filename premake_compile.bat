@echo off

if not defined DevEnvDir (
    call "C:/Program Files/Microsoft Visual Studio/2019/Community/Common7/Tools/VsDevCmd.bat"
)
set solutionFile = "Shard3D.sln"
msbuild /t:Build /p:Configuration=%BTYPE% /p:Platform=%ATYPE_A% %solutionFile%

pause