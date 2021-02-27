setlocal
del %windir%\System32\drivers\HelloKernel.sys
copy i386\HelloKernel.sys %windir%\System32\drivers\HelloKernel.sys
sc.exe create HelloKernel binpath= %windir%\System32\drivers\HelloKernel.sys type= kernel start= demand error= normal DisplayName= HelloKernelBla
sc.exe start HelloKernel
sc.exe stop HelloKernel
sc.exe delete HelloKernel
endlocal
