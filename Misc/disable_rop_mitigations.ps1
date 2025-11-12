# powershell.exe -f disable_rop_mitigations.ps1 blobrunner64.exe
$processname = $args[0]
Set-ProcessMitigation -Name $processname -Disable EnableRopStackPivot,AuditEnableRopStackPivot,EnableRopCallerCheck,AuditEnableRopCallerCheck,EnableRopSimExec,AuditEnableRopSimExec
Write-Output "[+] SimExec (Simulate execution), CallerCheck (Validate API invocation), StackPivot (Validate stack integrity) exploit mitigations disabled!"