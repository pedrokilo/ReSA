<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Peatones: desplazamiento de animación sin inicializar

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

## Defecto y corrección

Los 18 avisos de la integración anterior contenían una altura de `-431602080`, el valor float de `0xCDCDCDCD`. Las nuevas trazas registraron saltos instantáneos desde coordenadas normales, con velocidad pequeña, dentro de `CPed::ProcessControl`; la captura más precisa los acotó a `CPedIntelligence::Process` → `CTaskManager::ManageTasks`.

El constructor con parámetros de [CTaskSimpleRunNamedAnim](../../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (`0x61A990`) omitía tres inicializaciones del original:

- `m_vecOffsetAtEnd`: el original escribe tres ceros.
- `m_bOffsetAvailable`: el original desactiva la bandera `0x40`.
- `m_pAnimHierarchy`: el original escribe nulo cuando no encuentra el bloque.

El `ProcessPed` original (`0x61BF20`) comprueba primero `0x40` y llama a `OffsetPedPosition` (`0x61AB00`). Por eso una tarea construida sobre memoria `0xCD` podía aplicar inmediatamente un desplazamiento enorme. El banco reprodujo este mecanismo con objetos privados dentro del motor: el original mantuvo la posición y el constructor anterior provocó el salto. La corrección inicializa esos tres campos; conserva el constructor predeterminado, el desplazamiento legítimo de las animaciones y el aviso de CWorld.

## Comparación dentro del motor

**8.832 pares aprobados**, sin diferencias en los resultados comparados:

- 8.192 constructores: 256 patrones previos de memoria × 16 combinaciones de cuatro argumentos booleanos × bloque existente/ausente. Comparación semántica de nombres, temporizador, punteros, flags, tiempo, mezcla y desplazamiento. Se excluyen vtable, padding, colas de cadenas y `m_nAnimId`, que el original no inicializa aquí.
- 128 ejecuciones de `ProcessPed` original después de construir cada variante, con una tarea terminada y desplazamiento final habilitado. Cuatro patrones de memoria y 32 orientaciones; debe conservar la posición si no hay un desplazamiento pendiente.
- 512 aplicaciones de `OffsetPedPosition`: desplazamiento cero y no cero, distintas orientaciones y posiciones. Error máximo observado **0**, tolerancia `1e-5`.

El control anterior a la corrección registró **8.160 diferencias de inicialización y 64 de posición**; los 512 pares de aplicación de desplazamiento ya coincidían. No se ocultan esas diferencias ni se cuentan como aprobadas.

Original y candidato se ejecutan alternando sus hooks. Los contadores comprueban cero llamadas al candidato durante el constructor y desplazamiento originales, y una por llamada candidata. Objetos independientes, matriz privada y clump RenderWare vacío: la ruta de actualización no tiene huesos que modificar. Se comprueba la restauración del jugador y de los hooks. Sin exclusiones de hooks. No valida la reproducción visual de todas las animaciones ni toda la clase.

Evidencia anterior: `resultados-namedanim-init/20260923-145452-490547`. Aprobación corregida: `resultados-namedanim-init/20260923-150054-230081`, ambas bajo la copia de pruebas. Hashes y restauración en [validation-summary.json](validation-summary.json).

## Repetición

```powershell
python reversing/run.py --case namedanim-init
```

Disponible en el único `PROBAR.cmd` y en `--all`. [run_local.py](run_local.py) reutiliza `runtime_runner.py`, instala temporalmente la sonda y restaura ASI/PDB e INI, verificando las partidas. [build_probe.py](build_probe.py) recompila a staging y restaura fuentes y binarios normales en `finally`; conserva el manifiesto local de la sonda. `--diagnose-baseline` solo sirve con una sonda compilada desde el código defectuoso, no con la versión corregida.

[build_trace.py](build_trace.py) y [run_trace.py](run_trace.py) conservan el diagnóstico natural, separado del banco diferencial. La captura termina a 7.200 actualizaciones o después de reunir transiciones inválidas a partir de 1.200. Su aprobación significa captura completada, no equivalencia ni ausencia de errores. [symbolize.py](symbolize.py) necesita el ASI/PDB exacto de esa captura.

Las pasadas naturales anteriores fueron variables: una no reprodujo, otra capturó 74 transiciones y 14 avisos, y otra acotó las transiciones a ManageTasks. Una captura posterior específica de la función tampoco reprodujo el fallo; no se conoce el nombre de la animación de los 18 avisos originales. La atribución al defecto se apoya en el patrón de memoria, la ruta observada, el desensamblado y la reproducción determinista del salto, no en una captura directa de aquella animación.

## Alcance

ASI normal Debug x86 instalado: **6835a6eebc877f597a0d1cffe3992b7e8889cc0ddbff80446b4928d7978cd311**. Respaldo `_baseline/before-fallen-peds-20260923-150204`. Fuentes y binarios verificados; sin instrumentación temporal.

Integración final sin exclusiones: **120,64 segundos, 3.609 fotogramas**, cero avisos de peatones fuera del mapa, cero fallos de CEntryExit y máximo **402/512** entradas. Evidencia: `Unlocks/artifacts/current-20260923-150550`. La primera integración, también sin avisos, solo avanzó 815 fotogramas; se repitió exigiendo 3.600 como mínimo. No sumar las repeticiones como nueva cobertura diferencial.

```powershell
python reversing/Unlocks/run_current_build.py --fallen-peds --entryexit-pool --seconds 120 --min-frames 3600 --timeout 600
```

No se observaron cierre inesperado, aserción ni RTC. El log final conserva avisos de timecyc y skimmer; no hubo avisos de IK en esta muestra, lo cual no acredita su reparación. Partidas conservadas por hash y ambos INI restaurados: `FastLoader=false`, `SaveGameToLoad=-2`.

Solo se usa `D:\JUEGOS\GTA San Andreas - CVehicle Test`. No cambia `hooks.csv`, no completa CTaskSimpleRunNamedAnim, no modifica RpAnimBlend 43/43 ni retoma los 86 bloqueos aplazados. CEntryExit conserva su reserva inicial de 512. Los avisos de IK y de datos se abordaron después en [otro informe](../IKDataWarnings/README.md), que también identifica el ASI normal posterior. Las pruebas finitas no certifican estabilidad general ni descartan otras causas de peatones fuera del mapa.
