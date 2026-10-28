<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlSteer63f: DealWithBend_Racing (lote 63, CCarCtrl parte B1, tanda 6)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-2`, HEAD df26d6fe7 + fuente de las tandas 1 a 6). 4096 pares, cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauracion
en ambos lados y salida de cada lado (eventos, cambios de arena, `ret` en AL y punteros de CPathFind restaurados) igual a la del oraculo independiente (`fixture_oracle.py`, desde el desensamblado); cobertura minima cumplida.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63f\20261020-210808-088764`. La comparacion de NaN como clase estaba activa, pero NO hizo falta: ningun par difirio en el crudo (`coverage.nan_raw_difference` ausente), es decir, los 4096 pares son iguales bit a bit.

**Regresion sobre el arbol FINAL** (las seis tandas 63/63b..63f juntas, compiladas y corridas de nuevo el 20/10/2026 21:10..22:30): APROBADO, carpeta de evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63f\20261020-222844-058174`; es la que corresponde al `artifacts/build-manifest.json` de este banco (usar esta para `register_hook.py`).

| Funcion | Direccion | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CCarCtrl::DealWithBend_Racing(vehicle, l1, l2, l3, l4, a6, a7, a8, a9, a10, o11, o12, o13, o14, pos)` | 0x428040 | Simple | 4096 | CGeneral::GetATanOfXY 0x53CC70 |

`cdecl`, 15 argumentos de una palabra (las direcciones de tramo ocupan una palabra con basura en los 16 bits altos), devuelve `bool` en AL. `OneWayLaneOffset` (0x44DB00, en el EXE una funcion; en la DLL en linea en
`PathFind.h`) y la interseccion de dos rectas (`FindIntersection2Lines`, 0x4226F0, envoltura al EXE) NO se sustituyen: corren de verdad en ambos lados.

## Que hace (resumen del desensamblado)

Para una carrera: dados hasta 4 tramos de camino consecutivos (`CCarPathLink`, 0xE bytes: posicion int16 x 0.125 en +0/+2, direccion int8 en +8/+9, banderas de carriles en +0xB) decide el punto al que apuntar en una curva.
1. Devuelve false si `l1` o `l2` son 0xFFFF o sus areas no estan cargadas (`m_pPathNodes[area]` nulo); antes pone `*o12 = 1.0f` (moto, subtipo +0x594 == 10: umbrales 0.3485/0.6985 rad y 50 m; resto 0.5236/1.0472 y 40 m).
2. `*o14` = distancia del vehiculo al primer tramo; > 60: false. Encadena `l3` y `l4` mientras cada uno este a menos de 10 m del anterior (multiplicador de direccion = `a7`, `a8` o `a9` segun el ultimo).
3. Rumbo del vehiculo (top de su matriz) menos rumbo de la direccion del ultimo tramo (el ultimo `fpatan` se resta SIN redondear: ayudante `CarCtrlSteer63AtanRsubCpp`), normalizado a [-pi, pi]; si |dif| < umbral mínimo: false;
   si los dos tramos extremos tienen >= 4 carriles y |dif| < umbral ancho: false. `*o13 = dif`.
4. Puntos de carril (`5.4 x (desplazamiento + (carriles - 1)) x direccion`) de ambos tramos, el mas cercano al otro extremo de cada uno; el segundo punto del primer tramo se acorta a 11 m; direcciones casi paralelas
   (|dx| < 0.1 y |dy| != 0): punto A; si no, interseccion de las dos rectas en `*pos`.
5. `*o12 = 1 - ((limite - D) / limite * min(|dif x 0.6366|, 1)) x 0.6` si D < limite; `*o11` = rumbo hacia `*pos` o hacia el segundo punto B segun `D < min-max(a10 / 30) x min(|dif x 0.6366|, 1) x 20`. Devuelve true.

## Fuente (CarCtrl.cpp, X87Math.cpp/.h)

`float` puro con el x87 a 24 bits, tramos leidos en crudo (direcciones int8 x `0.01f`, no `/100`), `m_pPathNodes` comprobado y `m_pNaviNodes` indexado como el EXE. Ayudante x87 nuevo `CarCtrlSteer63AtanRsubCpp`
(`hf - GetATanOfXY(x, y)` con un solo redondeo; bloque «Lote 63c» de X87Math.cpp/.h). El primer y el ultimo `GetATanOfXY` se guardan a float, como el EXE.

## Revision propia contra el EXE (fase 2)

Se desensamblo `0x428040..0x42897D` completo (mapa de pila reconstruido; `[esp+58h]` reutiliza la palabra de `l1` para `L1y`, `[esp+2Ch]`/`[esp+58h]` se pisan con el punto C). Puntos delicados: el orden de las dos
comprobaciones (`l1`/`l2` invalidos ANTES de tocar `*o12`, areas despues), el bucle `d > pi` antes que `d < -pi` (al reves que en los demas pasos de direccion), `jp`/`jnp` de la prueba de paralelismo (la rama
«paralela» es |dx| < 0.1 y |dy| distinto de 0, NaN incluido), el recorte `1.0 < p ? 1.0 : (0.2 > p ? 0.2 : p)`, la comparacion `D < umbral` con `fcompp` y las dos formas de elegir A/B y C (empate -> R). Defectos reales
hallados por el banco: ninguno (aprobado a la primera).

## Registros (EAX/ECX/EDX)

Llamador unico del EXE: 0x435E05 (dentro de `SteerAICarWithPhysicsFollowPath_Racing`, 0x435830): `add esp,3Ch; test al,al; jne ...`: **solo lee AL** (el banco lo compara, `ret`); EAX alto, ECX y EDX no se leen
y solo se registran.

## Banco

Igual estructura que CarCtrlSteer63 (falsos identicos en ambos lados con parche en la entrada del EXE y en el cuerpo de la DLL, arena con guardas, compuertas de ABI, oraculo independiente desde el desensamblado,
espejo del generador). Dos arrays de 8 `CCarPathLink` en la arena con sus punteros escritos en `CPathFind::m_pPathNodes` (0x96F854) y `m_pNaviNodes` (0x96F974) para las areas 7 y 20 (96 palabras guardadas y restauradas tras
cada lado). El observado incluye `ret` (AL), las salidas `o11..o14` y `pos`. Los NaN se comparan como clase (campo `nan_raw_difference` de la fila cuando el crudo difiere: el x87 y el SSE propagan NaN distintos
cuando hay dos NaN).

## Suposiciones y limites

- Colaborador falso: no se valida su cuerpo. NaN silenciosos; sin rumbos infinitos ni enormes (bucles de normalizacion); tramos 0..7 de las areas 7 y 20; vehiculo con matriz siempre.
- `FindIntersection2Lines` (0x4226F0, aun envoltura al EXE) se llama por su direccion: en el banco corre el mismo codigo del EXE en ambos lados.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlSteer63f   # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlSteer63f/fixture_oracle.py       # autoprueba del oraculo
python -I reversing/CarCtrlSteer63f/fixture_mirror.py       # coherencia de las fixtures + cobertura minima (sin el juego)
python -B reversing/CarCtrlSteer63f/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlSteer63f/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
