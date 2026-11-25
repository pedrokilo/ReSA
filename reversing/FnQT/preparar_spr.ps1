#requires -Version 7
<#
Prepara D:\Original\_spr_run: carpeta de ejecucion propia del standalone para la prueba de pistas de usuario (CAEMFDecoder / Media Foundation).
Junctions de solo lectura a los datos de D:\Grand Theft Auto San Andreas (como _juego_run), partida 2 copiada de _juego_run\userfiles\GTASAsf2.b.orig,
y una pista de usuario de prueba (.mp3 de 15,5 s) registrada a mano en sa-utrax.dat / sa-ufiles.dat dentro de userfiles (documentos redirigidos con SaveDirectoryPath).
Uso: pwsh -File preparar_ejecucion.ps1 -Mp3 <ruta del .mp3 de prueba>
#>
param([Parameter(Mandatory)][string]$Mp3, [int]$Tipo = 4)
$ErrorActionPreference = 'Stop'
$src = 'D:\Original\_juego_run'
$dst = 'D:\Original\_spr_run'
$juego = 'D:\Grand Theft Auto San Andreas'
New-Item -ItemType Directory -Force $dst, "$dst\logs", "$dst\userfiles", "$dst\userfiles\User Tracks", "$dst\userfiles\Gallery" | Out-Null
foreach ($d in 'anim', 'audio', 'data', 'models', 'movies', 'text') {
    if (-not (Test-Path "$dst\$d")) { New-Item -ItemType Junction -Path "$dst\$d" -Target "$juego\$d" | Out-Null }
}
foreach ($f in 'eax.dll', 'ogg.dll', 'vorbis.dll', 'vorbisFile.dll', 'vorbisHooked.dll', 'stream.ini') { Copy-Item "$src\$f" "$dst\$f" -Force }
Copy-Item "$src\userfiles\GTASAsf2.b.orig" "$dst\userfiles\GTASAsf2.b.orig" -Force
Copy-Item "$src\userfiles\GTASAsf2.b.orig" "$dst\userfiles\GTASAsf2.b" -Force
Copy-Item "$src\userfiles\gta_sa.set" "$dst\userfiles\gta_sa.set" -Force
(Get-Content "$src\gta-reversed.ini") -replace [regex]::Escape('D:\Original\_juego_run\userfiles'), "$dst\userfiles" | Set-Content "$dst\gta-reversed.ini"
$pista = "$dst\userfiles\User Tracks\prueba" + [IO.Path]::GetExtension($Mp3)
Copy-Item $Mp3 $pista -Force
# sa-ufiles.dat: rutas concatenadas sin separador; sa-utrax.dat: registros de 12 bytes {inicio de la ruta, longitud, tipo (4 = AUDIO_FILE_TYPE_QUICKTIME, el de ".mp3")}
$ruta = [Text.Encoding]::Default.GetBytes($pista)
[IO.File]::WriteAllBytes("$dst\userfiles\sa-ufiles.dat", $ruta)
$reg = [byte[]]::new(12)
[BitConverter]::GetBytes([uint32]0).CopyTo($reg, 0)
[BitConverter]::GetBytes([uint32]$ruta.Length).CopyTo($reg, 4)
[BitConverter]::GetBytes([uint32]$Tipo).CopyTo($reg, 8)
[IO.File]::WriteAllBytes("$dst\userfiles\sa-utrax.dat", $reg)
"preparado $dst con la pista $pista ($($ruta.Length) bytes de ruta)"

