#requires -Version 7
<#
Crea (o recrea) la carpeta de pruebas del exe propio: E:\JUEGOS\GTA San Andreas - CVehicle Test - FASE8
 - Directorios de datos del juego como JUNCTIONS hacia la copia de pruebas (no se copia nada pesado, solo lectura de hecho):
   anim audio data models movies text
 - Copias de lo pequeno que el juego necesita al lado del exe: eax.dll ogg.dll vorbis*.dll stream.ini y userfiles (partidas propias de esta carpeta).
 - NO lleva dinput8.dll ni scripts\ (asi el cargador de ASI no inyecta gta_reversed.asi una segunda vez) ni gta_sa.exe.
 - gta-reversed.ini propio: FastLoader desactivado (arranque normal por menu) y SaveDirectoryPath en esta carpeta.
No modifica la copia de pruebas (E:\JUEGOS\GTA San Andreas - CVehicle Test) ni jamas toca E:\JUEGOS\GTA San Andreas.
Uso: pwsh -File crear_carpeta_fase8.ps1 [-FastLoader] [-Quitar]
#>
param([switch]$FastLoader, [switch]$Quitar)
$ErrorActionPreference = 'Stop'
$origen = 'E:\JUEGOS\GTA San Andreas - CVehicle Test'
$dest = 'E:\JUEGOS\GTA San Andreas - CVehicle Test - FASE8'
if ($dest -ieq 'E:\JUEGOS\GTA San Andreas') { throw 'destino prohibido' }
function Quitar-Carpeta {
    if (-not (Test-Path -LiteralPath $dest)) { return }
    foreach ($d in 'anim','audio','data','models','movies','text') {   # quitar las junctions SIN tocar su destino
        $j = Join-Path $dest $d
        if (Test-Path -LiteralPath $j) { cmd /c rmdir "`"$j`"" | Out-Null }
    }
    Remove-Item -LiteralPath $dest -Recurse -Force
}
if ($Quitar) { Quitar-Carpeta; 'carpeta FASE8 eliminada (junctions quitadas sin tocar los datos)'; return }
Quitar-Carpeta
New-Item -ItemType Directory -Path $dest | Out-Null
foreach ($d in 'anim','audio','data','models','movies','text') {
    New-Item -ItemType Junction -Path (Join-Path $dest $d) -Target (Join-Path $origen $d) | Out-Null
}
foreach ($f in 'eax.dll','ogg.dll','vorbis.dll','vorbisFile.dll','vorbisHooked.dll','stream.ini') {
    Copy-Item -LiteralPath (Join-Path $origen $f) -Destination $dest
}
Copy-Item -LiteralPath (Join-Path $origen 'userfiles') -Destination (Join-Path $dest 'userfiles') -Recurse
New-Item -ItemType Directory -Path (Join-Path $dest 'logs') | Out-Null
$fl = if ($FastLoader) { "[FastLoader]`r`nEnable = true`r`nSaveGameToLoad = 2`r`nNoEAX = true`r`nNoNVidia = true`r`nNoLogo = true`r`nNoTitleOrIntro = true`r`nNoCopyright = true`r`nNoFading = true`r`nNoLoadingTune = true`r`nRenderAtAllTimes = true" } else { "[FastLoader]`r`nEnable = false" }
Set-Content -LiteralPath (Join-Path $dest 'gta-reversed.ini') -Encoding ascii -Value ($fl + "`r`n[Misc]`r`nSkipDeviceSelection = true`r`nSaveDirectoryPath = $dest\userfiles`r`nLoadSavesWithMismatchingVersion = true`r`n")
Get-ChildItem $dest | Select-Object Name, Attributes | Format-Table -AutoSize | Out-String
