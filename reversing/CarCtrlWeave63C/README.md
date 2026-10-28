<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlWeave63C: CCarCtrl, parte B2, tanda C (UpdateCarOnRails)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-3`, HEAD df26d6fe7 + patches A y B + `source.patch` de esta tanda). 4096 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauración en ambos lados,
cobertura mínima cumplida. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlweave63c\20261020-230002-455115`.

| Función | Dirección | Hook | Colaboradores sustituidos |
|---|---|---|---|
| `CCarCtrl::UpdateCarOnRails` | 0x436540 | Simple | 0x422590, 0x432B10, 0x434790, 0x41B980, 0x43C900, 0x42EC90 |

`source.patch` (aplicar tras A y B) contiene la función, `#include "Curves.h"` y su `RH_ScopedInstall`.

## Qué hace
Llama a 0x422590; con `bIsStopped` termina; con acción temporal 1 o 24 pone la velocidad a 0, `ModifySpeed(0)` y, vencido `m_nTempActionTime`, limpia la acción y fija los dos relojes; con la 12 solo velocidad 0 y `ModifySpeed(0)`.
En el caso normal: `SlowCarOnRailsDownForTrafficAndLights`; avanza el tramo con 0x432B10 si `field_C + escala` es negativo o ya venció; sale si el estado es 3; calcula el tiempo normalizado del tramo, los carriles (`OneWayLaneOffset`, 5.4; +1.458 para el
subtipo 10), las direcciones normalizadas (0x59C910 del EXE) con una pequeña variación por semilla (0.009), `CalcCurvePoint` (posición y velocidad), fija z = 15 y coloca el vehículo con 0x42EC90; la velocidad (x 1/60) solo se guarda si el tramo actual y el siguiente son distintos.

## Revisión propia contra el EXE (fase 2)
Desensamblado completo y marco de pila normalizado (como en la tanda A); contrastados el orden de operaciones, las constantes por dirección/bits y los enteros (la comparación de 64 bits `cdq` de `field_C + escala` es `limit < 0 || now >= (uint32)limit`; el `fild` de un entero negativo
+2^32 y el `fidiv` por la escala con signo). Discrepancias halladas tras la transcripción: ninguna.

## Registros EAX/ECX/EDX
Llamadores del EXE: 0x6B51DD, 0x6BCC74, 0x6C1B82; solo hay un `push ecx` posterior como reserva de espacio sobrescrita por `fstp [esp]`. Solo se registran.

## Suposiciones y límites
- El cociente (now - field_C) / escala se calcula en double y se redondea una vez a float (el EXE hace `fild`/`fidiv` a 24 bits): un doble redondeo es posible solo en empates exactos (igual que en `CAutoPilot::ModifySpeed`, ya validada).
- Los enlaces son memoria del banco (dos áreas de `ThePaths.m_pNaviNodes` nulas en el juego, sustituidas y restauradas); sin nodos de ruta ni mundo reales.
- Mismos límites x87 que la tanda A en los valores raros (NaN, infinitos) que entran por el falso de `CalcCurvePoint`.

## Comandos
```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlWeave63C
python -I reversing/CarCtrlWeave63C/fixture_oracle.py
python -I reversing/CarCtrlWeave63C/fixture_mirror.py
python -B reversing/CarCtrlWeave63C/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlWeave63C/register_hook.py <carpeta-de-evidencia>      # SOLO el coordinador
```
