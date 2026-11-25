#requires -Version 7
<#
Lanza UNA ejecucion de la sonda de skinning bajo el candado D:\Original\gta_lock (espera si esta tomado; solo mata su propio proceso).
  -Modo orig : gta_sa.exe ORIGINAL puro (sin gta_reversed.asi) en D:\Original\_comparar\orig (calzas de ventana y de documentos que ya hay en scripts\).
  -Modo std  : gta_sa_standalone.exe (-Exe, por defecto la salida de FnFase8\salida de este arbol) en D:\Original\_comparar\std, FastLoader ranura 2.
  -Modo asi  : gta_sa.exe original + gta_reversed.asi (-Asi, por defecto wv-int\bin\Debug) en D:\Original\_comparar\orig.
  -Vigilancia: pone GTASA_SKIN_VIGILANCIA=1 en el entorno del juego (solo tiene efecto en un build con SkinVigilancia.cpp).
  -Evid      : carpeta de evidencias pesadas (PNG) fuera de D: (D: casi llena); por defecto C:\skin_evidencia\<etiqueta>.
Uso: pwsh -File ejecutar_skin.ps1 -Modo std -Etiqueta s1 -Limite 900 -Vigilancia -SondaArgs "--escenarios bici481 --minutos 10"
#>
param([ValidateSet('orig','std','asi')][string]$Modo = 'std', [string]$Etiqueta = 'x', [int]$Limite = 900, [string]$SondaArgs = '--escenarios bici481 --minutos 10', [string]$Exe = '', [string]$Asi = '', [switch]$Vigilancia, [string]$Evid = '', [string]$ExtraStd = '--shims')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$base = 'D:\Original\_comparar'
$ts = Get-Date -Format 'yyyyMMdd-HHmmss'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}-{2}" -f $Etiqueta, $Modo, $ts)
New-Item -ItemType Directory -Force $sal | Out-Null
if (-not $Evid) { $Evid = "C:\skin_evidencia\$Etiqueta-$Modo-$ts" }
New-Item -ItemType Directory -Force $Evid | Out-Null
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'skin' | Set-Content 'D:\Original\gta_lock\owner_skin'
$p = $null
try {
    while (Get-Process gta_sa*, gta_reversed_exe -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    $dir = Join-Path $base $(if ($Modo -eq 'std') { 'std' } else { 'orig' })
    Remove-Item (Join-Path $dir 'logs\*') -Force -ErrorAction SilentlyContinue
    $env:COMPARAR_DOCS = Join-Path $base 'orig\docs'
    if ($Vigilancia) { $env:GTASA_SKIN_VIGILANCIA = '1' } else { Remove-Item Env:\GTASA_SKIN_VIGILANCIA -ErrorAction SilentlyContinue }
    $fuente = Join-Path $base 'orig\userfiles'
    $ini = "[FastLoader]`r`nEnable = true`r`nSaveGameToLoad = 2`r`nNoEAX = true`r`nNoNVidia = true`r`nNoLogo = true`r`nNoTitleOrIntro = true`r`nNoCopyright = true`r`nNoFading = true`r`nNoLoadingTune = true`r`nRenderAtAllTimes = true`r`n[Misc]`r`nSkipDeviceSelection = true`r`nSaveDirectoryPath = $dir\userfiles`r`nLoadSavesWithMismatchingVersion = true`r`n"
    if ($Modo -eq 'std') {
        Copy-Item "$fuente\GTASAsf2.b", "$fuente\gta_sa.set" (Join-Path $dir 'userfiles') -Force
        if (-not $Exe) { $Exe = Join-Path (Split-Path $aqui) 'FnFase8\salida\gta_sa_standalone.exe' }
        Copy-Item $Exe (Join-Path $dir 'gta_sa_standalone.exe') -Force
        $ini | Set-Content (Join-Path $dir 'gta-reversed.ini') -Encoding ascii
        $a = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dir`" --salida `"$sal`" --limite $Limite $ExtraStd"
        $p = Start-Process -FilePath (Join-Path $dir 'gta_sa_standalone.exe') -ArgumentList $a -WorkingDirectory $dir -PassThru
    } else {
        $df = Join-Path $env:COMPARAR_DOCS 'GTA San Andreas User Files'
        New-Item -ItemType Directory -Force $df | Out-Null
        Copy-Item "$fuente\GTASAsf2.b", "$fuente\gta_sa.set" $df -Force
        $sc = Join-Path $dir 'scripts'
        if ($Modo -eq 'asi') {
            Remove-Item "$sc\redir_docs.asi" -ErrorAction SilentlyContinue
            if (-not $Asi) { $Asi = 'D:\Original\wv-int\bin\Debug\gta_reversed.asi' }
            Copy-Item $Asi "$sc\gta_reversed.asi" -Force
            $ini | Set-Content (Join-Path $dir 'gta-reversed.ini') -Encoding ascii
            Copy-Item "$fuente\GTASAsf2.b", "$fuente\gta_sa.set" (Join-Path $dir 'userfiles') -Force
            Copy-Item (Join-Path $dir 'gta-reversed.ini') (Join-Path $sc 'gta-reversed.ini') -Force
            Move-Item "$sc\III.VC.SA.WindowedMode.asi" "$sc\III.VC.SA.WindowedMode.asi.off" -Force -ErrorAction SilentlyContinue
        } else {
            Remove-Item "$sc\gta_reversed.asi", "$sc\gta-reversed.ini", (Join-Path $dir 'gta-reversed.ini') -ErrorAction SilentlyContinue
            if (Test-Path "$sc\III.VC.SA.WindowedMode.asi.off") { Move-Item "$sc\III.VC.SA.WindowedMode.asi.off" "$sc\III.VC.SA.WindowedMode.asi" -Force }
        }
        $p = Start-Process -FilePath (Join-Path $dir 'gta_sa.exe') -WorkingDirectory $dir -PassThru
    }
    $mapa = Join-Path (Split-Path $aqui) 'FnFase8\salida\gta_sa_standalone.map'
    $son = Start-Process python -ArgumentList '-I', "`"$(Join-Path $aqui 'sonda_skin.py')`"", "`"$sal`"", '--pid', $p.Id, '--modo', $Modo, '--evid', "`"$Evid`"", '--log', "`"$dir\logs\log.log`"", '--mapa', "`"$mapa`"", $SondaArgs -PassThru -WindowStyle Hidden -RedirectStandardOutput (Join-Path $sal 'sonda.txt') -RedirectStandardError (Join-Path $sal 'sonda_err.txt')
    "pid $($p.Id) modo $Modo vigilancia $Vigilancia evid $Evid args $SondaArgs" | Set-Content (Join-Path $sal 'lanzador.txt')
    $t0 = Get-Date
    while (-not $p.HasExited -and -not $son.HasExited -and ((Get-Date) - $t0).TotalSeconds -lt $Limite) { Start-Sleep 2 }
    if (-not $p.HasExited) { Start-Sleep 3; 'cierro mi juego (sonda terminada o limite)' | Add-Content (Join-Path $sal 'lanzador.txt'); $p.Kill() }
    'exit=0x{0:X8}' -f $p.ExitCode | Add-Content (Join-Path $sal 'lanzador.txt')
    if ($son -and -not $son.HasExited) { $son.WaitForExit(90000) | Out-Null; if (-not $son.HasExited) { $son.Kill() } }
    Copy-Item (Join-Path $dir 'logs\*') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dir 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
} finally {
    if ($p -and -not $p.HasExited) { $p.Kill() }
    Remove-Item Env:\GTASA_SKIN_VIGILANCIA -ErrorAction SilentlyContinue
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
