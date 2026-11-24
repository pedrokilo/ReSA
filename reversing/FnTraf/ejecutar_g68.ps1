#requires -Version 7
<#
Lanza el gta_sa_standalone.exe indicado en D:\Original\_g68_run (copia propia: junctions de solo lectura a los datos del juego,
partida 2 copiada) bajo el candado D:\Original\gta_lock (espera; solo toca su propio proceso) y, en paralelo, comparar_objetos.py
con el pid propio (lee memoria: objeto DatosExe frente a direccion del EXE).
Uso: pwsh -File ejecutar_g68.ps1 -Etiqueta c1 -Exe <exe> -Map <map> [-Limite 130] [-T 40,80,115]
#>
param([string]$Etiqueta = 'c', [string]$Exe, [string]$Map = '', [string]$Pdb = '', [int]$Limite = 130, [string]$T = '40,80,115', [string]$Extra = '--shims')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$repo = (Resolve-Path (Join-Path $aqui '..\..')).Path
$dest = 'D:\Original\_g68_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'g68' | Set-Content 'D:\Original\gta_lock\owner_g68'
try {
    while (Get-Process gta_sa*, gta_reversed_exe -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'userfiles\GTASAsf2.b.orig') (Join-Path $dest 'userfiles\GTASAsf2.b') -Force -ErrorAction SilentlyContinue
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $args_ = "--directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    $mapArgs = @('--map', "`"$Map`"")
    if ($Pdb) { $mapArgs = @('--pdb', "`"$Pdb`"", '--bin', "`"$Exe`"", '--modulo', 'gta_sa_standalone.exe') }
    $p = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    $pyArgs = @('-I', "`"$(Join-Path $repo 'contrib\auditoria_datos\comparar_objetos.py')`"") + $mapArgs + @('--repo', "`"$repo`"", '--pid', "$($p.Id)", '--salida', "`"$sal`"", '--t', $T, '--plan', "`"$(Join-Path $repo 'docs\DatosExe\plan-objetos.csv')`"")
    $son = Start-Process python -ArgumentList $pyArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'comparar.txt') -RedirectStandardError (Join-Path $sal 'comparar_err.txt')
    if (-not $p.WaitForExit(($Limite + 40) * 1000)) { 'TIMEOUT: mato mi proceso'; $p.Kill() }
    'exit=0x{0:X8}' -f $p.ExitCode | Tee-Object (Join-Path $sal 'lanzador.txt')
    if (-not $son.WaitForExit(30000)) { $son.Kill() }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
