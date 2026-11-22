#requires -Version 7
<#
Ejecuta gta_sa_standalone.exe (arranque propio del CRT, parte CRT-B) en una carpeta de pruebas sobre D: (junctions de solo lectura a los datos de D:\Grand Theft Auto San Andreas;
el gta_sa.exe original solo se lee como fuente de datos). Toma D:\Original\gta_lock y no corre si hay un gta_sa.exe o un ejecutable independiente vivo; solo mata su propio proceso.
Uso: pwsh -File ejecutar_crtb.ps1 -Etiqueta crtb1 [-Limite 25] [-Extra "--censo ..."]
La carpeta D:\Original\_fase8b_run se crea a mano (junctions anim audio data models movies text, eax.dll, ogg.dll, vorbis*.dll, stream.ini, userfiles, logs, gta-reversed.ini sin FastLoader).
#>
param([string]$Etiqueta = 'crtb', [int]$Limite = 25, [string]$Extra = '')
$ErrorActionPreference = 'Stop'
$aqui = $PSScriptRoot
$dest = 'D:\Original\_fase8b_run'
$sal = Join-Path $aqui ("ejecuciones\{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'f8b' | Set-Content 'D:\Original\gta_lock\owner_f8b'
try {
    while (Get-Process gta_sa, gta_sa_standalone, gta_reversed_exe -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $aqui 'salida\gta_sa_standalone.exe') (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $env:FASE8_ARGS = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    pwsh -NoProfile -File (Join-Path $aqui 'lanzar.ps1') -Exe (Join-Path $dest 'gta_sa_standalone.exe') -Dir $dest -EsperaS ($Limite + 30) | Tee-Object -FilePath (Join-Path $sal 'lanzador.txt')
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal



