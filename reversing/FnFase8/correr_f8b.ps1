param([string]$Et, [int]$Lim = 30, [string]$Extra = '', [int]$Cap = 15)
Set-Location $PSScriptRoot
$procCap = Start-Process pwsh -ArgumentList '-NoProfile','-File',"$PSScriptRoot\captura_f8b.ps1",'-Salida',"$PSScriptRoot\ejecuciones\$Et-captura.png",'-Espera',$(if ($Cap) { $Cap } else { 15 }) -PassThru -WindowStyle Hidden
pwsh -NoProfile -File ejecutar_f8b.ps1 -Etiqueta $Et -Limite $Lim -Extra $Extra 2>&1 | Select-Object -Last 2 | Out-String
$r = (Get-ChildItem ejecuciones -Directory -Filter "$Et-*" | Select-Object -Last 1).FullName
Get-Content "$r\fase8_arranque.log" | Select-String 'CODIGO ORIGINAL|FALLO|estado:|GUARDIA|ventana' | Select-Object -Last 8 | ForEach-Object { $_.Line.Substring(0,[Math]::Min(260,$_.Line.Length)) }
Get-Content "$r\log.log" | Select-String -NotMatch 'app_debug|Backtrace|timecyc' | Select-Object -Last 14 | ForEach-Object { "$_".Substring(0,[Math]::Min(200,"$_".Length)) }
