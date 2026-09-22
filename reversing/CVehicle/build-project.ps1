$ErrorActionPreference = 'Stop'
$project = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$env:CONAN_HOME = Join-Path $PSScriptRoot 'conan-cache'
$toolScripts = Join-Path $PSScriptRoot 'tooling\Scripts'
$cmake = Get-ChildItem -Path (Join-Path $env:CONAN_HOME 'p\cmake*\p\bin\cmake.exe') -File | Select-Object -First 1
$nasm = Get-ChildItem -Path (Join-Path $env:CONAN_HOME 'p\nasm*\p\bin\nasm.exe') -File | Select-Object -First 1
if (!$cmake -or !$nasm) { throw 'Faltan herramientas de la cache local de Conan.' }
$env:PATH = "$toolScripts;$($cmake.DirectoryName);$($nasm.DirectoryName);C:\Program Files (x86)\Microsoft Visual Studio\Installer;$env:PATH"
Push-Location $project
try {
    & (Join-Path $toolScripts 'python.exe') setup.py --build
    if ($LASTEXITCODE -ne 0) { throw "La compilacion termino con codigo $LASTEXITCODE." }
} finally {
    Pop-Location
}

