# Ejecuta un comando, guarda su salida en un log y, si falla, emite las ultimas 40 lineas
# como ::error:: (los registros de Actions exigen sesion; las anotaciones se ven en el resumen publico).
param(
    [Parameter(Mandatory = $true)][string]$Name,
    [Parameter(Mandatory = $true)][string]$Command
)
$ErrorActionPreference = 'Continue'
$log = Join-Path $env:RUNNER_TEMP "$Name.log"
cmd /c "$Command 2>&1" | Tee-Object -FilePath $log
$code = $LASTEXITCODE
if ($code -ne 0) {
    $drive = Get-PSDrive -Name C
    "Espacio libre en C: tras el fallo: {0:N1} GB" -f ($drive.Free / 1GB) | Write-Host
    $tail = (Get-Content $log -Tail 40 | ForEach-Object { $_ -replace '[\r\n]', '' }) -join '%0A'
    Write-Host "::error title=$Name fallo (codigo $code)::$tail"
    exit $code
}
