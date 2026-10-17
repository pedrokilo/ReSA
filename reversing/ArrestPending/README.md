<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# ArrestPending

Clase: `CTaskComplexArrestPed`. Funciones: ControlSubTask, CreateNextSubTask, CreateSubTask. Casos previstos: 2560 pares EXE/DLL.

Puertas, vehículo, caída, arresto, persecución y selección de arma. Constructores seguros sobre arena; inteligencia, armas y referencias sustituidas en ambos lados. Compilación y comparación pendientes de la cola coordinada; este README no acredita aprobación.

La reconstrucción se contrasta con `combo.sh` y ensamblador completo almacenados en `reversing/TasksPendingEvidence`. Cuando Ghidra no reconoce una función, se conserva explícitamente esa limitación y el ensamblador es la referencia. Las funciones virtuales se alternan mediante los hooks y sus ranuras comprobadas; los getters usan el ABI de retorno por estructura.

El banco compara retorno, bytes relevantes de fixtures y trazas ordenadas. Usa fixtures propios, restauración de globals/hooks y cobertura positiva por rama; cada pareja tiene un identificador global. `PumpMessages` mantiene la ventana receptiva. No usa tareas ni peds reales como entradas.

`build_probe.py` declara los cpp/h dependientes para la huella de fuentes; `run_local.py` espera las comparaciones y el resumen final. Un resultado válido exige pares completos, igualdad, cobertura positiva y restauración. Los helpers sustituidos delimitan la equivalencia del controlador: no validan integración con IA, animaciones, pools o audio reales. Los artefactos de compilación y ejecución son la evidencia; reconstruida no significa validada.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.560 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-arrest-pending\20261007-200023-270451/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
