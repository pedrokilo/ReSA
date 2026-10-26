<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# DriveByPending

Clase: `CTaskSimpleGangDriveBy`. Funciones: Clone, MakeAbortable, ProcessPed. Casos previstos: 2560 pares EXE/DLL.

Clonado, aborto, asiento, grupos de animación, disparo, recarga, temporizador y jugador. Helpers no catalogados de la clase ejecutan la dependencia original con stubs seguros. Compilación y comparación pendientes de la cola coordinada; este README no acredita aprobación.

La reconstrucción se contrasta con `combo.sh` y ensamblador completo almacenados en `reversing/TasksPendingEvidence`. Cuando Ghidra no reconoce una función, se conserva explícitamente esa limitación y el ensamblador es la referencia. Las funciones virtuales se alternan mediante los hooks y sus ranuras comprobadas; los getters usan el ABI de retorno por estructura.

El banco compara retorno, bytes relevantes de fixtures y trazas ordenadas. Usa fixtures propios, restauración de globals/hooks y cobertura positiva por rama; cada pareja tiene un identificador global. `PumpMessages` mantiene la ventana receptiva. No usa tareas ni peds reales como entradas.

`build_probe.py` declara los cpp/h dependientes para la huella de fuentes; `run_local.py` espera las comparaciones y el resumen final. Un resultado válido exige pares completos, igualdad, cobertura positiva y restauración. Los helpers sustituidos delimitan la equivalencia del controlador: no validan integración con IA, animaciones, pools o audio reales. Los artefactos de compilación y ejecución son la evidencia; reconstruida no significa validada.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.560 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-drive-by-pending\20261007-195931-567873/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->

## 2026-10-26: SetupAnim sustituido en ambos lados

El commit b0d3f19d7 (lote 67) reconstruyo y puso hook a `CTaskSimpleGangDriveBy::SetupAnim` (0x627B20), que el banco sustituia por un falso solo en la direccion del EXE. Desde entonces `MakeAbortable` y `ProcessPed` de la DLL llamaban al `SetupAnim` C++ real: faltaba el evento `setup` en la traza del candidato y los campos de la tarea diferian (el fallo era del banco, no de produccion). `LiveComparison.hpp` anade `EntryPatch sud(&CTaskSimpleGangDriveBy::SetupAnim, Setup)`, con el mismo falso (y la misma traza) que `su` en 0x627B20, igual que ya se hacia con AITarget/PlayerTarget/AimGun. Las compuertas (conmutacion del hook, cobertura, restauracion) no se tocaron y los casos siguen en 512 + 1024 + 1024. `SetupAnim` queda validado por su propio banco (LeaveCar67). Reejecucion: **APROBADO**, 2.560/2.560 pares iguales, 0 diferencias; evidencia en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-drive-by-pending\20261026-124641-249129`.
