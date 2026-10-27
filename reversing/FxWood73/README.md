<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxWood73: Fx_c::AddWood 0x49EE10 (hook S, lote 73)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-1`, HEAD 429302be1 + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados, oraculo Python 0 lados distintos.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-fxwood7361026-233837-928624`. Corrida negativa (mutacion: direccion*4 con float SSE en vez de x87, que pierde desnormales): `20261026-233603-317254`, 823/16384 diferencias. Corridas previas: `20261026-233012-485548` (sin coverage.json; el analizador fallo por el indicador `hooked` del colaborador en run_local.py, corregido) y `20261026-233317-022053` (aprobada).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void Fx_c::AddWood(const CVector& pos, const CVector& direction, int32 amount, float lightMult)` | 0x49EE10 (thiscall, `ret 10h`, 549 bytes hasta 0x49F032) | `Fx_c` / `AddWood` / S (`RH_ScopedInstall`) | `Fx/Fx.cpp`, ayudantes en `X87Math.cpp/.h` (bloque «Lote 73», prefijo `Wood73`) |

Fila de `docs/hooks.csv` (la anade el coordinador): `Fx_c,AddWood,0x0049EE10,1,0,1,S`.

## Que hace (desensamblado 0x49EE10..0x49F032)

`this` no se usa. `d = camara - pos` (camara = `[0xB6F03C] ? +0x30 : 0xB6F02C`), `(dx2 + dz2) + dy2 > 625.0` (0x85A6E8; NaN no sale) -> sale. `FxPrtMult_c(0.5, 0.25, 0, 1, 0.3, 0, 1)` (0x4AB290). `amount <= 0` (con signo) -> sale.
Por vuelta (`amount` veces), 7 sorteos de `rand()` del EXE (0x821B1E), cada uno `cdq; idiv 10000` (resto con signo): rojo = `r*1e-4*0.12 + 0.13`, verde = `r*1e-4*0.03 + 0.12`, azul = `r*1e-4*0.03 + 0.04`, tamano = `r*1e-4*0.3 + 0.7` (0x858FC4, 0x85A6F8, 0x859020, 0x85A6F4, 0x85A6F0, 0x85A6EC, 0x858CEC, 0x858C24, 0x858CB0);
`vel = direccion * 4.0` (guardada como float), luego a cada componente `+ (r*1e-4*4.0 - 2.0)` (sin guardar el termino); `AddParticle(pos, vel, 0.0, mult, -1.0, lightMult, 0.6, false)` sobre `[0xA9AE00]` (g_fx.m_Blood, releido en cada vuelta).
Todo en x87 a 24 bits con exponente extendido: los desnormales se conservan (por eso los ayudantes `Wood73*` de X87Math, compilados /arch:IA32). El EXE usa la ranura de pila del primer argumento como temporal del resto (inocuo: `ret 10h`).

## Banco

Original = EXE con el hook apagado; candidato = DLL con SOLO ese hook. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: `FxSystem_c::AddParticle(v3d)` 0x4AA440 (registra sistema, puntero de posicion, posicion, velocidad, tiempo, los 7 floats del multiplicador, rotZ, luz, limite, createLocal; con `swap_slot` cambia `m_Blood` entre dos sistemas) y `rand` 0x821B1E (cuenta sorteos; valores por modo: realista, bordes, 32 bits cualquiera, desplazamientos exactamente 0).
`FxPrtMult_c` 0x4AB290 corre de verdad en cada lado. Se compara la traza, los sorteos, `blood_slot`, 6 arenas, la camara (region 0xB6F028) y el estado x87/MXCSR; EAX/ECX/EDX solo se registran (void). `fixture_oracle.py` es un modelo independiente (fracciones exactas, redondeo a 24 bits) que reproduce los 16384 casos.

Colaboradores y quien los valida: `FxSystem_c::AddParticle(v3d)` 0x4AA440 -> `reversing/FxSystemPending` (1024 pares); `rand` 0x821B1E: CRT del EXE, sin hook ni banco propio; `FxPrtMult_c` 0x4AB290: sin hook, corre real.

## Comandos

```
bash reversing/_handoff/bench_here.sh FxWood73
python -B reversing/FxWood73/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/FxWood73/fixture_oracle.py                      # autoprueba (ok)
python -B reversing/FxWood73/fixture_oracle.py <carpeta>/comparisons.jsonl
python -B reversing/FxWood73/register_hook.py <carpeta-de-evidencia>   # SOLO el coordinador
```
Archivo de arranque en el clon: `fxwood73-runtime.start`; salida `fxwood73-runtime.jsonl`.

## Refuerzo 2026-10-27 (rand real del EXE y amount grande)

Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fxwood73\20261027-005009-965134` (APROBADO, 16384 pares, 0 diferencias, `rng_native` verdadero, oraculo Python 0 lados distintos). Corrida aprobada previa con el mismo codigo: `20261027-003739-465935`.
Negativas (mutacion de produccion, restaurada despues): direccion*4 con float SSE en vez de x87 -> `20261027-004330-855085`, 529/16384 diferencias (antes 823 con el rand falso: con el rand real hay menos casos cuyo resultado sea desnormal); orden de los sorteos de azul y tamano intercambiado -> `20261027-004654-574893`, 12091/16384 diferencias.

