<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote R3A: reconstruccion sin validar

Rama `reconstruccion-sin-validar`, worktree `wv-7`. Veintiocho direcciones del encargo, de RenderWare 3.7 enlazado en el EXE (no son del juego): plugin RpSkin (`rpskin`) y modulo RtBMP (`rtbmp`). **No hay banco**: no se ha ejecutado el juego ni se ha tocado `approvals.json`, `validation-registry` ni `gta_lock`. En `docs/hooks.csv` entran con `validated=0`. Cada cuerpo se transcribio del desensamblado (`dumpbin /disasm` del EXE) instruccion por instruccion; ninguna usa SEH ni registros no estandar (todas cdecl, resultado en EAX).

Ninguna de las direcciones estaba enganchada en la rama (`git grep` y `docs/hooks.csv`). Muchas eran envolturas por direccion fija (`0x7C6820`, `0x7C7520`..`0x7C7560`, `0x7C75B0`, `0x7C77E0`..`0x7C7810`, `0x7C8740`, `0x7C8A70`, `0x7C8B10`, `0x7CDF60`) y se sustituyeron por el cuerpo en C++; el resto no tenia envoltura y se creo. `_rpSkinDeinitialize` (0x7C8820, de otro lote, ya validada) solo se llama.

Clases: RpSkin (rpskin.cpp/h, `RpSkinInjectHooks`) y RtBMP (rtbmp.cpp/h, funcion nueva `RtBMPInjectHooks` declarada en `RenderWare.h` y llamada desde `InjectHooksMain.cpp`). `0x7C8D40` y `0x7C9680` son de RpSkin y `0x7CE5D0` es de RtBMP (ayudante de `RtBMPImageRead`): la confianza baja del mapa se resolvio por llamadores y desensamblado. Los nombres de las funciones internas son descriptivos, deducidos del comportamiento; el listado no los trae. `struct RpSkin` (0x40 bytes) se define en `rpskin.h` con los campos deducidos.

## Funciones

| Direccion | Nombre | Bytes | Tipo | Archivo | Riesgos que debe mirar el banco |
| --- | --- | ---: | :---: | --- | --- |
| `0x007C6820` | `RpSkinPluginAttach` | 183 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Registra el plugin 0x116 (motor, atomic, geometria) y guarda solo los tres desplazamientos (0xC978A0/A4/A8). Orden de las siete llamadas y argumentos exactos; callbacks no reconstruidos por direccion fija (0x7C68E0, 0x7C69A0, 0x7C6A80, 0x7C6AA0, 0x7C6AC0, 0x7C72A0, 0x7C7500, 0x7C7510, 0x7C6AE0, 0x7C69F0, 0x7C6A10, 0x7C6A70, 0x7C6D20, 0x7C6C30, 0x7C6BE0); el de derechos es el C++ de 0x7C6B70. Devuelve TRUE. |
| `0x007C6B70` | `_rpSkinAtomicStreamRightsCallBack` | 98 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Callback de derechos del flujo del atomic (object, offset, size, extraData). Colaboradores: RwEngineGetPluginOffset(0x120) y (0x12E) en ese orden (el segundo solo si el primero no cumple), luego 0x7C89B0. Probar extraData 1, 2, 3 con y sin los plugins 0x120/0x12E, geometria NULL y sin skin. |
| `0x007C70C0` | `_rpSkinFindUsedBones` | 205 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Pesos comparados como dword != 0 (-0.0f cuenta), desplazamiento (8*b)&31, busqueda lineal sin repetidos, escribe *numBones. maxNumWeightsPerVertex sin signo desde skin+0x10. Sin x87. |
| `0x007C7190` | `_rpSkinCreateData` | 269 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Reserva (RwMalloc 0x30116) de numBones*64+numVertices*20+usados+15, alineado a 16 con `and al,0F0h`, copias en orden usados/matrices (de la ultima a la primera)/indices/pesos. Fallo de reserva: skin+0x3C = NULL y FALSE sin liberar. Comparar el bloque completo y los punteros +0x08..+0x18. |
| `0x007C7520` | `RpSkinAtomicSetHAnimHierarchy` | 18 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Escribe en atomic + [0xC978A4]; devuelve el atomic (EAX = argumento 1). |
| `0x007C7540` | `RpSkinAtomicGetHAnimHierarchy` | 14 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lee atomic + [0xC978A4]. |
| `0x007C7550` | `RpSkinGeometryGetSkin` | 14 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lee geometria + [0xC978A8]. |
| `0x007C7560` | `RpSkinGeometrySetSkin` | 69 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Mismo skin: sin tocar nada. Otro: 0x7C8820 (C++) si habia, guarda, 0x7C8740 (C++) si no NULL; si esta devuelve NULL, devuelve NULL. Colaboradores reconstruidos en este lote. |
| `0x007C75B0` | `RpSkinCreate` | 494 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lista libre 0xC978B8 (hint 0x30116) sin comprobar; bucle de maxNumWeightsPerVertex sobre dwords de pesos (arranca en 1, tope 4, corta en 4); busqueda de huesos con array local de 256 (0x7C70C0); reserva, copias y fallo (RwFreeListFree del skin y NULL). Revisar el bloque completo (0x40 + datos) y el orden de las llamadas al motor. Sin x87. |
| `0x007C77E0` | `RpSkinGetNumBones` | 7 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lee skin+0. |
| `0x007C77F0` | `RpSkinGetVertexBoneWeights` | 8 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lee skin+0x18. |
| `0x007C7800` | `RpSkinGetVertexBoneIndices` | 8 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lee skin+0x14. |
| `0x007C7810` | `RpSkinGetSkinToBoneMatrices` | 8 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Lee skin+0x0C. |
| `0x007C86F0` | `_rpSkinPipelinesDestroy` | 45 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Cuatro pipelines en 0xC978C4..0xC978D4: _rxPipelineDestroy (0x805820) de los no nulos y a NULL; devuelve TRUE. |
| `0x007C8740` | `_rpSkinInitialize` | 213 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | x87 solo en el intercambio del peso inferior (fld/fstp de 32 bits: un NaN senalizador sale silencioso; emulado por bits). Comparaciones como ENTEROS con signo de los dwords de los pesos, burbuja repetida hasta pasada sin cambios, permutacion simultanea de los 4 bytes de indices. qsort del EXE (0x8247E0, comparador 0x7C8720) sobre usedBonesArray (numUsedBones). Con skin NULL el EXE se cae (se conserva). Probar pesos ya ordenados, desordenados, iguales, >= 1.0f, negativos, NaN. |
| `0x007C8830` | `_rpSkinNativeGeometryGetSize` | 3 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | `xor eax,eax; ret`: devuelve 0. |
| `0x007C8840` | `_rpSkinNativeGeometryWrite` | 101 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Colaboradores: _rwStreamWriteVersionedChunkHeader(stream, 1, -12, 0x36003, 0xFFFF), RwStreamWriteInt32 del 9, RwStreamWriteInt32 del primer dword del skin (numBones). Devuelve el flujo o NULL. |
| `0x007C88B0` | `_rpSkinNativeGeometryRead` | 202 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | RwStreamFindChunk(1), version 0x34000..0x36003 (si no: RwError pluginID 0x116, _rwerror(0x80000004), RwErrorSet), marcador 9, skin de lista libre (0x30116) a cero + lectura de numBones; si falla, NULL y el skin no se libera; RpSkinGeometrySetSkin (C++) y devuelve el flujo. El EXE reutiliza el hueco de pila del argumento 1 para la version (sin efecto observable). |
| `0x007C8980` | `_rpSkinPipelinesCreate` | 34 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Solo el byte bajo de flags; bit 0: 0x7CB2B0 -> 0xC978C4 (aunque sea NULL; entonces FALSE). |
| `0x007C89B0` | `_rpSkinPipelineAtomicAttach` | 14 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | atomic+0x6C = [0xC978C4]; devuelve el atomic; el segundo argumento no se lee. |
| `0x007C89C0` | `_rpSkinD3D9AtomicLimitCheckOrOff` | 60 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Nombre provisional. Lee 0xC978E0/E4/E8 y skin+0x1C (comparacion sin signo). Sin comprobar geometria NULL. |
| `0x007C8A00` | `_rpSkinD3D9AtomicLimitCheck` | 60 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Complemento de 0x7C89C0 (FALSE cuando 0xC978E0 == 0). Nombre provisional. |
| `0x007C8A70` | `_rpSkinSplitDataCreate` | 158 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Sustituye la envoltura. Libera con RwFree el bloque anterior (+0x30), pone a 0 +0x24..+0x38, reserva numMatrices+2*(numMeshes+numRLE) (0x30116), fija +0x24 boneLimit, +0x28 numMeshes, +0x2C numRLE, +0x30 base, +0x34 base+numMatrices, +0x38 +0x34+2*numMeshes. Fallo: NULL con +0x30 = NULL. |
| `0x007C8B10` | `_rpSkinSplitDataDestroy` | 56 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Sustituye la envoltura. RwFree de +0x30 si existe, pone a 0 +0x24..+0x38, TRUE. |
| `0x007C8D40` | `_rpSkinSplitDataGetStreamSize` | 28 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Tamano de los datos divididos: 12, o 12+numBones+2*(numMeshes+numRLE) si numMeshes != 0. |
| `0x007C9680` | `_rpSkinD3D9CreateVertexBuffer` | 1810 | S | `source/game_sa/RenderWare/rw/rpskin.cpp` | Callback de instanciado del pipeline D3D9 de skin; la funcion MAS arriesgada del lote (~450 instrucciones transcritas a mano). Revisar por separado: (a) liberacion de los dos flujos (0x7F56A0 / 0x7F5AE0), (b) numero de flujos y 0x74E970, (c) cada D3DVERTEXELEMENT9 (flujo, offset, tipo, uso) segun usage/geometryFlags/DeclTypes/maxNumWeights, (d) strides y geometryFlags por flujo, (e) 0x7FAA30, (f) 0x7F5500 / 0x7F5A00 con size = totalNumVertex*stride, dynamicLock, useOffsets, (g) baseIndex por malla (divisor stride 0 = #DE como en el EXE). Probar con y sin skin+0x20, reinstance 0/1, usage con bits bajos, 1-2-3-4 pesos y DeclTypes con/sin DEC3N, UBYTE4, SHORT4N, USHORT4N. |
| `0x007CDF60` | `RtBMPImageRead` | 1633 | S | `source/game_sa/RenderWare/rw/rtbmp.cpp` | Sustituye la envoltura. Solapamiento del buffer local de cabeceras como en la pila (0x400 bytes; paleta de 256x4 cabe justo). Caminos: OS/2 (12), 40 y otros tamanos; RLE4 -> error 0x80000009; RLE8; 24/32 bits sin compresion; BMP de 1/4/8/15/16/24/32 bits. Errores: unos destruyen la imagen y otros no (paleta/cabecera solo cierran el flujo) -> comparar secuencia de RwStreamClose/RwImageDestroy/RwFree. Desbordamientos del EXE conservados. Colaboradores: RwStreamOpen/Read/Skip/Close, RwImageCreate/AllocatePixels/Destroy, RwMalloc/RwFree, _rtBMPImageSetRow, RwErrorSet. |
| `0x007CE5D0` | `_rtBMPImageSetRow` | 955 | S | `source/game_sa/RenderWare/rw/rtbmp.cpp` | Clave profundidad|bits<<8; nibbles/bits leen un byte antes de mirar el ancho y uno mas al terminar cada byte (lectura de 1 byte pasado el final); formatos 15/16 bits, 24 bits con flag 0/1 (orden de bytes), 32 bits. Clave desconocida: error 0x80000008 y FALSE. Sin x87. |

Las 28 son tipo S (`RH_ScopedGlobalInstall`); ninguna es V. Ninguna necesita los helpers x87 de 24 bits: no hay aritmetica de coma flotante (solo copia de dwords/floats y comparaciones como enteros).

## Saltadas

Ninguna. `0x7C9680` (1810 bytes) se reconstruyo completa; es la de mayor riesgo (ver su fila). Todas las direcciones del encargo son funciones completas con prologo propio.

## Notas de revision (sin banco)

- ABI: todas cdecl. `0x7C89B0` y `0x7C7520` devuelven el argumento 1 en EAX; `0x7C8830` devuelve 0. Ningun llamador conocido lee ECX/EDX tras ellas.
- `_rpSkinInitialize` (0x7C8740) y `_rpSkinD3D9CreateVertexBuffer` (0x7C9680) conservan comportamientos indefinidos del EXE (desreferencia de skin NULL, `div` por stride 0, do/while con numMeshes 0): no se protegen para no cambiar el comportamiento.
- `RtBMPImageRead` conserva los desbordamientos de pila del EXE por cabeceras BMP malformadas (`biSize-4` mayor que 0x400, memset/lectura RLE sin limite de fila).
- Llamadas por direccion fija que NO son de este lote: 0x7C67F0, 0x7C77A0, 0x7C7820, 0x7C7830, 0x7C7880, 0x7C8A40..0x7C8A60, 0x7CB2A0, 0x7CB2B0, 0x7CE990, 0x7F56A0, 0x7F5AE0, 0x7F5500, 0x7F5A00, 0x7FAA30, 0x8247E0 (qsort) y los callbacks listados en `RpSkinPluginAttach`.

## Bancos viejos que citan las direcciones

Busqueda con `git grep` de cada direccion sobre `reversing/` (sin `PendienteValidar` ni `runs/`). No se repiten. `BinaryCoverageAudit` es el inventario de cobertura (cita todas); los demas son bancos o estudios.

| Direccion | Carpetas de `reversing/` que la citan (ademas de BinaryCoverageAudit) |
| --- | --- |
| `0x007C6820` | AppBatch, OriginalDependencies499 |
| `0x007C6B70` | (solo BinaryCoverageAudit) |
| `0x007C70C0` | (solo BinaryCoverageAudit) |
| `0x007C7190` | (solo BinaryCoverageAudit) |
| `0x007C7520` | ClothesWave3, Clump24, OriginalDependencies499 |
| `0x007C7540` | ClothesWave3, GfxWave2, OriginalDependencies499, Pending355 |
| `0x007C7550` | AnimAudio6, AnimMgr31, Audio15, CEntA, CEntB, CPedA, CPedB, CTasks14b, ClothesGeometry355, ClothesSkin355, ClothesWave3, Clump24, CppRpAnimUpdate1, CppTrigJetPack, CppTrigJetPackCoverageProposal, Cutscene35, Helpers66C, OriginalDependencies499, Pending355, RpAnim43 |
| `0x007C7560` | ClothesWave3, OriginalDependencies499 |
| `0x007C75B0` | ClothesWave3, OriginalDependencies499 |
| `0x007C77E0` | ClothesWave3, Helpers66C, OriginalDependencies499 |
| `0x007C77F0` | ClothesGeometry355, ClothesSkin355, Clump24, OriginalDependencies499, Pending355 |
| `0x007C7800` | ClothesGeometry355, ClothesSkin355, OriginalDependencies499, Pending355 |
| `0x007C7810` | ClothesWave3, Helpers66C, OriginalDependencies499 |
| `0x007C86F0` | (solo BinaryCoverageAudit) |
| `0x007C8740` | Dependencies25C, OriginalDependencies2026-10-17 |
| `0x007C8830` | (solo BinaryCoverageAudit) |
| `0x007C8840` | (solo BinaryCoverageAudit) |
| `0x007C88B0` | (solo BinaryCoverageAudit) |
| `0x007C8980` | Dependencies25C |
| `0x007C89B0` | (solo BinaryCoverageAudit) |
| `0x007C89C0` | (solo BinaryCoverageAudit) |
| `0x007C8A00` | (solo BinaryCoverageAudit) |
| `0x007C8A70` | OriginalDependencies2026-10-17 |
| `0x007C8B10` | OriginalDependencies2026-10-17 |
| `0x007C8D40` | (solo BinaryCoverageAudit) |
| `0x007C9680` | (solo BinaryCoverageAudit) |
| `0x007CDF60` | CppTrigBrightPlane, OriginalDependencies499, Render23 |
| `0x007CE5D0` | (solo BinaryCoverageAudit) |
