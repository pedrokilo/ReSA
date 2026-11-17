# Fn208A: 15 funciones de CPhoneInfo, CPickup, SurfaceInfos_c, CZoneDef, lectura de clump, CMemoryHeap, CMemoryMgr y user tracks (lote 208A)

Estado: **APROBADO dentro del juego** (16/11/2026, worktree `wv-6` desde origin/main 70b6d35891 + cherry-pick del lote 5850d9f9af; plantilla `Fn173A`, con `Fn95C` para los guiones de la lectura de clump). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn208a/20261116-202718-983824` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn208A-20261116-202718-983824`, `comparisons.jsonl.gz`). 15 x 16384 = **245760 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `7d10c0d6108b8df814e113e64f647d7cec4fa40beaf6542c0de86b65c870311e`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna correccion de fuente fue necesaria: el banco no encontro diferencias (incluidos los casos con coordenadas enormes de `CZoneDef::IsPointWithin`, donde se temia que el SSE del compilador difiriera de la x87 de 24 bits con exponente ampliado). Ninguna funcion excluida de las 15 con hook; las 6 direcciones saltadas del lote (0x72E080, 0x72E140, 0x72E220, 0x72E270, 0x72E360 con registros propios y 0x4F3A15, handler de catch) no tienen hook y no se prueban directamente (los ayudantes 0x72E080/0x72E270/0x72E360 se validan a traves de `RpClumpGtaStreamReadAtOnce`: corren reales en el EXE y como funciones privadas en la DLL).

| # | Funcion | Entrada | Colaboradores falseados |
|---|---|---|---|
| 0 | `CPhoneInfo::PhoneAnimFinishCallback` | 0x453F40 | CPad::GetPad 0x53FB70, CPed::SetPedState 0x5E4500 |
| 1 | `CPhoneInfo::Constructor` | 0x4541E0 | ninguno (el iterador del CRT corre real) |
| 2 | `CPickup::IsWeaponSlotExchangeable` | 0x454920 | ninguno |
| 3 | `SurfaceInfos_c::LoadSurfaceInfos` | 0x55EB90 | OpenFile 0x538900, LoadLine 0x536F80, CloseFile 0x5389D0, GetSurfaceIdFromName 0x55D220 |
| 4 | `SurfaceInfos_c::LoadSurfaceAudioInfos` | 0x55F2B0 | los mismos |
| 5 | `CZoneDef::IsPointWithin` | 0x72D850 | ninguno |
| 6 | `RpClumpGtaStreamReadAtOnce` | 0x72E3A0 | funciones de RenderWare (FindChunk, Read, ClumpCreate, FrameList, Geometry*, Atomic*, ClumpAddAtomic/Destroy) y CMemoryMgr::Malloc/Free |
| 7 | `CMemoryHeap::JoinAdjacentFreeBlocks` | 0x72E870 | ninguno |
| 8 | `CMemoryHeap::Shutdown` | 0x72EC60 | free() del CRT 0x82413F |
| 9 | `CMemoryHeap::FillInBlockData` | 0x72EC80 | JoinAdjacentFreeBlocks 0x72E870 |
| 10 | `CMemoryHeap::Malloc` | 0x72ED80 | FillInBlockData 0x72EC80 |
| 11 | `CMemoryHeap::Init` | 0x72F180 | CMemoryHeap::Malloc 0x72ED80 |
| 12 | `CMemoryMgr::Init` | 0x72F3B0 | ninguno |
| 13 | `UserTracksUninitCopy` | 0x4F3940 | ninguno |
| 14 | `UserTracksLegacyString::Reallocate` | 0x4F39C0 | operator new 0x82119A y operator delete 0x8214BD del EXE (el falso de new lanza una excepcion de C++ real) |

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D, guardas de 0x40) con direcciones reales anotadas en cada caso. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook, y solo en la direccion del EXE de los que la DLL llama por ella (free, operator new/delete). Los falsos de funciones de uso masivo (free, new, delete, CMemoryMgr::Malloc/Free) reenvian a la funcion original (trampolin) las llamadas de hilos ajenos al banco. `Reallocate` se ejecuta con `GateTry` (SEH): una excepcion que escapa (las dos reservas fallan) se anota como `threw`. Las cargas de superficies leen lineas dadas por el falso de `LoadLine` y corren el sscanf real de cada lado (CRT del EXE frente al de la DLL). Memoria global vigilada: marcador 0xA9B0B0 (`CMemoryMgr::Init`) y los globales de las fases de lectura de clump (0xC87AE8.., 0x8D607C, 0x8D6088, 0xC87FE8: deben quedar intactos). 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 245760 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn208A` (la corrida dura ~27 min; subir los timeouts como en `_v6_session.sh` si hace falta).

## Prueba negativa
`mutate.py` (15 mutaciones por ronda; las de funciones que el juego usa estan condicionadas a algo que el juego real no produce). **Las 15 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261116-201856-534159` (17213 de 30720 pares con diferencias) y ronda 2 `20261116-202242-551932` (12212 de 30720); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas).

## Fuera de los fixtures
`this` nulo o crudo, campos de texto mas largos que los buferes de `SurfaceInfo`, numeros fuera de int32 o NaN/inf en el texto, destino nulo con mas de un elemento en `UserTracksUninitCopy` (el EXE escribe en la direccion 4), contador negativo de geometrias con atomicos y vector de frames nulo en exito (el EXE desreferencia nulo), punteros nulos de RenderWare, y la exactitud de los colaboradores falseados.

## Bancos antiguos que citan las direcciones o dependen del cambio (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x55EB90 y 0x55F2B0 -> `Surface47` (**repetir**: las cargas ahora tienen hook); 0x72D850 -> `Zones21` y `OriginalDependencies499` (**repetir**: `IsPointWithin` ya no es una funcion en linea, ahora tiene hook y firma por valor); 0x72E3A0 -> `Fn95C` (la describe como funcion muerta; sin cambio de comportamiento, repetir por prudencia); 0x72F3B0 -> `Game22` (**repetir**: `CMemoryMgr::Init` tiene hook ahora). `CPickup::Update` (llama al predicado 0x454920 con hook, antes una lambda local) aparece en `PickWeap1`, `PickupsGenNew28`, `CPedB`, `HudCamFinal2` e `InteriorLifecycleRuntime`: **repetir** los que ejecuten `CPickup::Update`. Sin ningun banco: 0x453F40, 0x4541E0, 0x72E870, 0x72EC60, 0x72EC80, 0x72ED80, 0x72F180, 0x4F3940, 0x4F39C0.
