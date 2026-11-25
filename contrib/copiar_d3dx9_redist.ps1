#requires -Version 7
<#
Copia junto al ejecutable los DOS DLL del Runtime de usuario final de DirectX (junio de 2010) que el independiente solo necesita como RESPALDO (vs_1_1, shaders que el
ensamblador propio no cubre, funciones de compilacion HLSL): d3dx9_43.dll y D3DCompiler_43.dll (d3dx9_43.dll carga la segunda al ensamblar o compilar).
En una partida normal con una GPU actual (vs_2_x) NO se cargan: el independiente ensambla y calcula con C++ propio (docs/RenderWareExe/d3dx9-enlace.md).
Origen: los cab oficiales de Microsoft (Jun2010_d3dx9_43_x86.cab, Jun2010_D3DCompiler_43_x86.cab) de la carpeta Redist del DirectX SDK (June 2010) instalado, o de la
carpeta Redistributables\DirectX que trae el propio juego. NO descarga nada de internet. Se comprueba la firma Authenticode (Microsoft Corporation) y se imprime el SHA256.
Uso: pwsh -File contrib/copiar_d3dx9_redist.ps1 -Destino <carpeta del exe> [-Origen <carpeta con los cab>]
#>
param(
    [Parameter(Mandatory)][string]$Destino,
    [string]$Origen
)
$ErrorActionPreference = 'Stop'
if (-not $Origen) {
    $candidatos = @('C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Redist', 'D:\Grand Theft Auto San Andreas\Redistributables\DirectX')
    $Origen = $candidatos | Where-Object { Test-Path (Join-Path $_ 'Jun2010_d3dx9_43_x86.cab') } | Select-Object -First 1
    if (-not $Origen) { throw 'No se encontro Jun2010_d3dx9_43_x86.cab; indique -Origen con la carpeta que contiene los cab de DirectX (junio de 2010).' }
}
if (-not (Test-Path -LiteralPath $Destino -PathType Container)) { throw "No existe la carpeta de destino: $Destino" }
foreach ($par in @(@('Jun2010_d3dx9_43_x86.cab', 'd3dx9_43.dll'), @('Jun2010_D3DCompiler_43_x86.cab', 'D3DCompiler_43.dll'))) {
    $cab = Join-Path $Origen $par[0]
    $tmp = Join-Path $env:TEMP ('d3dxcab_' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $tmp | Out-Null
    try {
        & "$env:windir\System32\expand.exe" $cab "-F:$($par[1])" $tmp | Out-Null
        $dll = Join-Path $tmp $par[1]
        if (-not (Test-Path $dll)) { throw "El cab $cab no contiene $($par[1])" }
        $firma = Get-AuthenticodeSignature $dll
        if ($firma.Status -ne 'Valid' -or $firma.SignerCertificate.Subject -notmatch 'Microsoft Corporation') { throw "Firma no valida en $($par[1]): $($firma.Status)" }
        $hash = (Get-FileHash $dll -Algorithm SHA256).Hash
        Copy-Item $dll (Join-Path $Destino $par[1]) -Force
        '{0}: copiada a {1} desde {2}; SHA256 {3}; firma {4} ({5}); version {6}' -f $par[1], $Destino, $par[0], $hash, $firma.Status, $firma.SignerCertificate.Subject, (Get-Item (Join-Path $Destino $par[1])).VersionInfo.FileVersion
    } finally {
        Remove-Item -Recurse -Force $tmp -ErrorAction SilentlyContinue
    }
}
