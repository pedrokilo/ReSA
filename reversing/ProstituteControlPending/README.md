# ProstituteControlPending

Clase: `CTaskComplexProstituteSolicit`. Funciones: ControlSubTask. Casos previstos: 1024 pares EXE/DLL.

Cámara, temporizadores, observadores, policía, pago, controles, vibración y fuerza. Pad, cámara, tiempo y cheat se guardan/restauran; aplicación de fuerza y audio se trazan. Compilación y comparación pendientes de la cola coordinada; este README no acredita aprobación.

La reconstrucción se contrasta con `combo.sh` y ensamblador completo almacenados en `reversing/TasksPendingEvidence`. Cuando Ghidra no reconoce una función, se conserva explícitamente esa limitación y el ensamblador es la referencia. Las funciones virtuales se alternan mediante los hooks y sus ranuras comprobadas; los getters usan el ABI de retorno por estructura.

El banco compara retorno, bytes relevantes de fixtures y trazas ordenadas. Usa fixtures propios, restauración de globals/hooks y cobertura positiva por rama; cada pareja tiene un identificador global. `PumpMessages` mantiene la ventana receptiva. No usa tareas ni peds reales como entradas.

`build_probe.py` declara los cpp/h dependientes para la huella de fuentes; `run_local.py` espera las comparaciones y el resumen final. Un resultado válido exige pares completos, igualdad, cobertura positiva y restauración. Los helpers sustituidos delimitan la equivalencia del controlador: no validan integración con IA, animaciones, pools o audio reales. Los artefactos de compilación y ejecución son la evidencia; reconstruida no significa validada.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 1.024 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-prostitute-control-pending\20261007-195853-338047/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
