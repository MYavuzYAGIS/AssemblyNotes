setlocal
del %windir%\System32\drivers\BreakOnThruToTheOtherSide.sys
copy i386\BreakOnThruToTheOtherSide.sys %windir%\System32\drivers\BreakOnThruToTheOtherSide.sys
sc.exe create BreakOnThruToTheOtherSide binpath= %windir%\System32\drivers\BreakOnThruToTheOtherSide.sys type= kernel start= demand error= normal DisplayName= BreakOnThruToTheOtherSideBla
sc.exe start BreakOnThruToTheOtherSide
endlocal
