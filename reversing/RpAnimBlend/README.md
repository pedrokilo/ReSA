<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RpAnimBlendClumpUpdateAnimations (0x4D34F0)

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Estado: reconstrucción validada y hook activado. RpAnimBlend: 43/43 funciones registradas (100%).

El ejecutable revisado es el Compact US cuyo SHA-256 es 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. El desensamblado local abarca 0x4D34F0–0x4D3773.

## Diferencias corregidas durante la revisión

- 0x4D3519 asigna gpAnimBlendClump antes de salir por lista vacía; el C++ lo hacía después.
- 0x4D3548 comprueba el puntero de secuencias, no solamente su contador.
- Se conserva el orden división/multiplicación del cálculo sincronizado. El original no llama a UpdateTimeStep en ese bucle: contiene la operación directamente. Se evita calcular primero un recíproco flotante redondeado.
- Se respeta la selección independiente de los callbacks de actualización. USE_COPY_PASTE_FRAME_UPDATE está definido como 0, mientras otros bloques usan #ifdef y DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS vale 1. El original usa las entradas 0x4D2E40/0x4D32D0/0x4D2B90/0x4D30A0 y 0x4D2E10; el candidato no debe sustituirlas silenciosamente por plantillas diferentes que están sin validar.
- El recorrido final lee el siguiente enlace después de UpdateTime, como el original, para preservar el efecto de callbacks que modifican la lista.

La lista de nodos admite once asociaciones y un terminador nulo; las asociaciones adicionales siguen aportando al cálculo de tiempos. Este límite ya estaba conservado.

## Validación dentro del juego

El 2026-09-22 se aprobaron 652 comparaciones entre la función original y la reconstruida dentro del proceso de GTA, con clumps independientes y entradas iguales. Error máximo: 2.9802322387695312e-08; tolerancia: 1e-05. Flags, callbacks y contadores coincidieron exactamente. La pose del jugador y las partidas conservaron sus valores originales.

Se probaron listas vacías, animaciones sincronizadas y libres, mezcla parcial, pausa, eliminación, callbacks de finalización, trece asociaciones, actualización fuera de cámara y recuperación de keyframes; con y sin skin, comprimidas y sin comprimir. Las restricciones de las entradas y ensayos descartados están en `fixture-notes.md`.

Son escenarios controlados ejecutados en el motor, no una revisión visual ni una garantía de todos los escenarios de juego. El porcentaje cuenta funciones registradas como reconstruidas; no representa cobertura exhaustiva.

Ejecutar `PROBAR.cmd --case rpanimblend`. El runner local instala la sonda archivada, ejecuta el banco y restaura el ASI anterior, sin depender de un agente activo. Requiere la instalación local de pruebas y la sonda con su hash verificado; los binarios no se incluyen en Git.

Evidencia resumida: `validation-summary.json`. Registro completo local: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-rpanimblend/20260922-191537-770481`. SHA-256 del JSONL: `0bdb9f65285934aee34f1b797d857f44e35455e0109af2a0b96af4e2e7070a29`.

La compilación normal excluye la instrumentación temporal. `Probe.hpp` y `LiveComparison.hpp` conservan la fuente del banco para futuras reconstrucciones de la sonda.
