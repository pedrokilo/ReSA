#requires -Version 7
<#
IND1A: prueba MANUAL del exe propio (gta_reversed_exe.exe) con el juego real.
 1) crea la carpeta E:\JUEGOS\GTA San Andreas - CVehicle Test - IND1A (junctions a los datos de la copia de pruebas; sin dinput8.dll ni scripts\,
    asi gta_reversed.asi NO se carga una segunda vez) y copia ahi el exe;
 2) lo lanza sin limite de tiempo (--limite 0): juega, y cierra la ventana cuando quieras.
Antes: construir el exe (reversing\FnIND1A\construir.sh, ver README) o pasar -Exe con uno ya hecho. Toma D:\Original\gta_lock mientras corre.
Uso: pwsh -File probar_a_mano.ps1 [-FastLoader] [-Exe <ruta del gta_reversed_exe.exe>]
#>
param([switch]$FastLoader, [string]$Exe = (Join-Path $PSScriptRoot 'salida\gta_reversed_exe.exe'))
$ErrorActionPreference = 'Stop'
$dest = 'E:\JUEGOS\GTA San Andreas - CVehicle Test - IND1A'
$orig = 'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
if (-not (Test-Path $Exe)) { throw "No existe $Exe" }
if (Get-Process -Name gta_sa, gta_reversed_exe -ErrorAction SilentlyContinue) { throw 'Hay un juego en marcha; cierralo antes.' }
$candado = 'D:\Original\gta_lock'
try { New-Item -ItemType Directory -Path $candado -ErrorAction Stop | Out-Null } catch { throw "El candado $candado esta tomado por otra prueba." }
try {
    Set-Content (Join-Path $candado 'owner_manual') 'IND1A prueba a mano'
    if ($FastLoader) { & (Join-Path $PSScriptRoot 'crear_carpeta_ind1a.ps1') -FastLoader | Out-Null } else { & (Join-Path $PSScriptRoot 'crear_carpeta_ind1a.ps1') | Out-Null }
    Copy-Item $Exe (Join-Path $dest 'gta_reversed_exe.exe') -Force
    $salida = Join-Path $PSScriptRoot ('ejecuciones\manual-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
    New-Item -ItemType Directory -Force $salida | Out-Null
    $args_ = "--ruta-original `"$orig`" --directorio-juego `"$dest`" --salida `"$salida`" --ejecutar --limite 0"
    Write-Host "Lanzando: gta_reversed_exe.exe $args_`nInformes en $salida (ind1a_arranque.log, ind1a_fallo.txt si falla)"
    $p = Start-Process -FilePath (Join-Path $dest 'gta_reversed_exe.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    $p.WaitForExit()
    'Codigo de salida: 0x{0:X8}' -f $p.ExitCode
} finally {
    Remove-Item -LiteralPath $candado -Recurse -Force -ErrorAction SilentlyContinue
}
