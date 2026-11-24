#requires -Version 7
<#
Lanza gta_sa_standalone.exe (salida de reversing\FnFase8\salida de ESTE arbol) en D:\Original\_trafico_run (junctions de solo lectura a los datos de D:\Grand Theft Auto San Andreas),
bajo el candado D:\Original\gta_lock (espera si esta tomado; no mata procesos ajenos, solo el propio). Deja log y captura en ejecuciones\<etiqueta>-<hora>.
Uso: pwsh -File ejecutar_traf.ps1 -Etiqueta t1 [-Limite 120] [-Ini partida2|nueva] [-Extra "--shims"] [-Sonda script.py]
  -Sonda: script python que se lanza en paralelo con el pid del standalone y la carpeta de salida como argumentos.
#>
param([string]$Etiqueta = 't', [int]$Limite = 120, [string]$Ini = 'partida2', [string]$Extra = '--shims', [string]$Sonda = '', [string]$SondaArgs = '', [string]$Exe = '')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_trafico_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'trafico' | Set-Content 'D:\Original\gta_lock\owner_trafico'
try {
    while (Get-Process gta_sa*, gta_reversed_exe -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest "gta-reversed.ini.$Ini") (Join-Path $dest 'gta-reversed.ini') -Force
    if (-not $Exe) { $Exe = Join-Path $aqui '..\FnFase8\salida\gta_sa_standalone.exe' }
Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $env:FASE8_ARGS = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    $son = $null
    if ($Sonda) {
        $son = Start-Process python -ArgumentList '-I', "`"$Sonda`"", "`"$sal`"", $SondaArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    }
    pwsh -NoProfile -File (Join-Path $aqui '..\FnFase8\lanzar.ps1') -Exe (Join-Path $dest 'gta_sa_standalone.exe') -Dir $dest -EsperaS ($Limite + 30) | Tee-Object -FilePath (Join-Path $sal 'lanzador.txt')
    if ($son -and -not $son.HasExited) { $son.WaitForExit(20000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
