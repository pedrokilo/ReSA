<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlSteer63c: SteerAICarTowardsPointInEscort (lote 63, CCarCtrl parte B1, tanda 3)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-2`, HEAD df26d6fe7 + fuente de las tandas 1 a 3). 4096 pares, cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauracion
en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, desde el desensamblado); cobertura minima cumplida. Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63c\20261020-203424-733454`.

**Regresion sobre el arbol FINAL** (las seis tandas 63/63b..63f juntas, compiladas y corridas de nuevo el 20/10/2026 21:10..22:30): APROBADO, carpeta de evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63c\20261020-214055-412720`; es la que corresponde al `artifacts/build-manifest.json` de este banco (usar esta para `register_hook.py`).

| Funcion | Direccion | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CCarCtrl::SteerAICarTowardsPointInEscort(vehicle1, vehicle2, x, y, steer, accel, brake, handbrake)` | 0x4336D0 | Simple | 4096 | CGeneral::GetATanOfXY 0x53CC70, FindAngleToWeaveThroughTraffic 0x4325C0, FindMaxSteerAngle 0x427FE0 |

`cdecl`, `void`; el llamador limpia la pila (8 argumentos). La multiplicacion matriz x vector 0x59C890 NO se sustituye: corre de verdad en el EXE y, en la DLL, con el ayudante x87 compartido
`AutoPreMultiplyMatrixWithVectorCpp` (X87Math.cpp), de modo que el banco tambien valida que ambas coincidan con estas matrices.

## Que hace (resumen del desensamblado)

`vehicle1` conduce hacia el punto local (x, y) del vehiculo escoltado `vehicle2`:
1. `objetivo = M2 * (x, y, 0) + velocidad2 (x, y)` (0x59C890) y `*handbrake = false`.
2. Avance de vehicle1 (columna top de su matriz) normalizado (longitud 0: (1, y)); punto de mira = objetivo + 3 x (top.x, top.y) de vehicle2; `GetATanOfXY` al punto de mira y al avance;
   `FindAngleToWeaveThroughTraffic(v1, 0, rumbo, avance, 1.0f)`; diferencia normalizada a [-pi, pi] y limitada a `FindMaxSteerAngle`.
3. Una tercera llamada a `GetATanOfXY` (objetivo - posicion) cuyo resultado el EXE DESCARTA (se conserva la llamada: los bancos comparan la secuencia).
4. `dot = proyeccion de (objetivo - posicion) sobre el avance`, `dist = distancia`. Si `dot > 0.5`: velocidad deseada `base = (dist < 15) ? max(|v2|*60 + min(4, dot - 0.6), dist * 3.5) : 300` y despues `min(base, |v2|*60 + 10)`;
   si no: dist >= 15 -> deseada 8; dist < 15 -> sin giro ni acelerador y freno 1.0 (dot < -3) o 0.1; salida inmediata.
5. Acelerador / freno: `error = deseada - |v1|*60`; `error > 0`: 1.0 si |v1|*60 >= 25 o `min(1, 0.1 * error)`; si no, freno `min(0.5, -0.05 * error)`. `*steer = giro`.

## Fuente (CarCtrl.cpp)

`float` puro con el x87 a 24 bits (cada operacion del EXE equivale a la de `float`) y el ayudante x87 compartido para 0x59C890; matrices de ambos vehiculos sin comprobar nulo (como el EXE).
El segundo `GetATanOfXY` no necesita ayudante propio: sus resultados se guardan a float antes de usarse (a diferencia de HeadingForTarget, ver CarCtrlSteer63).

## Revision propia contra el EXE (fase 2)

Se desensamblo `0x4336D0..0x433B9F` (y `0x59C890` para el oraculo) y se contrasto instruccion a instruccion, reconstruyendo los desplazamientos de pila reutilizados como locales (el argumento
`arg8`, `arg3` y `arg4` se pisan como locales; `ebx` = vehicle2 y `esi` = vehicle1). Puntos delicados: el producto escalar usa `(tY - py) * fy` y `(tX - px) * fx`, la distancia
`sqrt((tY-py)^2 + (tX-px)^2)` con dos restas independientes, las rutas «detras y lejos» (deseada 8, sin recorte a |v2|*60+10) y «detras y cerca» (salida inmediata) NO pasan por el recorte, y la comparacion
`dot < -3.0f` (0x859024) va con `jp` (NaN -> 0.1f).

## Registros (EAX/ECX/EDX)

Llamador unico del EXE: 0x43775A (dentro de `SteerAICarWithPhysics_OnlyMission`, 0x436A90): tras la llamada solo `add esp,20h; pop edi; pop ebx; pop esi; pop ebp; ret` en una funcion `void`; **ninguno
lee EAX/ECX/EDX**. Solo se registran.

## Banco

Igual estructura que CarCtrlSteer63 (falsos identicos en ambos lados con parche en la entrada del EXE y en el cuerpo de la DLL, arena con guardas, compuertas de ABI, oraculo independiente desde el
desensamblado, espejo del generador). Dos vehiculos con matriz completa (rotaciones alrededor de Z con perturbaciones, NaN y valores especiales), velocidades, punto local y posicion de vehicle1 respecto al
objetivo generadas para cubrir `dot` <= 0.5 / > 0.5, `dist` < 15 / >= 15 y `|v1|*60` < 25 / >= 25.

## Suposiciones y limites

- Colaboradores falsos: no se validan sus cuerpos. NaN silenciosos; sin rumbos infinitos ni enormes (bucles de normalizacion).
- `FindAngleToWeaveThroughTraffic` (0x4325C0, aun envoltura) se lee como `float` redondeado; ver README de CarCtrlSteer63.
- Los dos vehiculos tienen siempre matriz (el EXE la desreferencia sin comprobar).

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlSteer63c   # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlSteer63c/fixture_oracle.py       # autoprueba del oraculo
python -I reversing/CarCtrlSteer63c/fixture_mirror.py       # coherencia de las fixtures + cobertura minima (sin el juego)
python -B reversing/CarCtrlSteer63c/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlSteer63c/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.

