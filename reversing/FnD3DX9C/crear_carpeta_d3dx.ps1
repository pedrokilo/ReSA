#requires -Version 7
<#
FnD3DX9C: crea la carpeta de pruebas D:\Original\_d3dx_run para el exe independiente: junctions de solo lectura a los datos de D:\Grand Theft Auto San Andreas
(anim audio data models movies text), copias de eax.dll ogg.dll vorbis*.dll stream.ini, userfiles propios y un gta-reversed.ini con FastLoader. NO lleva d3dx9_43.dll, gta-sa.exe,
dinput8.dll ni scripts\. Con -ConDll copia ademas d3dx9_43.dll desde el cab del SDK (Redist\Jun2010_d3dx9_43_x86.cab) comprobando hash y firma (prueba de la opcion de redistribucion).
Uso: pwsh -File crear_carpeta_d3dx.ps1 [-ConDll] [-Quitar]
#>
param([switch]$ConDll, [switch]$Quitar)
$ErrorActionPreference = 'Stop'
$origen = 'D:\Grand Theft Auto San Andreas'
$ref = 'D:\Original\_fase8b_run'   # solo se LEE (eax.dll etc.)
$dest = 'D:\Original\_d3dx_run'
if ($dest -ine 'D:\Original\_d3dx_run') { throw 'destino inesperado' }
function Quitar-Carpeta {
    if (-not (Test-Path -LiteralPath $dest)) { return }
    foreach ($d in 'anim','audio','data','models','movies','text') {
        $j = Join-Path $dest $d
        if (Test-Path -LiteralPath $j) { cmd /c rmdir "`"$j`"" | Out-Null }   # quita la junction sin tocar su destino
    }
    Remove-Item -LiteralPath $dest -Recurse -Force
}
if ($Quitar) { Quitar-Carpeta; 'carpeta _d3dx_run eliminada (junctions quitadas sin tocar los datos)'; return }
Quitar-Carpeta
New-Item -ItemType Directory -Path $dest | Out-Null
foreach ($d in 'anim','audio','data','models','movies','text') { New-Item -ItemType Junction -Path (Join-Path $dest $d) -Target (Join-Path $origen $d) | Out-Null }
foreach ($f in 'eax.dll','ogg.dll','vorbis.dll','vorbisFile.dll','vorbisHooked.dll','stream.ini') { Copy-Item -LiteralPath (Join-Path $ref $f) -Destination $dest }
Copy-Item -LiteralPath (Join-Path $ref 'userfiles') -Destination (Join-Path $dest 'userfiles') -Recurse
New-Item -ItemType Directory -Path (Join-Path $dest 'logs') | Out-Null
$ini = Get-Content -LiteralPath (Join-Path $ref 'gta-reversed.ini') -Raw
$ini = $ini -replace [regex]::Escape('D:\Original\_fase8b_run\userfiles'), "$dest\userfiles"
Set-Content -LiteralPath (Join-Path $dest 'gta-reversed.ini') -Encoding ascii -Value $ini
if ($ConDll) {
    # Redist oficial del SDK de junio de 2010 (Microsoft DirectX SDK, carpeta Redist): d3dx9_43.dll y, porque la primera la carga al ensamblar/compilar, D3DCompiler_43.dll
    $sdk = 'C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Redist'
    foreach ($par in @(@('Jun2010_d3dx9_43_x86.cab', 'd3dx9_43.dll'), @('Jun2010_D3DCompiler_43_x86.cab', 'D3DCompiler_43.dll'))) {
        $tmp = Join-Path $env:TEMP ('d3dxcab_' + [guid]::NewGuid().ToString('N'))
        New-Item -ItemType Directory -Path $tmp | Out-Null
        & "$env:windir\System32\expand.exe" (Join-Path $sdk $par[0]) "-F:$($par[1])" $tmp | Out-Null
        $dll = Join-Path $tmp $par[1]
        $firma = Get-AuthenticodeSignature $dll
        $hash = (Get-FileHash $dll -Algorithm SHA256).Hash
        if ($firma.Status -ne 'Valid' -or $firma.SignerCertificate.Subject -notmatch 'Microsoft Corporation') { throw "firma no valida en $($par[1]): $($firma.Status)" }
        Copy-Item $dll (Join-Path $dest $par[1])
        Remove-Item -Recurse -Force $tmp
        "$($par[1]) copiada desde $($par[0]): SHA256 $hash firma $($firma.Status) ($($firma.SignerCertificate.Subject)) version $((Get-Item (Join-Path $dest $par[1])).VersionInfo.FileVersion)"
    }
}
Get-ChildItem $dest | Select-Object Name, Attributes | Format-Table -AutoSize | Out-String
