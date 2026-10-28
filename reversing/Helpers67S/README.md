<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers67S: CDecisionMakerTypes::CDecisionMakerTypes 0x4650F0 (constructor, lote 67S)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-4`, HEAD 1ca520153 + `source.patch`). 16384 pares, cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados, cero llamadas del candidato a los colaboradores del EXE,
y salida de cada lado igual a la del oráculo independiente. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-helpers67s61026-070716-956298`. (Una corrida previa, `20261026-070355-271930`, falló 16384/16384 por un defecto del banco, ver «Hallazgos».)

| Función | Dirección | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CDecisionMakerTypes::CDecisionMakerTypes()` (el «helper sin símbolo confirmado» de `CDecisionMakerTypes::InjectHooks`) | 0x4650F0 | `CDecisionMakerTypes` / `Constructor` / S (`RH_ScopedInstall`) | `DecisionMakers/DecisionMakerTypes.cpp/.h` |

Fila de `docs/hooks.csv` (la añade el coordinador; no es virtual): `CDecisionMakerTypes,Constructor,0x004650F0,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh Helpers67S        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/Helpers67S/build_probe.py
python -B reversing/Helpers67S/run_local.py
python -B reversing/Helpers67S/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/Helpers67S/fixture_oracle.py          # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/Helpers67S/fixture_selfcheck.py       # compila fixture_dump.cpp (Fixtures.hpp tal cual), compara constantes de arena, calcula la cobertura prevista (imprime «ok»)
python -B reversing/Helpers67S/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers67s/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `helpers67s-runtime.start`; salida `helpers67s-runtime.jsonl`.

## Qué es 0x4650F0 (desensamblado)

Constructor de `CDecisionMakerTypes` (0xF1C0 bytes). Llamador único: `CDecisionMakerTypes::GetInstance` (0x4684F0, en 0x46852E), que reserva 0xF1C0 bytes con `operator new` (0x82119A) y lo llama. thiscall, `ECX = this`, sin argumentos en pila, `ret`.
Marco SEH propio (`push -1; push 0x837CD9; fs:[0]`) cuyo único fin es destruir (0x4634C0 = `ret`) los `CDecisionMaker` ya construidos si algún constructor lanzara; ninguno puede.

1. `[this+0] = 0` (`m_NoOfDecisionMakers`).
2. `eh vector constructor iterator` (0x821D30, stdcall `ret 14h`) de 20 elementos de 0x99C bytes en `this+4` con el constructor 0x4650A0 (`CDecisionMaker::CDecisionMaker`: 41 × `CDecision::CDecision` 0x6040C0 —`call SetDefault; mov eax,ecx; ret`— y 41 × `SetDefault` 0x600530).
3. Los cinco decision makers por defecto (`this+0xC1B4`, `0xCB50`, `0xD4EC`, `0xDE88`, `0xE824`), cada uno en línea: 41 × 0x6040C0 y 41 × 0x600530.
4. Globales: `0xC0AFE0` (5 dwords = 0: banderas «es de grupos»), `0xC0AFF4` (`rep stos` de 10 dwords con 0x00010001: `ScriptReferenceIndex[20] = 1`), `0xC0B01C` (5 dwords = 0: `m_IsActive`).
5. Salida: `EAX = this`, `ECX` = cabeza anterior de la cadena de excepciones (la restaura), `EDX = 0` (`xor edx,edx`).

`SetDefault` (0x600530): por 6 ranuras `[d+4i] = 0xFFFFFFFF; [d+0x18+4i] = 0; word [d+0x30+2i] = 0`, es decir, una `CDecision` de 0x3C bytes = 24 bytes 0xFF y 36 bytes 0.
**No toca** `m_EventIndices` (`this+0xC034`, 0x180 bytes) ni la instancia (`0xC0B030`).

## Fuente (`source.patch`: sólo `DecisionMakerTypes.cpp/.h`)

- `DecisionMakerTypes.h`: constructor declarado (`CDecisionMakerTypes();`) y la envoltura de hook `CDecisionMakerTypes* Constructor()` (como `CHeli::Constructor`); `m_EventIndices` deja de inicializarse a cero (`{}`): el EXE no lo toca y `LoadEventIndices` lo rellena;
  `VALIDATE_SIZE(CDecisionMakerTypes, 0xF1C0)`.
