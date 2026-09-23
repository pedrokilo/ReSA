# Datos privados del banco. No se accede a la carpeta de música del usuario.
$ErrorActionPreference = 'Stop'
$fixtureRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'artifacts\fixtures'))
$expectedRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'artifacts')) + '\'
if (-not $fixtureRoot.StartsWith($expectedRoot, [StringComparison]::OrdinalIgnoreCase)) { throw 'Ruta fuera del banco' }
function Write-Fixture([string]$relative) {
    $target = Join-Path $fixtureRoot $relative
    [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target)) | Out-Null
    if (-not [IO.File]::Exists($target)) { [IO.File]::WriteAllBytes($target, [byte[]]@(0)) }
}
foreach ($index in 0..7) { [IO.Directory]::CreateDirectory((Join-Path $fixtureRoot "case$index")) | Out-Null }
foreach ($index in 0..6) { [IO.Directory]::CreateDirectory((Join-Path $fixtureRoot "case$index\User Tracks")) | Out-Null }
Write-Fixture 'case1\User Tracks\single.wav'
foreach ($name in @('a.wav','b.ogg','c.mp3','d.wma','e.wmv','f.aac','g.m4a','x.txt')) { Write-Fixture "case2\User Tracks\$name" }
$nested = 'case3\User Tracks'
foreach ($index in 0..16) { Write-Fixture "$nested\t$index.wav"; $nested += '\d' }
foreach ($name in @('.ignore.wav','ok.wav','.hidden\deep.ogg')) { Write-Fixture "case4\User Tracks\$name" }
foreach ($name in @('ONE.WAV','two.OGG','README','no.extension')) { Write-Fixture "case5\User Tracks\$name" }
Write-Fixture 'link-target.wav'
Write-Fixture 'linked-folder\nested.ogg'
$shortcutShell = New-Object -ComObject WScript.Shell
foreach ($pair in @(@('file.LNK','link-target.wav'),@('folder.lnk','linked-folder'))) {
    $link = $shortcutShell.CreateShortcut((Join-Path $fixtureRoot "case6\User Tracks\$($pair[0])"))
    $link.TargetPath = Join-Path $fixtureRoot $pair[1]
    $link.Save()
}
Write-Output "Fixtures preparados en $fixtureRoot"
