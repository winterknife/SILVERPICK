# powershell.exe -f disable_iaf.ps1 blobrunner64.exe
$processname = $args[0]
Set-ProcessMitigation -Name $processname -Disable EnableImportAddressFilter,AuditEnableImportAddressFilter
Write-Output "[+] Import Address Filtering (IAF) exploit mitigation disabled!"