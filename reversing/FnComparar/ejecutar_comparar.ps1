#requires -Version 7
<#
Lanza UNA ejecucion de comparacion bajo el candado D:\Original\gta_lock (espera si esta tomado; no mata procesos ajenos, solo el propio).
  -Modo orig : gta_sa.exe ORIGINAL puro (sin gta_reversed.asi) en D:\Original\_comparar\orig, con dinput8.dll + scripts\ que contiene SOLO
               III.VC.SA.WindowedMode.asi (ventana, no es codigo del proyecto) y redir_docs.asi (calza que redirige "Mis documentos" a la copia).
  -Modo std  : gta_sa_standalone.exe (salida de FnFase8\salida de este arbol, o -Exe) en D:\Original\_comparar\std, FastLoader ranura 2.
  -Modo asi  : gta_sa.exe original + gta_reversed.asi (bin\Debug) con -AsiArgs (control EXE original + ASI).
Uso: pwsh -File ejecutar_comparar.ps1 -Modo orig -Etiqueta o1 -Limite 600 -Sonda comparar.py -SondaArgs "--hora 12"
#>
param([ValidateSet('orig','std','asi')][string]$Modo = 'orig', [string]$Etiqueta = 'x', [int]$Limite = 600, [string]$Sonda = '', [string]$SondaArgs = '', [string]$Exe = '', [string]$Asi = '', [string]$AsiArgs = '', [string]$StdArgs = '--shims', [string]$IniExtra = '', [switch]$Espia, [int]$Cap = 0)
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$base = 'D:\Original\_comparar'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'comparacion-original' | Set-Content 'D:\Original\gta_lock\owner_comparar'
$p = $null
try {
    while (Get-Process gta_sa*, gta_reversed_exe -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    python -I (Join-Path $aqui 'gamma_rampa.py') pre | Add-Content (Join-Path $sal 'gamma.txt')
    $dir = Join-Path $base $(if ($Modo -eq 'std') { 'std' } else { 'orig' })
    Remove-Item (Join-Path $dir 'logs\*') -Force -ErrorAction SilentlyContinue
    Remove-Item (Join-Path $dir 'comparar_shim.log') -Force -ErrorAction SilentlyContinue
    $env:COMPARAR_DOCS = Join-Path $base 'orig\docs'
    # copia fresca de userfiles (gta_sa.set y partida) en cada ejecucion, para que ambos modos partan del mismo ajuste grafico
    $fuente = 'E:\JUEGOS\GTA San Andreas - CVehicle Test\userfiles'
    if ($Modo -eq 'std') {
        Copy-Item "$fuente\GTASAsf2.b", "$fuente\gta_sa.set" (Join-Path $dir 'userfiles') -Force
        if (-not $Exe) { $Exe = 'D:\Original\wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe' }
        Copy-Item $Exe (Join-Path $dir 'gta_sa_standalone.exe') -Force
        "[FastLoader]`r`nEnable = true`r`nSaveGameToLoad = 2`r`nNoEAX = true`r`nNoNVidia = true`r`nNoLogo = true`r`nNoTitleOrIntro = true`r`nNoCopyright = true`r`nNoFading = true`r`nNoLoadingTune = true`r`nRenderAtAllTimes = true`r`n$(($IniExtra -split ';' | Where-Object { $_ } | ForEach-Object { $_ + [char]13 + [char]10 }) -join '')[Misc]`r`nSkipDeviceSelection = true`r`nSaveDirectoryPath = $dir\userfiles`r`nLoadSavesWithMismatchingVersion = true`r`n" | Set-Content (Join-Path $dir 'gta-reversed.ini') -Encoding ascii
        $env:FASE8_ARGS = "--directorio-juego `"$dir`" --salida `"$sal`" --limite $Limite $StdArgs"
        if ($Espia) { $pid_ = python -I (Join-Path $aqui '..\FnSinOriginal\lanzar_espiado.py') (Join-Path $dir 'gta_sa_standalone.exe') $dir (Join-Path $sal 'espia.log') $env:FASE8_ARGS; $p = Get-Process -Id ([int]($pid_ | Select-Object -Last 1)) }
        else { $p = Start-Process -FilePath (Join-Path $dir 'gta_sa_standalone.exe') -ArgumentList $env:FASE8_ARGS -WorkingDirectory $dir -PassThru }
    } else {
        $df = Join-Path $env:COMPARAR_DOCS 'GTA San Andreas User Files'
        New-Item -ItemType Directory -Force $df | Out-Null
        Copy-Item "$fuente\GTASAsf2.b", "$fuente\gta_sa.set" $df -Force
        $sc = Join-Path $dir 'scripts'
        if ($Modo -eq 'asi') {
            Remove-Item "$sc\redir_docs.asi" -ErrorAction SilentlyContinue
            if (-not $Asi) { $Asi = 'D:\Original\wv-int\bin\Debug\gta_reversed.asi' }
            Copy-Item $Asi "$sc\gta_reversed.asi" -Force
            "[FastLoader]`r`nEnable = true`r`nSaveGameToLoad = 2`r`nNoEAX = true`r`nNoNVidia = true`r`nNoLogo = true`r`nNoTitleOrIntro = true`r`nNoCopyright = true`r`nNoFading = true`r`nNoLoadingTune = true`r`nRenderAtAllTimes = true`r`n$(($IniExtra -split ';' | Where-Object { $_ } | ForEach-Object { $_ + [char]13 + [char]10 }) -join '')[Misc]`r`nSkipDeviceSelection = true`r`nSaveDirectoryPath = $dir\userfiles`r`nLoadSavesWithMismatchingVersion = true`r`n" | Set-Content (Join-Path $dir 'gta-reversed.ini') -Encoding ascii
            Copy-Item "$fuente\GTASAsf2.b", "$fuente\gta_sa.set" (Join-Path $dir 'userfiles') -Force
            Copy-Item (Join-Path $dir 'gta-reversed.ini') (Join-Path $sc 'gta-reversed.ini') -Force
            Remove-Item "$sc\III.VC.SA.WindowedMode.asi" -ErrorAction SilentlyContinue
        } else {
            Remove-Item "$sc\gta_reversed.asi", "$sc\gta-reversed.ini", (Join-Path $dir 'gta-reversed.ini') -ErrorAction SilentlyContinue
            if (Test-Path (Join-Path $aqui 'shim\redir_docs.asi')) { Copy-Item (Join-Path $aqui 'shim\redir_docs.asi') $sc -Force }
            Copy-Item 'D:\Grand Theft Auto San Andreas\Windowed Mode\III.VC.SA.WindowedMode.asi' $sc -Force
            Copy-Item (Join-Path $base 'III.VC.SA.WindowedMode.ini') $sc -Force
        }
        if ($Espia) { $pid_ = python -I (Join-Path $aqui '..\FnSinOriginal\lanzar_espiado.py') (Join-Path $dir 'gta_sa.exe') $dir (Join-Path $sal 'espia.log') $AsiArgs; $p = Get-Process -Id ([int]($pid_ | Select-Object -Last 1)) }
        else { $p = Start-Process -FilePath (Join-Path $dir 'gta_sa.exe') -ArgumentList $AsiArgs -WorkingDirectory $dir -PassThru }
    }
    $son = $null
    if ($Sonda) {
        $son = Start-Process python -ArgumentList '-I', "`"$(Join-Path $aqui $Sonda)`"", "`"$sal`"", '--pid', $p.Id, '--modo', $Modo, $SondaArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    }
    if (-not $p.WaitForExit($Limite * 1000)) { 'TIMEOUT: el proceso sigue vivo (es mio, lo cierro)'; $p.Kill() }
    "exit=0x{0:X8}" -f $p.ExitCode
    if ($son -and -not $son.HasExited) { $son.WaitForExit(20000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Start-Sleep 2; python -I (Join-Path $aqui 'gamma_rampa.py') post | Add-Content (Join-Path $sal 'gamma.txt')
    Copy-Item (Join-Path $dir 'logs\*') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dir 'comparar_shim.log') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dir 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
} finally {
    if ($p -and -not $p.HasExited) { $p.Kill() }
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
