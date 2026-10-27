<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ReplUpgrade72: CVehicle::AddReplacementUpgrade (0x6D3830, lote 72)

Estado: **APROBADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-3`, HEAD 95f7d73eb + `source.patch`). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-replupgrade7261026-204732-062452`. (Una corrida previa, `20261026-204625-143815`, se detuvo por un defecto del banco: fixtures con marco original del atomo nulo, que la DLL aserta en `RwFrameDestroy`; en el juego nunca ocurre.)

`source.patch`: solo `Vehicle.cpp` (`git apply --check` contra HEAD 95f7d73eb).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| AddReplacementUpgrade | 0x006D3830 | CVehicle,AddReplacementUpgrade | S |

Fila de `docs/hooks.csv` (la anade el coordinador): `CVehicle,AddReplacementUpgrade,0x006D3830,1,0,1,S`. No es virtual.

## Que hace (desensamblado 0x6D3830..0x6D39D4, 421 bytes; la siguiente funcion es 0x6D39E0 RemoveReplacementUpgrade)

thiscall con `ret 8`, sin valor de retorno (ningun llamador lee EAX/ECX/EDX: los cinco `call` estan en AddVehicleUpgrade 0x6E3290 y solo conservan ESI/EDI/EBX/EBP).

1. `frame = GetFrameFromId([this+0x18], nodeId)` (0x4C53C0). `flags = 0`; `RwFrameForAllObjects(frame, RemoveObjectsCB 0x6D33B0, &flags)` y `RwFrameForAllChildren(frame, 0x6D3420, &flags)` (vacian el componente y dejan en `flags` el id del ultimo atomo quitado).
2. `mi = ms_modelInfoPtrs[modelIndex]`; `atomo = mi->CreateInstance()` (vtable +0x2C); `mi->AddRef()` (0x4C4BA0); el marco original del atomo (`atomo+4`) se lee antes de `RpClumpAddAtomic`, `RpAtomicSetFrame(atomo, frame)` y `RwFrameDestroy(marco original)`.
3. `SetAtomicId(atomo, flags & ~3)`, `SetAtomicFlag(atomo, 1)` y el bloque 0x6D38D5..0x6D390B (`SetupUpgradeAtomicRendering(atomo, false)`: HasAlphaMaterialCB sobre la geometria, callback de render HiDetail o HiDetailAlpha, `SetRenderPipelinesCB(atomo, 0)`); `ms_bCreateDamagedVersion` (0xA9B0B0) = 0.
4. `nodeId == 0x13` y bit 0x2000 de `[manejabilidad+0xCC]` (`m_bDoubleExhaust`): `r = CreateReplacementAtomic(mi, frame, flags, false, true)`; `x` del marco de `r` = `x` del marco del componente * `-2.0f` (0x858B18, leida tras la llamada) y `RwMatrixUpdate`. Con `nodeId` 0xC o 0xD: `SetupClumpAfterVehicleUpgrade(clump, info->m_pPlateMaterial, info->m_nPlateType)` con el model info del vehiculo (`(int16)[this+0x22]`).
5. Si `mi->AsDamageAtomicModelInfoPtr()` (vtable +8; el EXE prueba EAX completo) no es nulo: `CreateReplacementAtomic(mi, frame, flags, true, false)` y, con `frame` no nulo, `RwFrameForAllObjects/Children(frame, SetVehicleAtomicVisibilityCB, 1)`.

## Cambios de produccion (`source.patch`)

`Vehicle.cpp`: el cuerpo (antes una llamada a 0x6D3830) pasa a C++ puro (sin `__asm`/`naked`), reutilizando `SetupUpgradeAtomicRendering` (el bloque 0x6D38D5..0x6D390B ya identificado) y `CreateReplacementAtomic`; se activa `RH_ScopedInstall(AddReplacementUpgrade, 0x6D3830)`. Sin cambios en `.h`.

## Banco

`Original` = bytes del EXE con el hook apagado (entrada 0x6D3830 intacta); `candidato` = DLL con SOLO ese hook encendido (la entrada salta al cuerpo del ASI, comprobado en cada lado: `route_ok`). Se llama a la entrada del EXE con ECX = vehiculo falso y la pila sembrada; se exigen las compuertas de ABI (`ret 8`, ESI/EDI/EBX/EBP conservados, DF limpia, SEH), x87 (FCW/TOP/ST validos) y restauracion de globales. EAX/ECX/EDX solo se registran.
Colaboradores (15) sustituidos por falsos identicos en la direccion del EXE y, si tienen hook, en el cuerpo de la DLL (los que tienen hook quedan apagados): GetFrameFromId 0x4C53C0, RwFrameForAllObjects 0x7F1200, RwFrameForAllChildren 0x7F0DC0, CBaseModelInfo::AddRef 0x4C4BA0, RpClumpAddAtomic 0x74A490, RpAtomicSetFrame 0x74BF20, RwFrameDestroy 0x7F05A0, SetAtomicId 0x732290, SetAtomicFlag 0x7322B0, RpGeometryForAllMaterials 0x74C790, SetAtomicRenderCallback 0x7328A0, SetRenderPipelinesCB 0x4C88F0, CreateReplacementAtomic 0x6D3700, RwMatrixUpdate 0x7F18A0 y SetupClumpAfterVehicleUpgrade 0x6FDFE0. `CreateInstance` y `AsDamageAtomicModelInfoPtr` son entradas de una vtable falsa (las demas ranuras registran un evento 99). Los falsos registran sus argumentos EN ORDEN (los callbacks se normalizan a un identificador igual en EXE y DLL; los punteros a la arena, a un desplazamiento); RpAtomicSetFrame escribe el marco en `atomo+4` y los de RwFrameForAll* escriben el `flags` de `data`, RpGeometryForAllMaterials un byte.
Vehiculo, model infos (ms_modelInfoPtrs[15..19]), manejabilidad, marcos, atomos y clump son FALSOS en una arena con guardas; se compara por oraculo independiente (`fixture_oracle.py`, escrito desde el desensamblado) la lista de eventos, los cambios de la arena y el byte final de `ms_bCreateDamagedVersion`, y ademas EXE contra DLL fila a fila.
16384 pares = 16 familias de 1024 (nodo 0x13 con/sin bandera 0x2000, 0xC, 0xD, otros nodos; marco nulo/arena/ajeno; id de atomo escrito por los dos recorridos o no; con/sin alfa; con/sin atomo danado (incluidos retornos solo con bits altos); x del componente cero/entero/normal/enorme (desborde)/infinito/NaN, `ms_bCreateDamagedVersion` inicial 0/1/otro, clump nulo).
`coverage.json` = minimos de ramas (50 % de lo medido con `fixture_mirror.py`). `fixture_oracle.py` autoprueba: `python -I fixture_oracle.py`.

## Limites

No se prueba: `modelIndex` fuera de ms_modelInfoPtrs[15..19], marco original del atomo nulo (la DLL aserta en `RwFrameDestroy`; en el juego CreateInstance siempre devuelve un atomo con marco), denormales (MXCSR FTZ), dos NaN, ni el cuerpo real de los colaboradores sustituidos (los validan sus propios bancos).
Dependencias: AddVehicleUpgrade (0x6E3290) llama a esta direccion cinco veces y sus bancos (Vehicle66, lo que llame a AddUpgrade/ProcessMods) la usan como falso o como llamada real: la integracion debe repetirlos.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh ReplUpgrade72   # compila la sonda y corre en el juego (candado global)
python -B reversing/ReplUpgrade72/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/ReplUpgrade72/fixture_oracle.py ; python -I reversing/ReplUpgrade72/fixture_mirror.py
python -B reversing/ReplUpgrade72/register_hook.py <carpeta-de-evidencia>                      # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `replupgrade72-runtime.start`; salida `replupgrade72-runtime.jsonl`.
