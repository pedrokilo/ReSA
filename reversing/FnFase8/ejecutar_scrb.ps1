#requires -Version 7
# Variante de ejecutar_f8b.ps1 para la rama vivo-scrb: usa el ejecutable independiente enlazado en D:\Original\wv-scrb\_f8out y deja los registros en reversing\FnFase8\ejecuciones de este worktree.
# Uso: pwsh -File ejecutar_scrb.ps1 -Etiqueta s1 [-Limite 40] [-Extra "..."]
param([string]$Etiqueta = 's', [int]$Limite = 25, [string]$Extra = '')
$ErrorActionPreference = 'Stop'
$aqui = 'D:\Original\wv-fase8b\reversing\FnFase8'
$dest = 'D:\Original\_fase8b_run'
$sal = Join-Path 'D:\Original\wv-scrb\reversing\FnFase8\ejecuciones' ("{0}-{1}" -f $Etiqueta, (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Force $sal | Out-Null
while ($true) { try { New-Item -ItemType Directory 'D:\Original\gta_lock' -ErrorAction Stop | Out-Null; break } catch { Start-Sleep 5 } }
'scrb' | Set-Content 'D:\Original\gta_lock\owner_scrb'
try {
    while (Get-Process gta_sa, gta_sa_standalone, gta_reversed_exe -ErrorAction SilentlyContinue) { Start-Sleep 10 }
    Remove-Item (Join-Path $dest 'logs\*') -Force -ErrorAction SilentlyContinue
    Copy-Item 'D:\Original\wv-scrb\_f8out\gta_sa_standalone.exe' (Join-Path $dest 'gta_sa_standalone.exe') -Force
    $env:FASE8_ARGS = "--ruta-original `"D:\Grand Theft Auto San Andreas\gta-sa.exe`" --directorio-juego `"$dest`" --salida `"$sal`" --limite $Limite $Extra"
    pwsh -NoProfile -File (Join-Path $aqui 'lanzar.ps1') -Exe (Join-Path $dest 'gta_sa_standalone.exe') -Dir $dest -EsperaS ($Limite + 30) | Tee-Object -FilePath (Join-Path $sal 'lanzador.txt')
    Copy-Item (Join-Path $dest 'gta-reversed.ini') $sal -ErrorAction SilentlyContinue
    Copy-Item (Join-Path $dest 'logs\*') $sal -ErrorAction SilentlyContinue
} finally {
    Remove-Item -Recurse -Force 'D:\Original\gta_lock' -ErrorAction SilentlyContinue
}
$sal
