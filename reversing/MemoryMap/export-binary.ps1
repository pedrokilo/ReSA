param(
    [Parameter(Mandatory = $true)][string]$GhidraDirectory,
    [string]$Exe = 'D:\Grand Theft Auto San Andreas\gta-sa.exe',
    [string]$Python = 'python',
    [switch]$Reanalyze
)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$exportDirectory = Join-Path $repoRoot 'build\binary-map'
$projectDirectory = Join-Path $repoRoot 'build\ghidra-memory-map'
New-Item -ItemType Directory -Path $exportDirectory,$projectDirectory -Force | Out-Null
& $Python (Join-Path $repoRoot 'contrib\prepare-binary-map.py') --root $repoRoot --exe $Exe --output $exportDirectory
if ($LASTEXITCODE -ne 0) { throw 'No se prepararon las entradas candidatas.' }
$headless = Join-Path $GhidraDirectory 'support\analyzeHeadless.bat'
$analysisLog = Join-Path $exportDirectory 'ghidra-analysis.log'
if (!(Test-Path -LiteralPath (Join-Path $projectDirectory 'ReSA10.gpr'))) {
    & $headless $projectDirectory ReSA10 -import $Exe -processor 'x86:LE:32:default' -scriptPath $PSScriptRoot -preScript SeedBinaryFunctions.java (Join-Path $exportDirectory 'seeds.txt') -postScript ExportMemoryInventory.java $exportDirectory -postScript ExportBinaryCoverage.java $exportDirectory -max-cpu 4 -log $analysisLog
} elseif ($Reanalyze) {
    & $headless $projectDirectory ReSA10 -process (Split-Path $Exe -Leaf) -scriptPath $PSScriptRoot -preScript SeedBinaryFunctions.java (Join-Path $exportDirectory 'seeds.txt') -postScript ExportMemoryInventory.java $exportDirectory -postScript ExportBinaryCoverage.java $exportDirectory -max-cpu 4 -log $analysisLog
} else {
    & $headless $projectDirectory ReSA10 -process (Split-Path $Exe -Leaf) -noanalysis -readOnly -scriptPath $PSScriptRoot -postScript ExportMemoryInventory.java $exportDirectory -postScript ExportBinaryCoverage.java $exportDirectory -log (Join-Path $exportDirectory 'ghidra-export.log')
}
if ($LASTEXITCODE -ne 0) { throw 'Falló Ghidra. No se publicará un inventario incompleto.' }
& $Python (Join-Path $repoRoot 'contrib\build-binary-map.py') --root $repoRoot --exe $Exe --export $exportDirectory
if ($LASTEXITCODE -ne 0) { throw 'No se generó el mapa del binario.' }
$candidateCsv = Join-Path $repoRoot 'docs\MemoryMap\binary_entry_candidates.csv'
$candidatesToReview = @(Import-Csv -LiteralPath $candidateCsv | Where-Object { !$_.containing_function_va })
if ($candidatesToReview.Count -gt 0) {
    $remainingSeeds = Join-Path $exportDirectory 'unresolved-seeds.txt'
    $candidatesToReview | Select-Object -ExpandProperty va | Set-Content -LiteralPath $remainingSeeds -Encoding ASCII
    & $headless $projectDirectory ReSA10 -process (Split-Path $Exe -Leaf) -noanalysis -scriptPath $PSScriptRoot -postScript ResolveSeedCandidates.java $remainingSeeds (Join-Path $exportDirectory 'seed-resolution.csv') -postScript ExportMemoryInventory.java $exportDirectory -postScript ExportBinaryCoverage.java $exportDirectory -log (Join-Path $exportDirectory 'ghidra-seed-resolution.log')
    if ($LASTEXITCODE -ne 0) { throw 'Falló la revisión de entradas pendientes.' }
    & $Python (Join-Path $repoRoot 'contrib\build-binary-map.py') --root $repoRoot --exe $Exe --export $exportDirectory
    if ($LASTEXITCODE -ne 0) { throw 'No se generó el mapa después de revisar las entradas.' }
}
