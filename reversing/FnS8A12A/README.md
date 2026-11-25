<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnS8A12A: lotes S8A, S9A, S10A, S11A y S12A del plan de sustitucion (20/11/2026)

Plan: [docs/ExeIndependiente/plan-sustitucion.md](../../docs/ExeIndependiente/plan-sustitucion.md), raices [raices.csv](../../docs/ExeIndependiente/raices.csv), lotes [lotes-sustitucion.csv](../../docs/ExeIndependiente/lotes-sustitucion.csv).
Mismo metodo que [FnS7A1A](../FnS7A1A/README.md): cada funcion del EXE que el codigo propio llamaba por direccion fija (callbacks de plugin, callbacks de esquemas de interpolacion, envolturas) se reconstruye en C++
puro desde el desensamblado (capstone), se engancha con `RH_ScopedGlobalInstall` en su direccion y el codigo propio pasa a usar la funcion propia. Worktree `wv-18`, rama `s8a12a`, sobre `origin/main`. Sin push.

Estado: **77 de 77 raices reconstruidas y enganchadas (+2 callbacks extra); compilacion Debug completa OK; sin banco en juego** (decision del 20/11: reconstruir y enganchar, sin validar en juego).
Las 79 filas nuevas de `docs/hooks.csv` quedan con `validated=0`. Los `XxxInjectHooks()` ya existian (`RpUVAnimInjectHooks`, `RpHAnimInjectHooks`, `RpSkinInjectHooks` en `source/InjectHooksMain.cpp`).

## Tabla

