setlocal
del %windir%\System32\drivers\ParlorTrick.sys
copy i386\ParlorTrick.sys %windir%\System32\drivers\ParlorTrick.sys
sc.exe create ParlorTrick binpath= %windir%\System32\drivers\ParlorTrick.sys type= kernel start= demand error= normal DisplayName= ParlorTrickBla
sc.exe start ParlorTrick
sc.exe stop ParlorTrick
sc.exe delete ParlorTrick
endlocal
