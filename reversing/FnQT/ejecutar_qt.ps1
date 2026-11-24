#requires -Version 7
<#
Variante de FnJuego\ejecutar_juego.ps1 para la prueba de pistas de usuario: lanza gta_sa_standalone.exe (reversing\FnFase8\salida de ESTE arbol, o -Exe) en D:\Original\_qt_run (preparar_ejecucion.ps1) con la sonda de FnJuego (escenario usertrack) (junctions de solo lectura a los datos de D:\Grand Theft Auto San Andreas;
partida 2 copiada de _juego_run\userfiles\GTASAsf2.b.orig) bajo el candado D:\Original\gta_lock, y ejecuta la sonda sonda_juego.py con un escenario.
Solo mata su propio proceso. -XInput copia xinput1_4.dll FALSO (xinput_falso.c) junto al exe; se retira al terminar.
Uso: pwsh -File ejecutar_juego.ps1 -Etiqueta c1 -Escenario conduccion [-Limite 400] [-XInput] [-Extra "--shims"] [-SondaArgs ""]
#>
param([string]$Etiqueta = 'j', [string]$Escenario = 'base', [int]$Limite = 400, [switch]$XInput, [string]$Exe = '', [string]$Extra = '--shims', [string]$SondaArgs = '')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot; $jj = Join-Path (Split-Path $PSScriptRoot) 'FnJuego'
$dest = 'D:\Original\_qt_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'qt' | Set-Content 'D:\Original\gta_lock\owner_qt'
"candado obtenido tras {0:N0} s" -f ((Get-Date) - $esperado).TotalSeconds | Set-Content (Join-Path $sal 'espera_candado.txt')
$pr = $null
try {
    while (Get-Process gta_sa -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Remove-Item (Join-Path $dest 'xinput_falso*') -Force -ErrorAction SilentlyContinue
    Remove-Item (Join-Path $dest 'xinput1_4.dll') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'userfiles\GTASAsf2.b.orig') (Join-Path $dest 'userfiles\GTASAsf2.b') -Force   # partida limpia en cada ejecucion (copia)
    if ($XInput) { Copy-Item (Join-Path $aqui 'xinput1_4.dll') (Join-Path $dest 'xinput1_4.dll') -Force }
    if (-not $Exe) { $Exe = 'D:\Original\wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe' }
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $args_ = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    $pr = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    "pid $($pr.Id)  args: $args_  escenario=$Escenario xinput=$XInput" | Set-Content (Join-Path $sal 'lanzador.txt')
    $pico = Join-Path $jj 'pico_audio.exe'; $aud = $null
    if (Test-Path $pico) { $aud = Start-Process $pico -ArgumentList $pr.Id, $Limite, 1000 -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'audio_wasapi.txt') }
    $son = Start-Process python -ArgumentList '-I', "`"$jj\sonda_juego.py`"", "`"$sal`"", $Escenario, '--pid', $pr.Id, '--dir', "`"$dest`"", '--log', "`"$dest\logs\log.log`"", $SondaArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    $t0 = Get-Date; while (-not $pr.HasExited -and -not $son.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt ($Limite + 60)) { Start-Sleep 2 }
    if (-not $pr.HasExited) { Start-Sleep 3; if ($son.HasExited) { 'sonda terminada: cierro mi juego' | Add-Content (Join-Path $sal 'lanzador.txt') } ; $pr.Kill() }
    if (-not $pr.WaitForExit(5000)) { 'TIMEOUT externo; mato mi proceso' | Add-Content (Join-Path $sal 'lanzador.txt'); $pr.Kill() }
    'exit=0x{0:X8}' -f $pr.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    if ($aud -and -not $aud.HasExited) { $aud.Kill() }
    if ($son -and -not $son.HasExited) { $son.WaitForExit(30000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'xinput_falso*') $sal -ErrorAction SilentlyContinue
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    Remove-Item (Join-Path $dest 'xinput1_4.dll') -Force -ErrorAction SilentlyContinue
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal

