$ErrorActionPreference = 'Stop'
# Todas las rutas se derivan de la ubicacion de este script, nunca de una unidad fija.
$project = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$env:CONAN_HOME = Join-Path $PSScriptRoot 'conan-cache'
$toolScripts = Join-Path $PSScriptRoot 'tooling\Scripts'
$python = Join-Path $toolScripts 'python.exe'
$cmake = Get-ChildItem -Path (Join-Path $env:CONAN_HOME 'p\cmake*\p\bin\cmake.exe') -File | Select-Object -First 1
$nasm = Get-ChildItem -Path (Join-Path $env:CONAN_HOME 'p\nasm*\p\bin\nasm.exe') -File | Select-Object -First 1
if (!$cmake -or !$nasm) { throw 'Faltan herramientas de la cache local de Conan.' }

# setup.py invoca Conan como modulo del python del venv (reubicable), no via conan.exe.

# Un build/ configurado desde otra ruta (p. ej. otra unidad) no se puede reutilizar: CMake rechaza
# un CMakeCache.txt cuyo CMAKE_HOME_DIRECTORY no coincide. Se descarta solo el estado de configuracion.
$buildDir = Join-Path $project 'build'
$cache = Join-Path $buildDir 'CMakeCache.txt'
if (Test-Path $cache) {
    $home_ = (Select-String -Path $cache -Pattern '^CMAKE_HOME_DIRECTORY:INTERNAL=(.*)$').Matches.Groups[1].Value
    if ($home_ -and ((Resolve-Path $home_ -ErrorAction SilentlyContinue).Path -ne $project)) {
        Write-Host "build/ configurado desde '$home_'; se regenera para '$project'."
        Remove-Item -Force $cache
        Remove-Item -Recurse -Force (Join-Path $buildDir 'CMakeFiles') -ErrorAction SilentlyContinue
        Remove-Item -Recurse -Force (Join-Path $buildDir 'generators') -ErrorAction SilentlyContinue
    }
}

$env:PATH = "$($cmake.DirectoryName);$($nasm.DirectoryName);C:\Program Files (x86)\Microsoft Visual Studio\Installer;$env:PATH"
Push-Location $project
try {
    & $python setup.py --build
    if ($LASTEXITCODE -ne 0) { throw "La compilacion termino con codigo $LASTEXITCODE." }
} finally {
    Pop-Location
}
