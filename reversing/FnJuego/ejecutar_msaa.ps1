#requires -Version 7
<#
Como ejecutar_juego.ps1 (mismo candado D:\Original\gta_lock que toma el propio script, misma carpeta D:\Original\_juego_run, mismo standalone, solo mata su proceso) y ademas:
-PrefsAA <n>: antes de lanzar escribe n en el campo m_nPrefsAntialiasing (int32) de userfiles\gta_sa.set (se localiza como el int32 que precede al de la calidad de efectos y al byte 0x54 'T'),
y al terminar RESTAURA el gta_sa.set tal como estaba (copia de bytes tomada con el candado ya en mano). Sirve para probar el acotado del valor guardado (SilentPatch MSAAFix, L66).
Uso: pwsh -File ejecutar_msaa.ps1 -Etiqueta clamp9 -Escenario msaa_estado -PrefsAA 9 [-Limite 120] [-Exe <exe>]; las opciones de SilentPatch se pasan por entorno (GTASA_SILENTPATCH_MSAAFIX=0).
#>
param([string]$Etiqueta = 'j', [string]$Escenario = 'msaa_estado', [int]$Limite = 150, [string]$Exe = '', [string]$Extra = '--shims', [string]$SondaArgs = '', [int]$PrefsAA = 0)
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_juego_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
$esperado = Get-Date
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'msaa' | Set-Content 'D:\Original\gta_lock\owner_msaa'
"candado obtenido tras {0:N0} s" -f ((Get-Date) - $esperado).TotalSeconds | Set-Content (Join-Path $sal 'espera_candado.txt')
$pr = $null
$setRuta = Join-Path $dest 'userfiles\gta_sa.set'
$setCopia = $null
try {
    while (Get-Process gta_sa -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'userfiles\GTASAsf2.b.orig') (Join-Path $dest 'userfiles\GTASAsf2.b') -Force
    if ($PrefsAA -ne 0) {
        $setCopia = [System.IO.File]::ReadAllBytes($setRuta)
        $b = [byte[]]$setCopia.Clone()
        $hallado = -1
        for ($p = 8; $p -lt $b.Length; $p++) {
            if ($b[$p] -eq 0x54 -and [BitConverter]::ToUInt32($b, $p - 4) -le 3 -and [BitConverter]::ToUInt32($b, $p - 8) -le 16) { $hallado = $p - 8; break }
        }
        if ($hallado -lt 0) { throw 'no se localiza m_nPrefsAntialiasing en gta_sa.set' }
        $antes = [BitConverter]::ToUInt32($b, $hallado)
        [BitConverter]::GetBytes([uint32]$PrefsAA).CopyTo($b, $hallado)
        [System.IO.File]::WriteAllBytes($setRuta, $b)
        "gta_sa.set: m_nPrefsAntialiasing en el desplazamiento ${hallado}: $antes -> $PrefsAA" | Set-Content (Join-Path $sal 'prefs_aa.txt')
    }
    if (-not $Exe) { $Exe = 'D:\Original\wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe' }
    Copy-Item $Exe (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $args_ = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    $pr = Start-Process -FilePath (Join-Path $dest 'gta_sa_standalone.exe') -ArgumentList $args_ -WorkingDirectory $dest -PassThru
    "pid $($pr.Id)  args: $args_  escenario=$Escenario prefsAA=$PrefsAA" | Set-Content (Join-Path $sal 'lanzador.txt')
    $son = Start-Process python -ArgumentList '-I', "`"$aqui\sonda_juego.py`"", "`"$sal`"", $Escenario, '--pid', $pr.Id, '--dir', "`"$dest`"", '--log', "`"$dest\logs\log.log`"", $SondaArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    $t0 = Get-Date; while (-not $pr.HasExited -and -not $son.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt ($Limite + 60)) { Start-Sleep 2 }
    if (-not $pr.HasExited) { Start-Sleep 3; if ($son.HasExited) { 'sonda terminada: cierro mi juego' | Add-Content (Join-Path $sal 'lanzador.txt') } ; $pr.Kill() }
    if (-not $pr.WaitForExit(5000)) { 'TIMEOUT externo; mato mi proceso' | Add-Content (Join-Path $sal 'lanzador.txt'); $pr.Kill() }
    'exit=0x{0:X8}' -f $pr.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    if ($son -and -not $son.HasExited) { $son.WaitForExit(30000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
    if ($setCopia -ne $null) { Copy-Item $setRuta (Join-Path $sal 'gta_sa.set.despues') -ErrorAction SilentlyContinue }
} finally {
    if ($pr -and -not $pr.HasExited) { $pr.Kill() }
    if ($setCopia -ne $null) { [System.IO.File]::WriteAllBytes($setRuta, $setCopia) }
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
