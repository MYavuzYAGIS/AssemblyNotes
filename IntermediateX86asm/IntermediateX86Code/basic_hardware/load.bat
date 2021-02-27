setlocal
del %windir%\System32\drivers\BASIC_HARDWARE.sys
copy i386\BASIC_HARDWARE.sys %windir%\System32\drivers\BASIC_HARDWARE.sys
sc.exe create BASIC_HARDWARE binpath= %windir%\System32\drivers\BASIC_HARDWARE.sys type= kernel start= demand error= normal DisplayName= BASIC_HARDWAREBla
sc.exe start BASIC_HARDWARE
endlocal
