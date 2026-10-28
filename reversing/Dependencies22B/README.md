<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies22B: CVehicle::DoHeadLightEffect y CVehicle::DoTailLightEffect

PENDIENTE: banco preparado y compilado contra el arbol real, pero NO ejecutado dentro del juego. No constituye validacion hasta que `run_local.py` apruebe una corrida real. Lo descrito como
"ensayo sin juego" (`offline/`) es evidencia previa con los bytes reales del EXE, no la sustituye. (Falta el bloque RESA-CURRENT-STATUS: lo anade la integracion.)

| Funcion | Entrada EXE | Bytes | Convencion | Pares | Fuente |
|---|---|---|---|---|---|
| CVehicle::DoHeadLightEffect | 0x6E0A50 | 962 | thiscall, `ret 0x10` (int32, CMatrix&, uint8, uint8) | 4096 | `Vehicle.cpp` + `VehHeadTailHead*Cpp` en `X87Math.cpp` |
| CVehicle::DoTailLightEffect | 0x6E1780 | 723 | thiscall, `ret 0x18` (int32, CMatrix&, uint8, uint8, uint32 sin uso, uint8) | 4096 | `Vehicle.cpp` + `VehHeadTailTail*Cpp` en `X87Math.cpp` |

Clasificacion (juego) y firmas corregidas: `g22b-classification.md`. Desensamblado: `original-disassembly.txt`. Residuo de registros: ningun llamador lee ECX/EDX ni mas que AL (`g22b-audit.txt`).

## Reconstruccion

Flujo y colaboradores en C++ (`Vehicle.cpp`); la aritmetica que el EXE deja en el x87 sin guardar a float (punto + frente*0.05, espejo `x - (x+x)`, `CMatrix*CVector` 0x59C890, direccion a la camara y
`CVector::NormaliseAndMag` 0x59C970, producto escalar, alfa/tamano, `_ftol2`) en 6 ayudantes `VehHeadTail*` de `X87Math.cpp` con `JpFixMemOp` (semantica de NaN de `fop dword [mem]`). No se usan los
`operator*` / `NormaliseAndMag` de la DLL (sumas distintas; el hook de CVector esta comentado). Hooks: `RH_ScopedInstall(DoHeadLightEffect, 0x6E0A50)` y `(DoTailLightEffect, 0x6E1780)`.

## Que compara cada par (original = EXE con hook apagado, candidato = DLL con hook activo, via `HookRoute`)

- Retorno (AL), traza completa de colaboradores con argumentos en bits, diferencias de memoria de toda la arena y de `TheCamera` (deben ser vacias: las funciones no escriben), ESP, EBX/ESI/EDI/EBP, FS:[0],
  fcw, TOP del fsw y pila x87 vacia (ftw = 0xFFFF), guardas de la arena y restauracion del estado (incluida la camara real y `ms_modelInfoPtrs`). Misma politica x87 que Dependencies21D: no se exigen las banderas
  sticky/codigos de condicion del fsw ni los registros ST muertos (en el ensayo sin juego difieren en ~1/3 de los casos, como en 21D); decision del propietario si se quiere endurecer.
- EAX alto, ECX y EDX no se comparan (ningun llamador los lee).
- Colaboradores falsos, en la direccion del EXE y en el cuerpo de la DLL (`EntryPatch` + `Resolve`): `CCoronas::RegisterCorona` (tipo, 0x6FC580; r, g, b y alfa como bytes, el resto como dwords), `FindPlayerVehicle`
  (0x56E0D0; devuelve nulo / el propio vehiculo / otro) y `CDamageManager::GetPanelStatus` (0x6C2180). 0x59C890, 0x59C970 y `_ftol2` son reales.
- Arena propia (`Fixtures.hpp`): un `CAutomobile` completo, tres matrices, un `CVehicleModelInfo` y `CVehicleStructure` falsos (el indice de modelo del caso se desvia temporalmente en `ms_modelInfoPtrs` y se
  restaura), manejo, conductor; la camara real se limpia, se rellena y se restaura entera. Todos los tamanos/desplazamientos se comprueban con `static_assert` (incl. `m_fLODDistMultiplier` = 0xF0).
- Fixtures: ~22% de valores de estres (NaN con carga distinta y senalizantes de ambos signos, +-inf, +-0, subnormales, +-1e30, FLT_MAX, umbrales +-ulp), indices de luz 0/1, `isRight` 0/1/2/5/0x80/0xFF,
  `disabledOrAlarm`, bytes altos de pila con basura, vehiculo con/sin matriz, camara con/sin matriz, matriz del argumento igual o distinta de la del vehiculo, camara activa 0..2 en modo 1a persona, modelos
  0x1B7/0x1DB/0x1C1, trenes, halogeno, freno/freno de mano/conductor. 45% de los casos son dirigidos: `range` (productos > FLT_MAX que se cancelan en el x87), `tiny`, `bigforward` (alfa y `_ftol2` fuera de rango
  de int), `camera-at-point` (suma de cuadrados 0), `threshold` (0.9/0.85 sobre la raiz, 40 sobre la distancia, 0 sobre el producto escalar).
- `run_local.py`: 8192 pares, todas las compuertas en ambos lados, memoria sin cambios, cobertura positiva (escenarios, indices, retornos, 0/1/2 coronas, tipos, tren, halogeno, matrices, primera persona con
  FindPlayerVehicle nulo/propio/otro, Stallion/Sabre con y sin parachoques danado, modos 0/1/2 de la cola, freno >0/0/<0, estres de float) y un oraculo independiente en Python: ids de corona, constantes de
  RegisterCorona, colores, tamano de la linea de luz, rango `150*LOD`, `disabledOrAlarm != 0 => false`, retorno de la cola = (modo != 0). Nunca se relaja una compuerta.

## Ensayo sin juego (`offline/`)

`rehearsal.cpp` mapea gta-sa.exe en un proceso hijo de 32 bits y ejecuta los bytes reales frente al texto de `Vehicle.cpp` (`extract_glue.py`, tipos falsos de `mock.h`) y `X87Math.cpp` con las opciones del
proyecto, con las MISMAS fixtures (`Fixtures.hpp`) y semillas del banco, y escribe un registro con el esquema de `LiveComparison.hpp`. Resultados (con la fuente entregada):
- Pares exactos del banco (4096 + 4096): 0 diferencias; `python run_local.py --analyze` los acepta (8192 pares).
- 400 000 casos mas por funcion (casos 4096..404095): 0 diferencias en cabeza y cola.
- `mutations.py`: 32 mutaciones del flujo y de los ayudantes x87; las 30 no equivalentes se detectan con los 4096 pares de cada funcion; 2 son equivalentes y se documentan (`(int)x` ya compila a `_ftol2` con
  `/arch:IA32`; `150 * subnormal` es exacto). `analyzer_mutations.py`: 13 alteraciones del registro, todas rechazadas por `run_local.py`.
- Compilacion de verificacion con los flags de Debug del proyecto: `Vehicle.cpp` parcheado, `Game.cpp` con este banco y `X87Math.cpp` (O2/IA32/fast) compilan sin errores; el auditor de residuo da 0 violaciones.

## Ejecucion

```
python -B reversing/Dependencies22B/build_probe.py
python -B reversing/Dependencies22B/run_local.py
python -B reversing/Dependencies22B/run_local.py --analyze <comparisons.jsonl>
```
o `python reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de evidencia> Dependencies22B`. Ensayo: `offline/` (`extract_glue.py`, `make_x87src.py`, `build.bat` con `NLOHMANN` apuntando al include de nlohmann).