| Lote | Funcion | Direccion | Fuente | Verificacion sin juego |
|---|---|---|---|---|
| S8A | `RpUVAnimEngineConstructor` | 0x7CB9B0 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimEngineDestructor` | 0x7CBA20 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimMaterialConstructor` | 0x7CBA80 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimMaterialCopy` | 0x7CBB30 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimMaterialStreamGetSize` | 0x7CBF20 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimAddRef` | 0x7CC100 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimGetName` | 0x7CC3A0 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpMaterialUVAnimGetInterpolator` | 0x7CC430 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpMaterialUVAnimSetInterpolator` | 0x7CC450 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimParamKeyFrameMulRecip` | 0x7CC740 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimParamKeyFrameAdd` | 0x7CC750 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimKeyFrameStreamRead` | 0x7CC870 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S8A | `RpUVAnimKeyFrameStreamWrite` | 0x7CC920 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpUVAnimMaterialDestructor` | 0x7CBAA0 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpUVAnimMaterialStreamRead` | 0x7CBC20 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpUVAnimCreate` | 0x7CC020 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpMaterialUVAnimSetCurrentTime` | 0x7CC470 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpMaterialUVAnimSubAnimTime` | 0x7CC4F0 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpUVAnimParamKeyFrameApply` | 0x7CC560 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpUVAnimParamKeyFrameInterpolate` | 0x7CC600 | `RenderWare/rw/rpuvanim.cpp` | `emu_x87.py`: 0 diferencias |
| S9A | `RpUVAnimParamKeyFrameBlend` | 0x7CC6A0 | `RenderWare/rw/rpuvanim.cpp` | `emu_x87.py`: 0 diferencias |
| S9A | `RpUVAnimLinearKeyFrameApply` | 0x7CC9F0 | `RenderWare/rw/rpuvanim.cpp` | no comprobada |
| S9A | `RpUVAnimLinearKeyFrameInterpolate` | 0x7CCA40 | `RenderWare/rw/rpuvanim.cpp` | `emu_x87.py`: 0 diferencias |
| S9A | `RpUVAnimLinearKeyFrameBlend` | 0x7CCAC0 | `RenderWare/rw/rpuvanim.cpp` | `emu_x87.py`: 0 diferencias |
| S9A | `RpUVAnimLinearKeyFrameMulRecip` | 0x7CCB30 | `RenderWare/rw/rpuvanim.cpp` | `emu_x87.py`: 0 diferencias |
| S9A | `RpUVAnimLinearKeyFrameAdd` | 0x7CCBE0 | `RenderWare/rw/rpuvanim.cpp` | `emu_x87.py`: 0 diferencias |
| S10A | `RpHAnimHierarchySetFreeListCreateParams` | 0x7C45E0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimEngineConstructor` | 0x7C4670 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimEngineDestructor` | 0x7C4720 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimFrameConstructor` | 0x7C4750 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimFrameCopy` | 0x7C4830 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimFrameStreamWrite` | 0x7C48D0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimFrameStreamGetSize` | 0x7C4BF0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimHierarchyCreateFromHierarchy` | 0x7C4ED0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimHierarchyAttach` | 0x7C4F40 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimHierarchyDetach` | 0x7C4FF0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimHierarchyAttachFrameIndex` | 0x7C5020 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimHierarchyDetachFrameIndex` | 0x7C5100 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S10A | `RpHAnimFrameSetHierarchy` | 0x7C5130 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimFrameDestructor` | 0x7C4770 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimFrameStreamRead` | 0x7C4A00 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimHierarchyDestroy` | 0x7C4D30 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimHierarchyCreateSubHierarchy` | 0x7C4DB0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimFrameSetID` | 0x7C5170 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimFrameGetID` | 0x7C5190 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimKeyFrameApply` | 0x7C5B80 | `RenderWare/rw/rphanim.cpp` | `emu_x87.py`: 0 diferencias |
| S11A | `RpHAnimKeyFrameInterpolate` | 0x7C5CA0 | `RenderWare/rw/rphanim.cpp` | `emu_x87.py`: 0 diferencias |
| S11A | `RpHAnimKeyFrameBlend` | 0x7C60C0 | `RenderWare/rw/rphanim.cpp` | `emu_x87.py`: 0 diferencias |
| S11A | `RpHAnimKeyFrameStreamWrite` | 0x7C6540 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimKeyFrameStreamGetSize` | 0x7C65B0 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| S11A | `RpHAnimKeyFrameMulRecip` | 0x7C65C0 | `RenderWare/rw/rphanim.cpp` | `emu_x87.py`: 0 diferencias |
| S11A | `RpHAnimKeyFrameAdd` | 0x7C6720 | `RenderWare/rw/rphanim.cpp` | `emu_x87.py`: 0 diferencias |
| S12A | `RpSkinSetFreeListCreateParams` | 0x7C67F0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinEngineConstructor` | 0x7C68E0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinEngineDestructor` | 0x7C69A0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinGeometryConstructor` | 0x7C69F0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinGeometryDestructor` | 0x7C6A10 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinGeometryCopy` | 0x7C6A70 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicConstructor` | 0x7C6A80 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicDestructor` | 0x7C6AA0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicCopy` | 0x7C6AC0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicStreamAlwaysCallBack` | 0x7C6AE0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinGeometryStreamGetSize` | 0x7C6BE0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinGeometryStreamWrite` | 0x7C6C30 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinGeometryStreamRead` | 0x7C6D20 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicStreamRead` | 0x7C72A0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicStreamWrite` | 0x7C7500 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicStreamGetSize` | 0x7C7510 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinDestroy` | 0x7C77A0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinIsSplit` | 0x7C7820 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicSetType` | 0x7C7830 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinAtomicGetType` | 0x7C7880 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RpSkinUsedBoneCompare` | 0x7C8720 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `_rpSkinGetMeshBoneRemapIndices` | 0x7C8A40 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `_rpSkinGetMeshBoneRLECount` | 0x7C8A50 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `_rpSkinGetMeshBoneRLE` | 0x7C8A60 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| S12A | `RxNodeDefinitionGetD3D9SkinAtomicAllInOne` | 0x7CB2A0 | `RenderWare/rw/rpskin.cpp` | no comprobada |
| (extra) | `RpHAnimAttachChildCallBack` | 0x7C4F90 | `RenderWare/rw/rphanim.cpp` | no comprobada |
| (extra) | `RpHAnimAttachIndexChildCallBack` | 0x7C50A0 | `RenderWare/rw/rphanim.cpp` | no comprobada |

## Que cambia en el codigo propio (raices cerradas)
- `RpUVAnimPluginAttach`, `RpHAnimPluginAttach` y `RpSkinPluginAttach` registran sus funciones propias (antes `reinterpret_cast` de direcciones del EXE: 0x7CB9B0.., 0x7C4670.., 0x7C68E0..).
- `RpHAnimEngineConstructor` (0x7C4670) registra el esquema de interpolacion 1 con las funciones propias de fotograma (antes 0x7C5B80, 0x7C60C0, 0x7C5CA0, 0x7C6720, 0x7C65C0, 0x7C64C0, 0x7C6540, 0x7C65B0).
- `RpHAnimHierarchyAttach` y `RpHAnimHierarchyAttachFrameIndex` pasan como callback de `RwFrameForAllChildren` sus callbacks propios (0x7C4F90 y 0x7C50A0, que no figuraban en el CSV de raices: son las +2 extra).
- `RpMaterialUVAnimApplyUpdate` ya no compara con las direcciones 0x7CC9F0/0x7CC560: compara el callback de la interpolacion con el de su esquema (`keyFrameApplyCB` de 0x8DED84 / 0x8DEDB4, que en el EXE vale esas direcciones).
- `_rpSkinInitialize` (ordena los huesos usados con qsort) llama a `ExeCrt::Qsort` con `RpSkinUsedBoneCompare` propia (antes 0x8247E0 y 0x7C8720).
- `RxNodeDefinitionGetD3D9SkinAtomicAllInOne` (0x7CB2A0) la usa `_rpSkinD3D9CreateGenericPipeline`.
- Las tablas estaticas del EXE (esquemas lineal/parametrico de UVAnim en 0x8DED84/0x8DEDB4, definicion de nodo 0x8DED08, esquema de diccionario 0x8DED50) siguen siendo datos del EXE: contienen punteros a direcciones del EXE
  que ahora saltan a la funcion propia por el hook (los datos no se reescriben).

## Verificacion sin juego
- **`emu_x87.py`** (Unicorn sobre los bytes del EXE con la palabra de control x87 a 24 bits, 1500+ casos por funcion con ceros, unos, cuaterniones casi iguales/opuestos y todas las ramas del acos en linea): las 11 funciones x87
  (`UVAnim{Linear,Param}KeyFrame{Blend,Interpolate}`, `UVAnimLinearKeyFrameAdd/MulRecip`, `HAnimKeyFrame{Apply,Add,MulRecip,Interpolate,Blend}`) coinciden bit a bit con un modelo en Python que repite operacion a operacion las
  formulas de `X87Math.cpp` (cada operacion redondeada a float32). Se comprobo con dos mutaciones del modelo que el arnes detecta diferencias. `_rwSqrt` (0x7EDB30), que necesita estado del motor, se sustituye en la
  emulacion por `fsqrt`. **No prueba el binario C++ compilado** (solo lectura del desensamblado y formulas): eso requiere un banco en el juego.
- El resto (flujos, constructores y destructores, listas libres, jerarquias) solo tiene la lectura del desensamblado; **no se comprobo** (necesitaria falsos de RenderWare como los de `FnS7A1A/emu_check.py`).

## Lo que no se pudo / diferencias asumidas
- **Casos no reproducibles** (pila sin inicializar o fallo de reserva que en el EXE cuelga el juego): `HAnimKeyFrameMulRecip` con norma <= 0 o NaN (valores de pila: aqui ceros); `RpSkinEngineConstructor` con `RwMalloc` NULL (no se rellena
  a cero); `RpSkinGeometryStreamRead` y `RpSkinAtomicStreamRead` con la lista libre agotada o `_rpSkinCreateData` fallida (devuelven NULL en vez de seguir con el skin a NULL); buffer de huesos usados de 256 bytes (pila sin
  inicializar en el EXE, aqui ceros: se vuelve a calcular).
- `RpUVAnimCreate` copia `numNodes` dwords del mapa nodo-canal sin limite (mas de 8 desbordan el bloque de 0x44 bytes) igual que el EXE. `RpHAnimHierarchyCreateSubHierarchy`: la pila de indices del EXE es de 28 enteros; aqui 1024.
- `RpSkinAtomicStreamRead`: con la geometria ya con skin el EXE salta el quinto argumento (`sizeInObject`), no `binaryLength`; se conserva.
- `RpUVAnimMaterialStreamRead` y `RpHAnimFrameStreamRead` ignoran `binaryLength`; el nombre de animacion UV se lee en un buffer de 0x20 bytes sin terminador garantizado (como el EXE).
- Las lecturas con denormales/desbordamiento de la x87 (rango de exponente ampliado) no se modelan en `emu_x87.py` (el codigo C++ usa los mismos operadores sobre `double` dentro de `X87Math.cpp`, compilado con `/arch:IA32`).
- Lo que debe repetirse si algun dia se valida en juego: los bancos que citen estas direcciones (los callbacks registrados ahora son direcciones de la DLL, no del EXE).
