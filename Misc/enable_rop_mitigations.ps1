# powershell.exe -f enable_rop_mitigations.ps1 blobrunner64.exe
$processname = $args[0]
Set-ProcessMitigation -Name $processname -Enable EnableRopStackPivot,EnableRopCallerCheck,EnableRopSimExec
Write-Output "[+] SimExec (Simulate execution), CallerCheck (Validate API invocation), StackPivot (Validate stack integrity) exploit mitigations enabled!"