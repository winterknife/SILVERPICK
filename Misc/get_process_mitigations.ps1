# https://learn.microsoft.com/en-us/defender-endpoint/exploit-protection
# powershell.exe -f get_process_mitigations.ps1 blobrunner64.exe
$processname = $args[0]
Get-ProcessMitigation -Name $processname