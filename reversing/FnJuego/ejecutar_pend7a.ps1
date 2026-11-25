#requires -Version 7
<#
Lanza gta_sa_standalone.exe (el de ESTA rama, -Exe) en D:\Original\_juego_run bajo el candado D:\Original\gta_lock (lo toma el propio script, nunca a mano) con
GTASA_SILENTPATCH_SELFTEST_PEND7A=1 (banco aislado de sourcegame_saSilentPatchPend7aSelfTest.cpp) y espera a que logs\silentpatch_pend7a_selftest.log contenga FIN.
Opciones de SilentPatch por entorno: -Entorno "GTASA_SILENTPATCH=0" o "GTASA_SILENTPATCH_CARGENCOUNTERFIX=0;GTASA_SILENTPATCH_VANREARDOORFIX=0".
Solo mata su propio proceso (el que lanza). Uso: pwsh -File ejecutar_sp2peds.ps1 -Exe <exe> -Etiqueta on [-Entorno ""] [-Limite 240]
#>
param([string]$Etiqueta = 'sp2', [string]$Exe = '', [string]$Entorno = '', [int]$Limite = 240)
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_juego_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'sp7a' | Set-Content 'D:\Original\gta_lock\owner_sp7a'
"candado obtenido tras {0:N0} s" -f ((Get-Date) - $esperado).TotalSeconds | Set-Content (Join-Path $sal 'espera_candado.txt')
$pr = $null
$guardados = @{}
try {
    while (Get-Process gta_sa -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'userfiles\GTASAsf2.b.orig') (Join-Path $dest 'userfiles\GTASAsf2.b') -Force
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $env:GTASA_SILENTPATCH_SELFTEST_PEND7A = '1'
    foreach ($par in ($Entorno -split ';' | Where-Object { $_ })) { $k, $v = $par -split '=', 2; Set-Item -Path ("Env:" + $k) -Value $v; $guardados[$k] = 1 }
    $args_ = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite --shims"
    $pr = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    "pid $($pr.Id)  args: $args_  entorno=$Entorno" | Set-Content (Join-Path $sal 'lanzador.txt')
    $log = Join-Path $dest 'logs\silentpatch_pend7a_selftest.log'
    $t0 = Get-Date
    while (-not $pr.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt ($Limite + 60)) {
        if ((Test-Path $log) -and (Select-String -Path $log -Pattern 'SELFTEST FIN' -Quiet)) { Start-Sleep 3; break }
        Start-Sleep 2
    }
    if (-not $pr.HasExited) { $pr.Kill() }
    $pr.WaitForExit(5000) | Out-Null
    'exit=0x{0:X8}' -f $pr.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    Remove-Item Env:GTASA_SILENTPATCH_SELFTEST_PEND7A -ErrorAction SilentlyContinue
    foreach ($k in $guardados.Keys) { Remove-Item ("Env:" + $k) -ErrorAction SilentlyContinue }
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
