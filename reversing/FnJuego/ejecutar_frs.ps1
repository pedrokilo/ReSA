#requires -Version 7
<#
Lanza gta_sa_standalone.exe (el de ESTA rama, -Exe) en D:\Original\_frs_run (junctions de solo lectura a los datos de D:\Grand Theft Auto San Andreas; partida 2 copiada de
userfiles\GTASAsf2.b.orig; cleo\cleo_text con los .fxt del usuario) bajo el candado D:\Original\gta_lock (lo toma el propio script, nunca a mano) con
GTASA_FRS_SELFTEST=<modo> (banco en juego de source\game_sa\FreeRoamSubtitles.cpp) y arranca frs_captura.py, que hace capturas de la ventana cuando el log dice MOSTRADO / FIN DE VOZ.
Espera a que logs\free_roam_subs_juego.log contenga FIN (o al limite). Solo mata su propio proceso.
Opciones por entorno: -Entorno "GTASA_FREE_ROAM_SUBS=0;GTASA_FRS_READINGTIME=2000". -Ini escribe gta-reversed.ini adicional (lineas separadas por ';'), -SinFxt retira cleo\cleo_text de la ejecucion.
Uso: pwsh -File ejecutar_frs.ps1 -Etiqueta on [-Modo 3] [-Entorno ""] [-Limite 150] [-Exe <exe>] [-SinFxt]
#>
param([string]$Etiqueta = 'frs', [string]$Exe = '', [string]$Entorno = '', [int]$Limite = 150, [int]$Modo = 3, [string]$IniExtra = '', [switch]$SinFxt, [switch]$SinCaptura, [switch]$CapturarPasos)
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_frs_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
if (-not $Exe) { $Exe = 'D:\Original\wv-sub\reversing\FnFase8\salida\gta_sa_standalone.exe' }
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'frs' | Set-Content 'D:\Original\gta_lock\owner_frs'
"candado obtenido tras {0:N0} s" -f ((Get-Date) - $esperado).TotalSeconds | Set-Content (Join-Path $sal 'espera_candado.txt')
$pr = $null; $cap = $null
$guardados = @{}
$iniOrig = Get-Content (Join-Path $dest 'gta-reversed.ini') -Raw
$fxtMov = $false
try {
    while (Get-Process gta_sa -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'userfiles\GTASAsf2.b.orig') (Join-Path $dest 'userfiles\GTASAsf2.b') -Force
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    if ($SinFxt -and (Test-Path "$dest\cleo\cleo_text")) { Rename-Item "$dest\cleo\cleo_text" 'cleo_text_off'; $fxtMov = $true }
    if ($IniExtra) { Add-Content (Join-Path $dest 'gta-reversed.ini') ("`n" + ($IniExtra -split ';' -join "`n")) }
    $env:GTASA_FRS_SELFTEST = "$Modo"
    foreach ($par in ($Entorno -split ';' | Where-Object { $_ })) { $k, $v = $par -split '=', 2; Set-Item -Path ("Env:" + $k) -Value $v; $guardados[$k] = 1 }
    $args_ = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite --shims"
    $pr = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    "pid $($pr.Id)  args: $args_  entorno=$Entorno ini=$IniExtra modo=$Modo" | Set-Content (Join-Path $sal 'lanzador.txt')
    $log = Join-Path $dest 'logs\free_roam_subs_juego.log'
    if (-not $SinCaptura) {
        $capArgs = @('-I', "`"$aqui\frs_captura.py`"", "`"$sal`"", $log, '--pid', $pr.Id, '--limite', $Limite)
        if ($CapturarPasos) { $capArgs += '--paso' }
        $cap = Start-Process python -ArgumentList $capArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'captura.txt') -RedirectStandardError (Join-Path $sal 'captura_err.txt')
    }
    $t0 = Get-Date
    while (-not $pr.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt ($Limite + 60)) {
        if ((Test-Path $log) -and (Select-String -Path $log -Pattern '^FIN$' -Quiet)) { Start-Sleep 6; break }
        Start-Sleep 2
    }
    if (-not $pr.HasExited) { $pr.Kill() }
    $pr.WaitForExit(5000) | Out-Null
    'exit=0x{0:X8}' -f $pr.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    if ($cap -and -not $cap.HasExited) { $cap.WaitForExit(20000) | Out-Null; if (-not $cap.HasExited) { $cap.Kill() } }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    if ($cap -and -not $cap.HasExited) { $cap.Kill() }
    if ($fxtMov -and (Test-Path "$dest\cleo\cleo_text_off")) { Rename-Item "$dest\cleo\cleo_text_off" 'cleo_text' }
    Set-Content (Join-Path $dest 'gta-reversed.ini') $iniOrig -NoNewline
    Remove-Item Env:GTASA_FRS_SELFTEST -ErrorAction SilentlyContinue
    foreach ($k in $guardados.Keys) { Remove-Item ("Env:" + $k) -ErrorAction SilentlyContinue }
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
