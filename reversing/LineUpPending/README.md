# LineUpPending

Clase: `CTaskUtilityLineUpPedWithCar`. Funciones: GetLocalPositionToOpenCarDoor, GetPositionToOpenCarDoor. Casos previstos: 2048 pares EXE/DLL.

Cinco puertas conocidas y puerta de reserva, coche/bici/barco, asociación presente/ausente y blend. Modelos y grupos de animación se restauran; offsets y corrección de bici trazados. No cubre asociación con ID desconocido y el retorno indeterminado original asociado. Validación final aprobada: 2048 pares EXE/DLL iguales, cero diferencias, cobertura positiva y estado restaurado. Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-line-up-pending/20261007-194654-977255`. Esta repetición verifica la huella actual del cpp compartido tras las correcciones de ProcessPed.

La reconstrucción se contrasta con `combo.sh` y ensamblador completo almacenados en `reversing/TasksPendingEvidence`. La evidencia contiene pseudocódigo REA y ensamblador completo. Las funciones virtuales se alternan mediante los hooks y sus ranuras comprobadas; los getters usan el ABI de retorno por estructura.

El banco compara retorno, bytes relevantes de fixtures y trazas ordenadas. Usa fixtures propios, restauración de globals/hooks y cobertura positiva por rama; cada pareja tiene un identificador global. `PumpMessages` mantiene la ventana receptiva. No usa tareas ni peds reales como entradas.

`build_probe.py` declara los cpp/h dependientes para la huella de fuentes; `run_local.py` espera las comparaciones y el resumen final. Un resultado válido exige pares completos, igualdad, cobertura positiva y restauración. Los helpers sustituidos delimitan la equivalencia del controlador: no validan integración con IA, animaciones, pools o audio reales. Los artefactos de compilación y ejecución son la evidencia; reconstruida no significa validada.


<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 2.048 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-line-up-pending\20261007-200227-088394/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
