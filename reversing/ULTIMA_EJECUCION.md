<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ejecución conjunta de los bancos de reversing

> Informe histórico de la ejecución conjunta del 22 de septiembre. La entrega más reciente es [HooksActivados, 01/10/2026](HooksActivados/README.md): 39 clases registradas, compilación y arranque al menú, sin comparaciones contra el original. La última validación diferencial completa sigue siendo [CConversationForPed, 30/09/2026](ConversationForPed/README.md), con 4.702 pares exactos. Consultar [la continuidad vigente](MEJORAS_DEL_PROCESO.md#continuidad-vigente) para el estado instalado. Esa validación individual no repite ni amplía los resultados de la suite histórica conservados aquí.

Fecha local: 2026-09-22. Rama: ReSA. Resultado: **9/9 bancos aprobados, 76.319 comparaciones dentro de GTA**.

Entre las observaciones posteriores figura la [recuperación del ratón al arrancar](MouseRecovery/README.md), que no repitió esta suite. Después se entregaron la [corrección del cierre](ShutdownLists/README.md), [CPedToPlayerConversations](PedToPlayerConversations/README.md) y [CConversationForPed](ConversationForPed/README.md), cada una con su evidencia y alcance propios.

Se abrió y cerró GTA para cada banco, utilizando exclusivamente `D:/JUEGOS/GTA San Andreas - CVehicle Test`. Después de cada ejecución se volvió a analizar su registro nuevo y se verificaron el hash de la evidencia, la restauración de ASI/PDB, el cierre del proceso y los hashes de las partidas. No se contaron informes históricos como ejecuciones nuevas.

| Banco | Comparaciones | Segundos | Evidencia nueva |
|---|---:|---:|---|
| CVehicle | 63 | 25.91 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cvehicle/20260922-195331-311323/comparisons.jsonl>) |
| C_PcSave | 2,088 | 22.93 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pcsave/20260922-195358-005252/comparisons.jsonl>) |
| CPlayerPed | 4,064 | 24.42 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-sprint/20260922-195421-741562/comparisons.jsonl>) |
| CPed — PositionAttachedPed | 361 | 28.38 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-attached/20260922-195448-219364/comparisons.jsonl>) |
| CPed — UpdatePosition | 2,534 | 21.99 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-updateposition/20260922-195512-664176/comparisons.jsonl>) |
| CWeaponEffects | 301 | 51.59 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-weaponeffects/20260922-195536-839942/comparisons.jsonl>) |
| FxManager_c | 720 | 22.68 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fxmanager/20260922-195627-053670/comparisons.jsonl>) |
| CBaseModelInfo | 65,536 | 24.98 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-basemodelinfo/20260922-195650-434515/comparisons.jsonl>) |
| RpAnimBlend | 652 | 28.87 | [Registro](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-rpanimblend/20260922-195716-322699/comparisons.jsonl>) |

Tiempo acumulado de los ejecutores: 251.75 segundos. Todos finalizaron con código 0 y sus registros contienen el evento de finalización requerido. No hubo diferencias ni errores/aserciones rechazados por los analizadores.

ASI y PDB restaurados tras cada banco. Partidas originales y de prueba sin cambios. ASI normal conservado: `6999d5daf6a7e4a9051ed773dda66dcd0d66e8572b1211bcbf9059b58c8b5176`.

[Resumen conjunto con hashes y resultados de cada banco](artifacts/suite-20260922-195327-597160/summary.json). En esa carpeta también están las salidas de consola de los nueve ejecutores.

## Alcance

Se repitieron las sondas archivadas de cada trabajo. Esto valida otra vez los casos documentados de esas versiones; no es una prueba conjunta de todos los cambios sobre el ASI normal actual ni valida todas las funciones de las clases. Audio y Global/SurfaceInfos_c se mantuvieron excluidos. No hubo revisión visual ni sesión prolongada. El caso pcsave-isolated no se repitió: la misma función se probó con el banco pcsave dentro del motor.

La campaña integral sigue prevista para cuando todas las clases están completas. Esta ejecución fue solicitada expresamente por el usuario para repetir los bancos existentes.

Para ejecutar el catálogo vigente: `PROBAR.cmd --all`, o `python reversing/run.py --all` para evitar la pausa interactiva. Actualmente incluye 45 casos de motor, por lo que ya no equivale a repetir sólo estos nueve bancos; el caso aislado se omite en `--all`. Se mantiene un solo PROBAR.cmd. En la repetición histórica documentada aquí no se modificaron hooks.csv, ReversedClasses.md ni código del juego. Sin commit ni push.
