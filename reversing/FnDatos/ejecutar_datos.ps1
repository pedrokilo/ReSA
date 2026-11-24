#requires -Version 7
<#
Variante de reversing\FnJuego\ejecutar_juego.ps1 para la auditoria dinamica de 'dato en dos sitios': lanza el standalone (-Exe) en D:\Original\_juego_run bajo el candado
D:\Original\gta_lock y ejecuta reversing\FnDatos\sonda_datos.py (escenarios encadenados + vigilancia de la memoria del EXE). Solo mata su propio proceso.
Uso: pwsh -File ejecutar_datos.ps1 -Etiqueta d1 -Escenario interiores,conduccion -Exe <exe> -Mapa <map> [-Limite 900] [-XInput]
#>
param([string]$Etiqueta = 'd', [string]$Escenario = 'base', [int]$Limite = 900, [switch]$XInput, [string]$Exe = '', [string]$Mapa = '', [string]$Extra = '--shims', [string]$SondaExtra = '--nada')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_juego_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'datos-g5b' | Set-Content 'D:\Original\gta_lock\owner_datos_g5b'
"candado obtenido tras {0:N0} s" -f ((Get-Date) - $esperado).TotalSeconds | Set-Content (Join-Path $sal 'espera_candado.txt')
$pr = $null
try {
    while (Get-Process gta_sa, gta_sa_standalone -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Remove-Item (Join-Path $dest 'xinput1_4.dll') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'userfiles\GTASAsf2.b.orig') (Join-Path $dest 'userfiles\GTASAsf2.b') -Force
    if ($XInput) { Copy-Item (Join-Path $aqui '..\FnJuego\xinput1_4.dll') (Join-Path $dest 'xinput1_4.dll') -Force }
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $args_ = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    $pr = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    "pid $($pr.Id)  args: $args_  escenario=$Escenario" | Set-Content (Join-Path $sal 'lanzador.txt')
    $son = Start-Process python -ArgumentList '-I', "`"$aqui\sonda_datos.py`"", "`"$sal`"", $Escenario, '--pid', $pr.Id, '--dir', "`"$dest`"", '--log', "`"$dest\logs\log.log`"", '--mapa', "`"$Mapa`"", '--sizes', "`"$aqui\sizes_prev.json`"", $SondaExtra -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    $t0 = Get-Date; while (-not $pr.HasExited -and -not $son.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt ($Limite + 60)) { Start-Sleep 2 }
    if (-not $pr.HasExited) { Start-Sleep 8; $pr.Kill() }
    if (-not $pr.WaitForExit(5000)) { $pr.Kill() }
    'exit=0x{0:X8}' -f $pr.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    if ($son -and -not $son.HasExited) { $son.WaitForExit(40000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    Remove-Item (Join-Path $dest 'xinput1_4.dll') -Force -ErrorAction SilentlyContinue
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
