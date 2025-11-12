# powershell.exe -f enable_eaf.ps1 blobrunner64.exe kernel32.dll,kernelbase.dll,ntdll.dll
$processname = $args[0]
#$modulenamelist = $args[1]
#Set-ProcessMitigation -Name $processname -Enable EnableExportAddressFilter,EnableExportAddressFilterPlus -EAFModules $modulenamelist
Set-ProcessMitigation -Name $processname -Enable EnableExportAddressFilter,EnableExportAddressFilterPlus
Write-Output "[+] Export Address Filtering (EAF), Export Address Filtering Plus (EAF+) exploit mitigations enabled!"