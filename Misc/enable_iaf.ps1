# powershell.exe -f enable_iaf.ps1 blobrunner64.exe
$processname = $args[0]
Set-ProcessMitigation -Name $processname -Enable EnableImportAddressFilter
Write-Output "[+] Import Address Filtering (IAF) exploit mitigation enabled!"