# powershell.exe -f disable_eaf.ps1 blobrunner64.exe
$processname = $args[0]
Set-ProcessMitigation -Name $processname -Disable EnableExportAddressFilter,EnableExportAddressFilterPlus,AuditEnableExportAddressFilter,AuditEnableExportAddressFilterPlus
Write-Output "[+] Export Address Filtering (EAF), Export Address Filtering Plus (EAF+) exploit mitigations disabled!"