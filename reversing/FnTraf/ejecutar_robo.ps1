#requires -Version 7
<#
Robo de coches de trafico real en gta_sa_standalone.exe (salida de reversing\FnFase8\salida de ESTE arbol) en D:\Original\_robo_run (junctions de solo lectura a los datos
de D:\Grand Theft Auto San Andreas; partida 2 copiada de la copia de pruebas), bajo el candado D:\Original\gta_lock. Solo mata su propio proceso.
Uso: pwsh -File ejecutar_robo.ps1 -Etiqueta r1 [-Limite 700] [-Robar "14,10,15,10,0,1,1"] [-SegPos 125] [-Exe ruta]
#>
param([string]$Etiqueta = 'r', [int]$Limite = 700, [string]$Robar = '14,10,15,10,0,1,1', [int]$SegPos = 125, [string]$Exe = '', [string]$Extra = '--shims')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_robo_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'robo' | Set-Content 'D:\Original\gta_lock\owner_robo'
"candado obtenido tras {0:N0} s" -f ((Get-Date) - $esperado).TotalSeconds | Set-Content (Join-Path $sal 'espera_candado.txt')
$pr = $null
try {
    while (Get-Process gta_sa -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item 'E:\JUEGOS\GTA San Andreas - CVehicle Test\userfiles\GTASAsf2.b' (Join-Path $dest 'userfiles\GTASAsf2.b') -Force   # partida limpia en cada ejecucion
    if (-not $Exe) { $Exe = Join-Path $aqui '..\FnFase8\salida\gta_sa_standalone.exe' }
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $args_ = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra --robar=$Robar"
    $pr = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    "pid $($pr.Id)  args: $args_" | Set-Content (Join-Path $sal 'lanzador.txt')
    $son = Start-Process python -ArgumentList '-I', "`"$aqui\sonda_robo.py`"", "`"$sal`"", '--pid', $pr.Id, '--seg-pos', $SegPos, '--log', "`"$dest\logs\log.log`"" -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    if (-not $pr.WaitForExit(($Limite + 60) * 1000)) { 'TIMEOUT externo; mato mi proceso' | Add-Content (Join-Path $sal 'lanzador.txt'); $pr.Kill() }
    'exit=0x{0:X8}' -f $pr.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    if ($son -and -not $son.HasExited) { $son.WaitForExit(30000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