- `DecisionMakerTypes.cpp`: `CDecisionMakerTypes::CDecisionMakerTypes()` en C++ puro (`m_NoOfDecisionMakers = 0`; los 25 `CDecisionMaker` los construyen los miembros, en el orden del EXE; banderas de grupo a 0, `ScriptReferenceIndex.fill(1)`, `m_IsActive.fill(false)`);
  `Constructor()` (`__declspec(safebuffers)`, `runtime_checks` apagado como `GetInstance`) lee `fs:[0]` al entrar, construye y termina en `ResidueRegsCpp(fs:[0] de entrada, 0, this)`: EAX = this, ECX = cabeza anterior, EDX = 0, como el EXE (`GetInstance` toma el EDX del constructor como residuo propio).
  `RH_ScopedInstall(Constructor, 0x4650F0)` en `InjectHooks`. `GetInstance` no cambia: sigue llamando al constructor por la dirección del EXE (`kExeConstruct`), que ahora salta a la DLL; su banco (`DependenciesDecision*`) no forma parte de este.
- Sin `__asm`/`naked`; ningún archivo nuevo en `source/game_sa`.

## Qué compara el banco

Original = bytes del EXE con el hook `Constructor` apagado; candidato = DLL con SOLO ese hook encendido. El hook de `CDecision::SetDefault` (0x600530) queda ENCENDIDO en ambos lados: en el lado EXE 0x600530 salta a la `SetDefault` de la DLL (validada en su propio banco) y el candidato la llama directamente; las otras tres entradas (0x821D30, 0x4650A0, 0x6040C0), sin hook, ejecutan código del EXE en el lado EXE y llevan trampas en el candidato que cuentan la llamada (cero en los 16384 pares; `candidate_exe_calls`). No hay falsos: el resultado es memoria determinista.
Por caso se compara: CRC-32 del objeto de 0xF1C0 bytes completo (incluye `m_EventIndices` intacto), el resto de la arena (guardas de 0x40, relleno y bytes fuera del objeto), la región de globales `0xC0AFD0..0xC0B03F` (0x70 bytes: banderas de grupo, `ScriptReferenceIndex`,
`m_IsActive`, 16 bytes a cada lado; la instancia `0xC0B030` conserva su valor real), `EAX = this`, `ECX = fs:[0]` de entrada, `EDX = 0` (se EXIGEN en ambos lados) y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, cadena SEH, x87 normalizado, hooks y entradas vigiladas intactas,
región de globales restaurada tras quitar los parches). La salida de cada lado se contrasta con `fixture_oracle.py`, modelado desde el desensamblado (no desde el C++).

Fixtures (`Fixtures.hpp`, 16 familias × 1024 casos): arena de ruido (dos semillas, baldosa de 127 palabras), constante (0, 0xFF, 0xCD, 0xA5), objeto en 8 desplazamientos (4 desalineados, 3 pegados a la guarda final), región de globales en ruido/ceros/0xFF/ya final/invertida/0x80,
EAX/EDX extremos y 12 semillas de pila. Mínimos de cobertura en `run_local.py` (calibrados con `fixture_selfcheck.py`).

## Suposiciones y límites

- No se ejercita una excepción dentro del constructor (el marco SEH del EXE sólo sirve para ese desenrollado).
- La región de globales se rellena con ruido sólo durante la llamada y se restaura en el acto (el juego no corre entre medias); la instancia del singleton conserva su valor real.
- `GetInstance` conserva su llamada por dirección al constructor; no se prueba aquí.

## Hallazgos

- Defecto del banco (no de producción): la primera versión apagaba el hook de `CDecision::SetDefault`. `Simple::Switch` con el hook apagado reescribe el cuerpo de la DLL con un salto al EXE, así que el candidato acababa llamando a 0x600530/0x6040C0 del EXE (2050 llamadas por caso, atrapadas) y el objeto salía distinto del oráculo. El EXE coincidía con el oráculo. Corregido dejando el hook encendido.
- Producción: sin discrepancias respecto al desensamblado; el único cambio funcional frente al código previo es que `m_EventIndices` ya no se pone a cero (el EXE no lo toca).
- Candado: la ejecución estuvo bloqueada por un `D:\Original\gta_lock` y un `reversed-tests.lock` obsoletos del 22/10; los liberó otra persona/agente.
