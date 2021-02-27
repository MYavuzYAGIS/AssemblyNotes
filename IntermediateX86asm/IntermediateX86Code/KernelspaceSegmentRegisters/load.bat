setlocal
del %windir%\System32\drivers\KernelspaceSegmentRegisters.sys
copy i386\KernelspaceSegmentRegisters.sys %windir%\System32\drivers\KernelspaceSegmentRegisters.sys
sc.exe create KernelspaceSegmentRegisters binpath= %windir%\System32\drivers\KernelspaceSegmentRegisters.sys type= kernel start= demand error= normal DisplayName= KernelspaceSegmentRegistersBla
sc.exe start KernelspaceSegmentRegisters
sc.exe stop KernelspaceSegmentRegisters
sc.exe delete KernelspaceSegmentRegisters
endlocal
