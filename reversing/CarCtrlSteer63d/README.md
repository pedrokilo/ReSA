<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlSteer63d: SteerAICarBlockingPlayerForwardAndBack (lote 63, CCarCtrl parte B1, tanda 4)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-2`, HEAD df26d6fe7 + fuente de las tandas 1 a 5). 4096 pares, cero diferencias EXE/DLL (con NaN comparados como CLASE, ver abajo), todas las
compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, desde el desensamblado); cobertura minima cumplida. Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63d\20261020-205846-574956`.
**Caveat NaN**: en 56 de los 4096 pares el crudo del EXE y el de la DLL difieren SOLO en el signo/carga de un NaN de salida (el EXE da 0x7FC00000 y la DLL 0xFFC00000, o al reves) y son identicos en todo lo demas. Ocurre
(causa probable, no aislada instruccion por instruccion) cuando una operacion mezcla dos NaN de signo distinto (`t = -(A / C)` voltea el signo con `fchs`, y despues `W*t + E2` suma dos NaN): el x87 devuelve el de mayor significando y, a igualdad, el de un operando; el SSE de la DLL el primero.
Con datos finitos no ocurre (las otras 4040 son bit a bit iguales). Las filas conservan el observado crudo (`nan_raw`, campo `nan_raw_difference`) y el banco compara cada palabra con patron de NaN como la clase NaN. La
primera corrida (sin esa regla) dio FALLO 56/4096 exactamente por esto.

**Regresion sobre el arbol FINAL** (las seis tandas 63/63b..63f juntas, compiladas y corridas de nuevo el 20/10/2026 21:10..22:30): APROBADO, carpeta de evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63d\20261020-220505-792469`; es la que corresponde al `artifacts/build-manifest.json` de este banco (usar esta para `register_hook.py`).

| Funcion | Direccion | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CCarCtrl::SteerAICarBlockingPlayerForwardAndBack(vehicle, steer, accel, brake, handbrake)` | 0x422B20 | Simple | 4096 | FindPlayerSpeed 0x56E090, FindPlayerEntity 0x56E120, CPlaceable::AllocateMatrix 0x54F560, CSimpleTransform::UpdateMatrix 0x54F1B0, FindPlayerCoors 0x56E010 |

`cdecl`, `void`; el llamador limpia la pila (5 argumentos). `CVector::Normalise` (0x59C910) NO se sustituye: solo se apaga su hook, de modo que el EXE ejecuta su codigo original y la DLL el suyo sobre
los mismos vectores (el banco tambien valida que coincidan, incluida la rama de longitud 0 que deja `x = 1.0`).

## Que hace (resumen del desensamblado)

El vehiculo se coloca delante o detras del jugador (bloqueo adelante-atras):
1. `*steer = 0`, `*handbrake = false`. Copia la velocidad del jugador (`FindPlayerSpeed(-1)`), toma su entidad (`FindPlayerEntity(-1)`, sin comprobar nulo; si no tiene matriz la crea:
   `AllocateMatrix` + `m_placement.UpdateMatrix`) y le suma 0.1 x el avance de esa entidad (matriz top x, y).
2. Eje derecho y eje de avance del vehiculo en el plano (z = 0), normalizados (`CVector::Normalise`). `A` = desplazamiento del jugador (`FindPlayerCoors(-1)` menos la posicion del vehiculo) proyectado sobre el eje derecho;
   `C` = velocidad lateral del jugador (0.01 si es 0); `t = -(A / C)`. Si `t < 0`: aceleracion y freno 0 y sale.
3. Segunda lectura de `FindPlayerCoors`: `G = (v_jugador_adelante * t + desplazamiento_adelante) - v_vehiculo_adelante * t`. `G > 0`: acelerador `min(1, 0.1 G)`; si no y el vehiculo avanza (`V > 0`): freno
   `min(1, -0.1 G)` y freno de mano si el freno supera 0.95; si no: acelerador `max(-1, 0.1 G)` (marcha atras suave).

## Fuente (CarCtrl.cpp)

`float` puro (con el x87 a 24 bits cada operacion del EXE equivale a la de `float`). `FindPlayerSpeed`/`FindPlayerEntity`/`FindPlayerCoors` en C++ (hooks de World.cpp); `AllocateMatrix` y
`CSimpleTransform::UpdateMatrix` en C++.

## Revision propia contra el EXE (fase 2)

Se desensamblo `0x422B20..0x422E08` completo y se contrasto instruccion a instruccion (los argumentos `arg2`/`arg5` pisados como locales, los dos productos escalares y las tres ramas finales con sus `fcomp`
y `test ah,...` con NaN). Puntos delicados: el EXE no hace la segunda llamada a `FindPlayerCoors` si `t < 0`; la condicion `t < 0` es la unica que sale antes (con NaN sigue); el divisor `C == 0` se sustituye por
0.01f (0x858C58); la rama `G <= 0` con `V > 0` pone el acelerador a 0 antes de comparar. Defectos reales hallados por el banco: ninguno de la fuente (el unico FALLO inicial fue el de la comparacion de NaN descrito arriba; el oraculo coincidia con el EXE en los 4096 casos).

## Registros (EAX/ECX/EDX)

Llamador unico del EXE: 0x4376EF (dentro de `SteerAICarWithPhysics_OnlyMission`, 0x436A90): tras la llamada solo `add esp,14h; pop edi; pop ebx; pop esi; pop ebp; ret` en una funcion `void`; **ninguno lee
EAX/ECX/EDX**. Solo se registran.

## Banco

Igual estructura que CarCtrlSteer63 (falsos identicos en ambos lados con parche en la entrada del EXE y en el cuerpo de la DLL, arena con guardas, compuertas de ABI, oraculo independiente desde el
desensamblado, espejo del generador). El vehiculo (matriz completa), la entidad del jugador (con o sin matriz; si no la tiene, `AllocateMatrix` la deja apuntando a la matriz del banco), la velocidad y las dos
posiciones del jugador son objetos del banco. Se compara: eventos de los falsos EN ORDEN, cambios de toda la arena (region `X` = cualquier cambio inesperado) y las compuertas de ABI.

## Suposiciones y limites

- Colaboradores falsos: no se validan sus cuerpos. NaN silenciosos (las fixtures los meten en algunas posiciones y velocidades).
- La entidad del jugador existe siempre (el EXE la desreferencia sin comprobar).

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlSteer63d   # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlSteer63d/fixture_oracle.py       # autoprueba del oraculo
python -I reversing/CarCtrlSteer63d/fixture_mirror.py       # coherencia de las fixtures + cobertura minima (sin el juego)
python -B reversing/CarCtrlSteer63d/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlSteer63d/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
