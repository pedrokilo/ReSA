#requires -Version 7
<#
Lanza el gta_sa.exe ORIGINAL (copia solo de lectura de D:\Original\_comparar\orig\gta_sa.exe, datos por junction a la copia de pruebas) en D:\Original\_cai_orig con
el cargador ASI y NUESTRA DLL solo como cargador (FastLoader, partida 2) y TODOS los ganchos apagados (--unhook-all): el codigo del juego es el del EXE original.
Bajo el candado D:\Original\gta_lock; ejecuta sonda_juego.py con un escenario. Solo mata su propio proceso.
Uso: pwsh -File ejecutar_orig.ps1 -Etiqueta o1 -Escenario caida_interior -Asi <gta_reversed.asi> [-Limite 330] [-Args "--unhook-all"]
#>
param([string]$Etiqueta = 'o', [string]$Escenario = 'caida_interior', [string]$Asi = 'D:\Original\_asi_spe_ctl.asi', [int]$Limite = 330, [string]$ArgsJuego = '--unhook-all', [string]$SondaArgs = '')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_cai_orig'
$orig = 'D:\Original\_comparar\orig'
$datos = 'E:\JUEGOS\GTA San Andreas - CVehicle Test'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
New-Item -ItemType Directory -Force $dest, "$dest\scripts", "$dest\userfiles", "$dest\logs" | Out-Null
foreach ($d in 'anim', 'audio', 'data', 'models', 'movies', 'text') { if (-not (Test-Path "$dest\$d")) { cmd /c mklink /J "$dest\$d" "$datos\$d" | Out-Null } }
foreach ($f in 'gta_sa.exe', 'dinput8.dll', 'eax.dll', 'ogg.dll', 'vorbis.dll', 'vorbisFile.dll', 'vorbisHooked.dll', 'stream.ini') { Copy-Item "$orig\$f" "$dest\$f" -Force }
Copy-Item "$orig\scripts\III.VC.SA.WindowedMode.asi", "$orig\scripts\III.VC.SA.WindowedMode.ini" "$dest\scripts" -Force
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'juego' | Set-Content 'D:\Original\gta_lock\owner_juego'
$pr = $null
try {
    while (Get-Process gta_sa -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item "$dest\logs\*", "$dest\scripts\logs\*" -Force -Recurse -ErrorAction SilentlyContinue
    Copy-Item "$orig\userfiles\GTASAsf2.b" "$dest\userfiles\GTASAsf2.b" -Force
    Copy-Item $Asi "$dest\scripts\gta_reversed.asi" -Force
    @"
[FastLoader]
Enable = true
SaveGameToLoad = 2
NoEAX = true
NoNVidia = true
NoLogo = true
NoTitleOrIntro = true
NoCopyright = true
NoFading = true
NoLoadingTune = true
RenderAtAllTimes = true
[Misc]
SkipDeviceSelection = true
SaveDirectoryPath = $dest\userfiles
LoadSavesWithMismatchingVersion = true
"@ | Set-Content "$dest\gta-reversed.ini"
    Copy-Item "$dest\gta-reversed.ini" "$dest\scripts\gta-reversed.ini" -Force
    $pr = Start-Process -FilePath "$dest\gta_sa.exe" -ArgumentList $ArgsJuego -WorkingDirectory $dest -PassThru
    "pid $($pr.Id) args: $ArgsJuego asi: $Asi escenario=$Escenario" | Set-Content (Join-Path $sal 'lanzador.txt')
    $log = "$dest\scripts\logs\log.log"
    $son = Start-Process python -ArgumentList '-I', "`"$aqui\sonda_juego.py`"", "`"$sal`"", $Escenario, '--pid', $pr.Id, '--dir', "`"$dest`"", '--log', "`"$log`"", $SondaArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    $t0 = Get-Date; while (-not $pr.HasExited -and -not $son.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt ($Limite + 60)) { Start-Sleep 2 }
    if (-not $pr.HasExited) { Start-Sleep 3; $pr.Kill() }
    $pr.WaitForExit(5000) | Out-Null
    if ($son -and -not $son.HasExited) { $son.WaitForExit(30000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item "$dest\scripts\logs\*", "$dest\logs\*" $sal -ErrorAction SilentlyContinue
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
