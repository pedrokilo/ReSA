# Ejecuta un comando, guarda su salida en un log y, si falla, emite como ::error:: (visible en el resumen publico
# de la ejecucion sin iniciar sesion; los registros de Actions la exigen):
#   1) las lineas con errores del compilador/enlazador/CMake/Conan (sin duplicados, hasta 30),
#   2) las ultimas 12 lineas del log,
#   3) el espacio libre en disco.
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
    function Limpiar-Linea([string]$s) { ($s -replace '[\r\n]', '' -replace '\s+\[D:\\a\\[^\]]*\]\s*$', '').Trim() }
    $patron = 'error C\d+|error LNK\d+|fatal error|error RC\d+|: error |CMake Error|ERROR:|undefined reference|ConanException'
    $errores = Get-Content $log | Where-Object { $_ -match $patron } | ForEach-Object { Limpiar-Linea $_ } | Select-Object -Unique | Select-Object -First 30
    $cola = Get-Content $log -Tail 12 | ForEach-Object { Limpiar-Linea $_ }
    $lineas = @("== errores (hasta 30, sin duplicados) ==") + $errores + @("== ultimas 12 lineas ==") + $cola
    $texto = ($lineas | ForEach-Object { if ($_.Length -gt 400) { $_.Substring(0, 400) } else { $_ } }) -join '%0A'
    if ($texto.Length -gt 60000) { $texto = $texto.Substring(0, 60000) }
    Write-Host "::error title=$Name fallo (codigo $code)::$texto"
    exit $code
}
