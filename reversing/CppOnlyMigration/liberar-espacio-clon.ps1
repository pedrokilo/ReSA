# Libera espacio en el clon de pruebas. Sin -Ejecutar solo muestra lo que borraria.
#   powershell -File liberar-espacio-clon.ps1             (vista previa)
#   powershell -File liberar-espacio-clon.ps1 -Ejecutar   (borra de forma permanente, sin respaldo)
#
# Borra solo artefactos que build_probe.py y runtime_runner.py regeneran en cada banco:
#   _baseline\*            copias de sondas (.asi/.pdb) de bancos anteriores
#   resultados-*\          informes crudos de corridas (lo publicado esta comprimido en el repo)
#   *-runtime.jsonl/.start trazas crudas de corridas anteriores
# NO toca el juego (audio, models, data...), ni scripts\ (asi/pdb activos), ni userfiles\, ni gta_sa.exe.
param([switch]$Ejecutar)

$clon = 'E:\JUEGOS\GTA San Andreas - CVehicle Test'
if ($clon -ne 'E:\JUEGOS\GTA San Andreas - CVehicle Test' -or $clon -like '*Andreas') { throw 'Ruta inesperada.' }
if (-not (Test-Path (Join-Path $clon 'gta_sa.exe'))) { throw 'No es el clon de pruebas (falta gta_sa.exe).' }
if (Get-Process -Name 'gta_sa' -ErrorAction SilentlyContinue) { throw 'Cierra gta_sa.exe antes de limpiar.' }

$objetivos = @()
$objetivos += Get-ChildItem (Join-Path $clon '_baseline') -Force -ErrorAction SilentlyContinue
$objetivos += Get-ChildItem $clon -Directory -Force -Filter 'resultados-*'
$objetivos += Get-ChildItem $clon -File -Force | Where-Object { $_.Name -like '*-runtime.jsonl' -or $_.Name -like '*-runtime.start' -or $_.Name -like '*-harness.jsonl' }

$total = 0
foreach ($o in $objetivos) {
    if ($o.PSIsContainer) { $bytes = (Get-ChildItem $o.FullName -Recurse -File -Force -ErrorAction SilentlyContinue | Measure-Object Length -Sum).Sum }
    else { $bytes = $o.Length }
    $total += $bytes
}
'{0} elementos, {1:N2} GB' -f $objetivos.Count, ($total / 1GB)
$antes = (Get-PSDrive E).Free / 1GB
'Libre en E: ahora {0:N2} GB' -f $antes

if (-not $Ejecutar) { 'Vista previa: no se borro nada. Usa -Ejecutar para borrar.'; return }

foreach ($o in $objetivos) { Remove-Item -LiteralPath $o.FullName -Recurse -Force -ErrorAction Continue }
'Libre en E: ahora {0:N2} GB' -f ((Get-PSDrive E).Free / 1GB)
