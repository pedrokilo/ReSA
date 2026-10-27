<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies22C (grupo g22c de BRIEF_21): CCam::WrapAngle y FxEmitter_c::CreateParticles

Estado: banco ESCRITO y comprobado sin juego (arnes aislado con los bytes reales del EXE y compilacion en el contexto del proyecto); NO se ha ejecutado dentro del juego (encargo de solo analisis).

| Funcion | Entrada | Convencion | Pares |
| --- | --- | --- | --- |
| CCam::WrapAngle | 0x509BE0 | cdecl, `float*`; el EXE conserva EDX y deja ECX = puntero | 2048 |
| FxEmitter_c::CreateParticles | 0x4A41E0 | thiscall, `(float, float)`, `ret 8` | 4096 |

Hook de cada una apagado para el lado original (EXE) y encendido para el candidato (`OriginalDependencies499/HookRoute.hpp`). WrapAngle usa `RegisteredTarget` (la envoltura `WrapAngleKeepEdx` es estatica en Cam.cpp).

## Que se compara (todo en bits, original == candidato)
- Memoria: arena propio completo con 16 bytes de guarda por lado. Retorno/ESP/EBX/ESI/EDI/EBP; semillas en EAX/ECX/EDX (EDX por caso en WrapAngle).
- x87 completo: FNSAVE de 108 bytes (TOP, FCW, FSW con C0-C3 y banderas acumuladas, FTW, registros ST vacios incluidos; FIP normalizado) y MXCSR. `kCompareDeadX87Registers = true` (no se relaja). Clase de la ultima x87 (fcomp m32 / fstp m32) por los bytes en FIP, en el EXE para el original y en el ASI para el candidato.
- WrapAngle: EDX conservado y ECX = puntero en ambos lados (los lee KeepTrackOfTheSpeed 0x50A052). EAX solo se registra (nadie lo lee).
- CreateParticles: traza de colaboradores (tipo, desplazamientos, bits de floats, hashes FNV de EmissionInfo_t, matrices y FxPrtMult_c), intensidad final y camino. ECX/EDX/EAX solo se registran: ningun llamador los lee (callers.dis.txt).
- Colaboradores sustituidos en AMBOS lados por los mismos stubs trazados (EntryPatch en la direccion del EXE y en la entrada de la DLL, resuelta con `Resolve()`): ProcessEmissionInfo 0x4A4960, FxRwMatrixCreate 0x4A9440, FxRwMatrixDestroy 0x4A9460, GetRWMatrix 0x4A9DC0, RwMatrixUpdate 0x7F18A0, RwMatrixMultiply 0x7F18B0. CreateParticle 0x4A2580 esta ya en C++ (lote 30, hook RH) y CreateParticles la llama en C++: se sustituye en AMBOS lados con el mismo stub (EntryPatch en 0x4A2580 y en su cuerpo real de la DLL, resuelto con el hook ENCENDIDO; ese hook se enciende solo durante cada llamada, con los parches creados encendido y destruidos antes de apagarlo; la ruta se exige en `route_ok`). Sin ese parche doble el candidato ejecutaria la CreateParticle real y el banco dejaria de probar CreateParticles. El constructor de FxPrtMult_c y _ftol2 se ejecutan de verdad. CWeather::Wind/Rain se guardan y restauran.

## Fixtures (Fixtures.hpp; fixture_oracle.py las repite; comprobadas iguales en los 6144 casos)
- WrapAngle: tabla de 72 valores x4 y 1760 generados (magnitud media, k*2pi+-delta, +-pi/2pi +-ulp, fracciones); NaN con carga/signo distinto y senalizantes, +-0, subnormales, hasta 2^24. FUERA de dominio (el EXE no termina): +-inf, |x| >= 2^27 (x - 2pi == x en float), +-1e30, +-FLT_MAX; se comprueba en el oraculo y en el analizador.
- CreateParticles: 4096 casos (25% "benignos" que recorren el bucle 1..15 particulas; el resto con dist NaN/senalizante/inf/subnormal/1e30/FLT_MAX, lod invertidos/iguales, count/init de estres, limites de viento/lluvia NaN/inf, delta NaN/1e30...). Si la suma prevista esta en [16, 1e19) el caso sale por el viento (evita bucles de miles de millones; por encima de 2^63 `_ftol2` da EAX = 0 y no hay bucle).

## Oraculo independiente (fixture_oracle.py)
Aritmetica x87 a 24 bits sobre fracciones exactas (NaN/inf/cero con signo con reglas IEEE): intensidad final, camino, numero de particulas, timeSince, traza y arena completos. Contrastado con los bytes reales del EXE reubicados (offline/): 4096/4096 casos de CreateParticles y 2048/2048 de WrapAngle sin diferencias.

## Compuertas de run_local.py
Igualdad fila a fila, hook registrado y cuerpo en el ASI, colaboradores sustituidos, ABI/x87/ruta en ambos lados, `fp_difference` vacio, fixture == oraculo, estado/traza/intensidad == oraculo, EDX/ECX de WrapAngle, clase de la ultima x87, cobertura minima (caminos, 5 salidas, bucles 1..8, padre/sin padre, local/global, NaN) y restauracion final (hooks, Wind/Rain, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.

## Ejecucion
```
python -B reversing/Dependencies22C/fixture_oracle.py
python -B reversing/Dependencies22C/build_probe.py
python -B reversing/Dependencies22C/run_local.py
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta> Dependencies22C
```
Antes: aplicar `g22c.patch`, anexar `g22c-x87.cpp.txt`/`.h.txt` a X87Math.cpp/.h, anadir las filas de `g22c-hooks.csv.txt` a docs/hooks.csv, aplicar los cambios de `g22c-audit.txt` (o `g22c-extras.patch`, que los incluye todos); compilar; `python -B contrib/audit_register_residue.py`.

## offline/ (evidencia sin juego)
`gen_cp.py`/`gen_wrap.py` extraen los bytes del EXE (exebytes.inc); `wh.cpp` (WrapAngle) y `cph.cpp` (CreateParticles, con los mismos stubs, Fixtures.hpp y Arenas.hpp) ejecutan esos bytes reubicados frente al codigo C++ (X87Math.cpp real compilado con /O2 /Oi /arch:IA32 /fp:fast + `cp_body.inc`, el texto de CreateParticles de FxEmitter.cpp) y comparan memoria, traza, FNSAVE y registros. `build.sh X87Math.cpp` y `buildh.sh <arnes.cpp> X87Math.obj`; `cph.exe N 0 o` vuelca arenas/trazas para `cmp_o.py`. Resultado: 0 diferencias en 4096 fixtures + 5.000.000 aleatorios (CreateParticles) y 250.000 aleatorios + 2048 fixtures (WrapAngle).

## Limites
- No ejecutado en el juego. Esta CPU deja FDP/FOP a 0 tras fcomp/fstp (medido), por eso las constantes de la DLL no afectan al FNSAVE.
- El `_ftol2` del CRT de la DLL es la variante SSE (no deja valores en la pila): X87Math.cpp escribe el residuo del `_ftol2` del EXE (v-R y R) con cargas/almacenamientos; solo equivale con redondeo al par (el del juego).
- CreateParticle (0x4A2580) no se prueba aqui (banco FxEmitterCreateParticle30): se sustituye por el stub en ambos lados. Si el compilador la incorporase en linea dentro de CreateParticles el parche de la DLL no la interceptaria y la traza del candidato fallaria frente al oraculo (se detecta, no se oculta). offline/cph.cpp y cp_body.inc no se han regenerado con la fuente del lote 30 (el arnes ya sustituye CreateParticle con el mismo stub en ambos lados).
