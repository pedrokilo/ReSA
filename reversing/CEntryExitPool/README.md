<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEntryExit: agotamiento de la reserva

> Esta entrega precede a la [corrección del desplazamiento de peatones](../FallenPeds/README.md). Los hashes y la integración de este informe corresponden al cierre de CEntryExit; el ASI normal más reciente figura en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

## Diagnóstico y corrección

El registro anterior contenía 13 fallos de asignación. El problema es intermitente: dos observaciones del ASI anterior alcanzaron 399/400 y 396/400 plazas sin errores; una traza posterior registró **444 altas, 25 bajas y 19 altas fallidas**, con las 400 plazas ocupadas. Las búsquedas de las entradas que se intentaron liberar no fallaron. Solo había dos duplicados de posición entre las altas aceptadas, presentes ya en la carga inicial; no se borraron entradas del mapa.

Se reprodujo esa secuencia de demanda con memoria independiente dentro del motor. **El original también falla en las mismas 19 altas con 400 plazas.** Con 512, ambas implementaciones aceptan toda la secuencia: 419 entradas quedan activas. Esto identifica un límite insuficiente para esa demanda; no demuestra ausencia de cualquier fuga en todo el juego.

[Init](../../source/game_sa/EntryExitManager.cpp) reserva ahora **512 entradas** desde el principio: 112 plazas adicionales, 6.832 bytes de objetos y estados (unos 6,7 KiB, sin contar metadatos del asignador). Es una **ampliación deliberada del límite original**, no un descubrimiento de que el original tuviera 512. No se redimensiona una reserva en uso: los punteros del árbol espacial, enlaces y pila conservan sus direcciones. No se suprime el mensaje de error ni se cambia el pool genérico.

## Banco automático

- **1.854 comprobaciones**: 1.833 resultados normalizados iguales y 21 diferencias de retorno previstas. Control con capacidad 400 y candidato con 512; secuencia de 469 operaciones, vaciado, reutilización completa y una petición adicional sobre cada pool lleno.
- Las 21 diferencias son el comportamiento previo de `FIX_BUGS`: devuelve **-1** al fallar; el original devuelve **0**, que también puede ser un índice válido. Esta tarea conserva esa corrección existente. No se declaran 1.854 igualdades exactas.
- Contadores del candidato: **0** durante las llamadas originales; **1.270 / 1.401** durante las pasadas candidatas de 400/512. Se comprueban liberación, índices válidos, capacidad, campos semánticos de las entradas y restauración del pool y árbol reales.
- Memoria independiente por pasada. Se normaliza el índice asignado y su generación porque el pool genérico ya tenía una política de cursor distinta del original; tampoco se compara el relleno de depuración ni padding.
- Dependencias reales, sin exclusiones de hooks. Árbol privado de nivel cero y entradas sin rotación, aleatoriedad ni enlaces: **no valida toda la geometría, las transiciones de interiores ni todas las funciones de CEntryExit**.

Direcciones contrastadas: `AddOne` 0x43FA00 y `DeleteOne` 0x43FD50; el original reserva 400 en `Init` 0x43F880. El ejecutable conserva SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Evidencia aprobada: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-entryexit-pool/20260923-141006-579167`. La pasada anterior se rechazó porque el banco todavía exigía igualdad en el retorno de error; no se cambió el código de producción para acomodar esa expectativa.

```powershell
python reversing/run.py --case entryexit-pool
```

Disponible en el único `PROBAR.cmd` y en `--all`. [run_local.py](run_local.py) usa [runtime_runner.py](../runtime_runner.py), cierra su propio proceso y restaura ASI/PDB, INI y partidas. La sonda archivada tiene SHA-256 `12a1673274333d01b296e4ec83e1d567180e494222d8193a0dd87acc696dba11`. Los errores de asignación producidos deliberadamente por el control de saturación son esperados **solo en este banco**.

[build_probe.py](build_probe.py) recompila la sonda y restaura las fuentes en `finally`; requiere actualizar el hash e instalar el nuevo binario de pruebas antes de repetirlo. [trace-operations.json](trace-operations.json) y [TraceOperations.hpp](TraceOperations.hpp) conservan la demanda lógica extraída de la traza, sin partidas. Los auxiliares `build_trace.py` / `run_trace.py` sirven para diagnóstico a 400 plazas, con un prefijo separado; las trazas históricas no son comparaciones deterministas de recorridos idénticos.

## Integración y límites

**ASI normal instalado:** `f01faea570319f6f3f7b5a7278ae7b80e3fc27507e478fa367a811b677fdaa93`. Respaldo `_baseline/before-entryexit-pool-20260923-141213`, únicamente en la copia de pruebas.

Integración sin exclusiones: **120,24 segundos**, **3.404 fotogramas**, **0 fallos de asignación de CEntryExit**, máximo **394/512** entradas. Esta pasada natural no volvió a alcanzar 400; la demanda de 419 y la reutilización de las 512 plazas se probaron en el banco determinista. Evidencia: [informe de integración](../Unlocks/artifacts/current-20260923-141219/summary.json).

No hubo cierre inesperado, aserción ni RTC. Persisten avisos de timecyc, definición skimmer, guardado incompatible, cinco de IK y 18 de peatones fuera del mapa. Ya existía un aviso de peatón en una observación del ASI anterior; no se establece aquí una causa ni se declara limpio el log completo.

Esos avisos describen esta ejecución histórica. Después se repararon [peatones](../FallenPeds/README.md) e [IK/datos](../IKDataWarnings/README.md), y se verificó la [recuperación del ratón](../MouseRecovery/README.md). La partida incompatible continúa protegida frente a carga forzada.

El [resumen verificable](validation-summary.json) identifica binarios, fuentes, respaldo, logs y restauración. El ejecutor de integración permite:

```powershell
python reversing/Unlocks/run_current_build.py --entryexit-pool --seconds 120
```

Esta opción lee la ocupación sin escribir memoria del juego y exige cero mensajes nuevos de fallo de CEntryExit para aprobar. Las partidas y ambos INI se verifican y restauran; para uso normal siguen `FastLoader=false` y `SaveGameToLoad=-2`.

La reserva sigue siendo finita. No se certifican sesiones largas ni un ciclo completo de guardar/cargar, ni compatibilidad de guardados nuevos que usen índices superiores a 399 con un ejecutable sin esta ampliación. Las partidas existentes se conservan sin modificaciones. No se incorporan los 86 desbloqueos aplazados ni se cambia el porcentaje de reversing: CSV y catálogo de clases no necesitan modificar ninguna fila.

> **Nota 07/10/2026 — ampliación retirada.** Este banco (sonda congelada) comprobaba el pool de `CEntryExit` ampliado a 512 plazas.
> Midiendo con el EXE solo (`--unhook-except=Win/Win`, partida nueva) el pool llega a 400 de 400 en los primeros minutos de partida,
> así que llenarse es el comportamiento normal del original y no una fuga de la DLL. Se volvió a 400 plazas como el EXE
> (`CEntryExitManager::Init` validado de nuevo con el banco `EnExInit`) y el aviso "Allocation failed" del log solo se registra
> una vez por tipo de pool. Este banco ya no aplica (exige `live_capacity == 512`) y no forma parte de la regresión.
