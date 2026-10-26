<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies23A: CVehicle::DoVehicleLights (0x6E1A60)

PENDIENTE: banco preparado y compilado contra el arbol (comprobacion de sintaxis con las opciones del proyecto), pero NO ejecutado dentro del juego. No constituye validacion hasta que `run_local.py`
apruebe una corrida real. El ensayo sin juego (`offline/`) es evidencia previa con los bytes reales del EXE y no la sustituye. (Falta el bloque RESA-CURRENT-STATUS: lo anade la integracion.)

| Funcion | Entrada EXE | Bytes | Convencion | Pares | Fuente |
|---|---|---|---|---|---|
| CVehicle::DoVehicleLights | 0x6E1A60 | 3737 | thiscall, `ret 8` (CMatrix&, eVehicleLightsFlags = dword) | 8192 | `Vehicle.cpp` + `VehLights*` / `VehHeadTail*` en `X87Math.cpp` |

Clasificacion (juego) y firma verificada: `g23a-classification.md`. Desensamblado: `original-disassembly.txt`. Residuo de registros: ningun llamador lee EAX/ECX/EDX (`g23a-audit.txt`).

## Que compara cada par (original = EXE con hook apagado, candidato = DLL con hook activo, via `HookRoute`)
- Traza ordenada de colaboradores con argumentos en bits (los `uint8` como byte, los dwords completos), diferencias de memoria de toda la arena, de `TheCamera` y de los tres globales del EXE que lee/escribe la
  funcion (`CTimer::m_snTimeInMilliseconds` 0xB7CB84, `CTimer::ms_fTimeStep` 0xB7CB5C, `CVehicle::ms_forceVehicleLightsOff` 0xC1CC18), ESP, EBX/ESI/EDI/EBP, FS:[0], FCW y TOP.
- Estado x87 completo (`kCompareDeadX87Registers = true`, sin relajar): FNSAVE tras cada lado, pila vacia (`ftw == 0xFFFF`), FSW completa (C0..C3 y IE/DE/ZE/OE/UE/PE) y los diez bytes de los ocho registros ST, vacios incluidos.
  Antes de cada llamada la pila se carga con un residuo distinto por registro y con uno de cuatro FSW iniciales (`BuildPrime`). FIP/FCS/FDP/FDS/FOP no se comparan (cada lado ejecuta su ultima instruccion x87 en otra direccion).
- EAX, ECX y EDX no se comparan: la funcion es `void` y ningun llamador los lee.
- Colaboradores falsos (EntryPatch en la direccion del EXE y en la entrada de la DLL, mismo falso): `CCoronas::RegisterCorona` 0x6FC580, `FindPlayerVehicle` 0x56E0D0, `CVehicle::GetVehicleLightsStatus` 0x6D55C0,
  `CDamageManager::GetLightStatus` 0x6C2130, `CVehicle::DoHeadLightEffect` 0x6E0A50, `CVehicle::DoTailLightEffect` 0x6E1780, `CVehicle::DoHeadLightReflection` 0x6E1720 y `CPointLights::AddLight` 0x7000E0.
  Son reales en los dos lados: `CMatrix * CVector` 0x59C890, `CVector::NormaliseAndMag` 0x59C970, `_ftol2` 0x821B40 y el maximo 0x420800 (en la DLL, bloques de `X87Math.cpp`).
- El EXE trae DOS COPIAS EN LINEA de `DoHeadLightEffect(lightId = 1)` (0x6E1F31..0x6E229F y 0x6E22C2..0x6E263B) que NO llaman a 0x6E0A50 (por eso el falso de 0x6E0A50 solo ve `lightId = 0`). La DLL las ejecuta con
  `VehicleHeadLightEffect(*this, true, 1, ...)` (funcion estatica de `Vehicle.cpp`, no pasa por la entrada parcheada) y sus `RegisterCorona` / `FindPlayerVehicle` van a los mismos falsos. Las copias en linea difieren de la
  funcion en un unico punto del x87: el tamano de la corona se calcula `((1 - d/150) * sqrtDot) * 0.4` (la funcion `((1 - d/150) * 0.4) * sqrtDot`); `inlinedCopy` elige el orden.
- Arena propia (`Fixtures.hpp`): un `CAutomobile` completo, tres matrices, un `CVehicleModelInfo` y `CVehicleStructure` falsos (el indice de modelo del caso se desvia temporalmente en `ms_modelInfoPtrs` y se restaura),
  manejo, conductor y otro vehiculo; la camara real se limpia, se rellena y se restaura entera. `static_assert`s y una prueba de contrato comprueban desplazamientos, bits de banderas, estado, `m_nOverrideLights`, `m_renderLights`.
- Fixtures: escenarios (abandonado con distancia cerca de 100, ZR-350 subida/bajada con el umbral 0.698 y paso 0.01, velocidad cerca de 0.2025, alarma, freno, anulacion de luces) y dirigidos de geometria (`range`, `tiny`,
  `bigforward`, `camera-at-point`, `threshold`), ~22% de valores de estres (NaN con carga distinta y senalizantes, +-inf, +-0, subnormales, +-1e30, FLT_MAX, +-1 ulp de los umbrales).
- `run_local.py`: compuertas, `compare_dead_x87_registers`, oraculo independiente en Python (llamadas a colaboradores y argumentos, bits de `m_renderLights`, constantes de coronas y AddLight, niebla, luz de freno) y
  cobertura por rutas (`GROUPS`): el minimo exigido es la MITAD de lo que alcanza el ensayo sin juego con los mismos 8192 casos (margen >= 2, por encima del 1,5 exigido; en juego se alcanza algo menos porque `std::sin/cos`
  de las fixtures cambian). Nunca se relaja una compuerta.

## Reconstruccion
`Vehicle.cpp`: `DoVehicleLights` (flujo, banderas, orden de colaboradores; hook `RH_ScopedInstall(DoVehicleLights, 0x6E1A60)`), `VehicleHeadLightEffect` (cuerpo comun de `DoHeadLightEffect` y de las copias en linea) y
`DoHeadLightEffect` (delgada). `X87Math.cpp`: bloques `VehLights*` y `VehHeadTail*` (cada tramo recto del EXE: levanta las mismas banderas, deja C0/C2/C3 del ultimo `fcomp`, planta los registros muertos con
`PointLightsPlantN` y termina con el mismo `fstp dword` cuando el EXE termina asi, para C1). Los comparadores se escriben con el operando izquierdo = ST0 del EXE (el codigo de `NormaliseMag` se cambio a `fld suma; fcom 0`).

## Ensayo sin juego (`offline/`)
`rehearsal.cpp` mapea gta-sa.exe en un proceso hijo de 32 bits y ejecuta los bytes reales frente al texto de `Vehicle.cpp` (`extract_glue.py`, tipos falsos de `mock.h`) y `X87Math.cpp` con las opciones del proyecto, con las
MISMAS fixtures y semillas del banco; `build.bat` lo compila (`NLOHMANN` = include de nlohmann). Resultados con la fuente entregada: 8192 pares exactos del banco y 120000 casos mas (10000..130000): 0 diferencias en traza,
memoria, ABI, FSW y registros muertos; modo `head` (`rehearsal.exe N 0 - 0 head`: `0x6E0A50` frente a `DoHeadLightEffect`, mismas comparaciones x87): 60000 casos, 0 diferencias. `python run_local.py --analyze` acepta el
registro de los 8192 pares. `mutate.py`: 24 mutaciones del flujo y de los bloques; las 21 no equivalentes que alcanzan el estado final se detectan; 3 son equivalentes en `DoVehicleLights` (la planta de T-1 de PropFall, y las
de `VehHeadTailLimitCpp`/ T-3 de la corona, que el siguiente bloque sobrescribe siempre; la de la corona se detecta en modo `head`, y las de Limit se retiraron). `analyze_diff.py`, `dec.py` y `x87sym.py` (simulador simbolico
de la pila x87 sobre el desensamblado) son las herramientas de diagnostico.

## Ejecucion
```
python -B reversing/Dependencies23A/build_probe.py
python -B reversing/Dependencies23A/run_local.py
python -B reversing/Dependencies23A/run_local.py --analyze <comparisons.jsonl>
python reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpeta de evidencia> Dependencies23A
```
Antes: aplicar `g23a.patch`, anexar `g23a-x87.cpp.txt` / `g23a-x87.h.txt` al final de `X87Math.cpp` / `X87Math.h`, y compilar el proyecto.
