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