- **rand real.** El falso de 0x821B1E se ha quitado. `AddWood` llama al rand del EXE por su direccion (como `CarCtrl`, `Cam` y `Decision`), asi que EXE y DLL ejecutan los mismos bytes (0x821B1E llama a getptd 0x827B3D y usa `getptd()+0x14`; seed = seed * 0x343FD + 0x269EC3, devuelve (seed >> 16) & 0x7FFF). Antes de cada lado el banco fija esa semilla al estado inicial del caso (`rand_seed`), compara el estado final (`rand_state`, ademas del hash de globales) y la restaura (region global, `globals_restored`). `begin.rng_native` comprueba los bytes de rand/getptd, que no saltan a la DLL y que rand llama a getptd; ambas entradas se vigilan.
  Estados iniciales: cualquiera (incluidos 0, 1, 0x80000000, 0xFFFFFFFF), sorteo forzado a un borde (0, 4999, 5000, 9999, 10000, 29999, 32767), y desplazamientos de velocidad con resto 5000 (uno o dos consecutivos) para conservar desnormales de la direccion; se obtienen invirtiendo la congruencia lineal (exacto). Consumo exigido: 7 sorteos por vuelta (el estado final lo demuestra: un sorteo de mas o de menos lo cambia).
  Ya no se prueban restos negativos del `idiv` (rand real nunca es negativo); eso lo cubrio la corrida anterior con el falso (`20261026-233837-928624`).
- **amount grande.** Se anaden 31, 64, 100, 255, 256, 1000, 4096 y 10000 vueltas (familia 13). La traza guardada se limita a 256 eventos; el falso de AddParticle ademas acumula un hash FNV-1a de TODOS los eventos y el oraculo lo reproduce. Sin cota: ni el EXE ni la DLL limitan las vueltas (AddParticle esta sustituido, no hay maximo de particulas; el bucle es `dec edi; jne` en el EXE y un `for` identico en la DLL). `INT_MAX` (2147483647) son 2.1e9 vueltas x 7 sorteos x 2 lados (~3e10 llamadas a rand); a ~0.2 microsegundos por sorteo y ~1 microsegundo por vuelta en el falso, del orden de 1 a 2 horas por caso y lado, ademas del hash: inviable, queda sin validar (el bucle es el mismo, solo cambia el numero de vueltas).
- **CPad::GetPad.** No aplica a `AddWood`: la funcion no lee ningun pad; este README no la cita como colaborador.
- Sigue sin validar: restos negativos del idiv con el rand real (inalcanzables), amount > 10000, y la validez de `FxSystem_c::AddParticle(v3d)` en este banco (la prueba `reversing/FxSystemPending`).
