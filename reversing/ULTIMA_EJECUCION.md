# Ejecuci?n conjunta de los bancos de reversing

Fecha local: 2026-09-22. Rama: PGTASA. Resultado: **9/9 bancos aprobados, 76.319 comparaciones dentro de GTA**.

Se abri? y cerr? GTA para cada banco, utilizando exclusivamente `E:/JUEGOS/GTA San Andreas - CVehicle Test`. Despu?s de cada ejecuci?n se volvi? a analizar su registro nuevo y se verificaron el hash de la evidencia, la restauraci?n de ASI/PDB, el cierre del proceso y los hashes de las partidas. No se contaron informes hist?ricos como ejecuciones nuevas.

| Banco | Comparaciones | Segundos | Evidencia nueva |
|---|---:|---:|---|
| CVehicle | 63 | 25.91 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cvehicle/20260922-195331-311323/comparisons.jsonl>) |
| C_PcSave | 2,088 | 22.93 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pcsave/20260922-195358-005252/comparisons.jsonl>) |
| CPlayerPed | 4,064 | 24.42 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-sprint/20260922-195421-741562/comparisons.jsonl>) |
| CPed ? PositionAttachedPed | 361 | 28.38 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-attached/20260922-195448-219364/comparisons.jsonl>) |
| CPed ? UpdatePosition | 2,534 | 21.99 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-updateposition/20260922-195512-664176/comparisons.jsonl>) |
| CWeaponEffects | 301 | 51.59 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weaponeffects/20260922-195536-839942/comparisons.jsonl>) |
| FxManager_c | 720 | 22.68 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fxmanager/20260922-195627-053670/comparisons.jsonl>) |
| CBaseModelInfo | 65,536 | 24.98 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-basemodelinfo/20260922-195650-434515/comparisons.jsonl>) |
| RpAnimBlend | 652 | 28.87 | [Log](<E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-rpanimblend/20260922-195716-322699/comparisons.jsonl>) |

Tiempo acumulado de los ejecutores: 251.75 segundos. Todos finalizaron con c?digo 0 y sus registros contienen el evento de finalizaci?n requerido. No hubo diferencias ni errores/aserciones rechazados por los analizadores.

ASI y PDB restaurados tras cada banco. Partidas originales y de prueba sin cambios. ASI normal conservado: `6999d5daf6a7e4a9051ed773dda66dcd0d66e8572b1211bcbf9059b58c8b5176`.

[Resumen conjunto con hashes y resultados de cada banco](artifacts/suite-20260922-195327-597160/summary.json). En esa carpeta tambi?n est?n las salidas de consola de los nueve ejecutores.

## Alcance

Se repitieron las sondas archivadas de cada trabajo. Esto valida otra vez los casos documentados de esas versiones; no es una prueba conjunta de todos los cambios sobre el ASI normal actual ni valida todas las funciones de las clases. Audio y Global/SurfaceInfos_c se mantuvieron excluidos. No hubo revisi?n visual ni sesi?n prolongada. El caso pcsave-isolated no se repiti?: la misma funci?n se prob? con el banco pcsave dentro del motor.

La campa?a integral sigue prevista para cuando todas las clases est?n completas. Esta ejecuci?n fue solicitada expresamente por el usuario para repetir los bancos existentes.

Repetir: `PROBAR.cmd --all`, o `python reversing/run.py --all` para evitar la pausa interactiva. Se mantiene un solo PROBAR.cmd. No se modificaron hooks.csv, ReversedClasses.md ni c?digo del juego en esta repetici?n. Sin commit ni push.
