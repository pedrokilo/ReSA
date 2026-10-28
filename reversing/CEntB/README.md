<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco CEntB: CEntity, grupo B (ciclo de vida, referencias, estado y colision; 28 funciones)

Fecha: 07/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `python reversing/CEntB/build_probe.py` y `python reversing/CEntB/run_local.py`).
Prefijo de caso `centb`, espacio de nombres `CEntBLive`, ficheros `centb-runtime.start`, `centb-runtime.jsonl` y `centb-only.txt` (filtro `CEntity::Funcion`).

## Funciones y casos (13.824 pares)

| Funcion | Hook / vtable | Casos |
|---|---|---|
| `Constructor` / `Destructor` | 0x532A90 / 0x535E90 | 512 / 512 |
| `CleanUpOldReference-` / `RegisterReference-` | 0x571A00 / 0x571B70 | 512 / 1024 |
| `ResolveReferences` / `PruneReferences` | 0x571A40 / 0x571A90 | 512 / 512 |
| `IsEntityPointerValid` | 0x533310 | 512 |
| `SetIsStatic`, `SetModelIndex`, `SetModelIndexNoCreate`, `CreateRwObject`, `DeleteRwObject` | vtable 0x863928, huecos 4, 5, 6, 7, 8 | 128, 256, 1024, 2048, 1024 |
| `ProcessControl`, `ProcessCollision`, `ProcessShift`, `TestCollision`, `Teleport` | huecos 10, 11, 12, 13, 14 | 64, 64, 64, 64, 128 |
| `SpecialEntityPreCollisionStuff`, `SpecialEntityCalcCollisionSteps`, `FlagToDestroyWhenNextProcessed` | huecos 15, 16, 21 | 128, 64, 64 |
| `GetIsStatic`, `GetIsTypePhysical`, `TreatAsPlayerForCollisions` | 0x4633E0, 0x4DA030, 0x541F70 | 256 c/u |
| `GetIsTouching-ent` / `GetIsTouching-vec` | 0x5343F0 / 0x5344B0 | 1024 / 1024 |
| `DoesNotCollideWithFlyers` | 0x532D40 | 512 |
| `FindTriggerPointCoors` | 0x533380 | 1024 |
| `SetupBigBuilding` | 0x533150 | 256 |

## Diferencias reales corregidas (fuente: `source/game_sa/Entity/Entity.cpp`)

| Funcion | Correccion (evidencia en el EXE 1.0 US) |
|---|---|
| `SetupBigBuilding` | 0x533150: `and eax,0FFFFFFFEh / or eax,10100h`: pone los bits **8** (`m_bIsBIGBuilding`) y **16** (`m_bDontCastShadowsOn`) de +0x1C. La DLL ponia el bit 10 (`m_bStreamingDontDelete`). El EXE comprueba el bit 10 con `test ah,24h` en 0x409965, pero esta funcion nunca lo activa. |
| `RegisterReference` | 0x571BC3..0x571C90: cuando no quedan referencias libres se recorren los pools de peatones (0xB74490, paso 0x7C4), vehiculos (0xB74494, 0xA18) y objetos (0xB7449C, 0x19C) **de la ultima ranura a la primera**, parando en cuanto `pEmptyList` deja de ser nulo; la DLL (`GetAllValid`) los recorria de la primera a la ultima, con lo que podia podar otra entidad antes. |
| `ResolveReferences` | 0x571A50: `mov edx,[eax+4] / cmp [edx],ecx`: el EXE no comprueba que `m_ppEntity` sea nulo (la DLL si). |
| `DeleteRwObject` | (1) 0x534046..0x534055: para un atomic el EXE lee el frame, llama a `RpAtomicDestroy` y a `RwFrameDestroy(frame)` **sin comprobar que el frame sea nulo** (la DLL se saltaba ambos). (2) 0x5340B1..0x5340C4: el enlace de streaming se pasa a `RemoveEntity` y solo despues se pone a nulo (la DLL lo anulaba antes con `std::exchange`). |
| `GetIsTouching` (entidad) | 0x53445B..0x534487: suma en x87 en orden **z, y, x**: `(dz*dz + dy*dy) + dx*dx < (r1+r2)^2`; la DLL usaba `SquaredMagnitude` (x, y, z). |
| `GetIsTouching` (esfera) | 0x5344EE..0x534516: orden **z, x, y**: `(dz*dz + dx*dx) + dy*dy`. |
| `FindTriggerPointCoors` | 0x5333FC: `MultiplyMatrixWithVector` (0x59C890) con su orden de sumas (x: z, y, x; y: z, x, y; z: z, x, y); la DLL usaba `CMatrix::TransformPoint` (x, y, z). Helper local `EntityGroupB_MultiplyPoint59C890` en `Entity.cpp`. |

## Sin diferencias (MATCH, con la evidencia del EXE)

* `Constructor` (0x532A90): `CPlaceable::CPlaceable` (0x54F1E0), vptr, +0x36 = 0x20 (tipo 0, estado ABANDONED), +0x1C = 0x08000080, +0x2C/+0x22/+0x18/+0x2F/+0x2E/+0x20 (rand)/+0x24/+0x28/+0x34/+0x35/+0x30; mismo conjunto de escrituras que la DLL y sin valor-inicializacion de mas.
* `Destructor` (0x535E90): `dec byte [lod+34h]` si hay LOD, `DeleteRwObject` (0x534030), `ResolveReferences` (0x571A40) y `~CPlaceable` (0x54F490: `RemoveMatrix` + `--numMatrices` si hay matriz, `m_matrix = 0xB74240`).
* `CleanUpOldReference` (0x571A00), `PruneReferences` (0x571A90): mismas listas (siguiente en +0, `m_ppEntity` en +4) y mismo orden de enlace a `pEmptyList` (0xBA1768).
* `IsEntityPointerValid` (0x533310): tabla de saltos 0x533360 (tipos 1 a 5 hacia 0x4040E0, 0x6E38F0, 0x5E8A30, 0x5A2C20, 0x532730; tipo 6 verdadero; 0, 7 y nulo falsos).
* `SetModelIndex` (0x532AE0): hueco 6 (`SetModelIndexNoCreate`) y hueco 7 (`CreateRwObject`). `SetModelIndexNoCreate` (0x533700): modelo sin signo, `HasPreRenderEffects` al bit 21, `bDrawLast` (bit 14) si el bit 1 del modelo, bit 27 a cero si no tiene el bit 6, y `CTagManager::AddTag` si es atomic con `bTagDisabled` a cero y tipo especial 6.
* `CreateRwObject` (0x533D30): visible (bit 7), danado (bit 9) con `ms_bCreateDamagedVersion` (0xA9B0B0), `CreateInstance` por el hueco 11 (0x2C) del model info, `gBuildings`, `UpdateRwMatrix` en linea (0x59AD70 / 0x54EF40), `IsTag`/`ResetAlpha`/`PreRenderUpdate` para atomics, ruta de carretera para clumps (objeto: `AddToMovingList` + `SetIsStatic(false)` por el hueco 4; resto: lista de cuerpos en movimiento), sincronizacion de la animacion del LOD, `AddRef`, `AddEntity`, `CreateEffects` e `IsCBPCPipelineAttached` (bit 28 si falla).
* `SetIsStatic` (0x403E20: bit 2 con el bit 0 del byte), `GetIsStatic` (0x4633E0: bits 2 y 18), `GetIsTypePhysical` (0x4DA030: tipo 2..4), `TreatAsPlayerForCollisions` (0x541F70: estado 0), `DoesNotCollideWithFlyers` (0x532D40: tipo especial 1 o 2, o bit 10 del model info).
* `ProcessControl`, `ProcessCollision`, `ProcessShift`, `FlagToDestroyWhenNextProcessed` (`ret`), `TestCollision` (devuelve 0, `ret 4`), `Teleport` (`ret 10h`), `SpecialEntityPreCollisionStuff` (`ret 18h`), `SpecialEntityCalcCollisionSteps` (devuelve 1, `ret 8`): vacias; las DLL tambien (con `/RTC` el banco detecta un `ret N` distinto).

## Metodo

* Entidades falsas: tres buferes de 0x1C0 bytes (probada, otra y LOD) con la vtable falsa de la entidad (huecos 4, 6 y 7 con traza; el resto, trampas `ent_virtual_inesperada`). El constructor se llama sobre 0xCD y se compara el bufer completo (salvo vptr, que se compara por tipo: EXE 0x863928/0x863C40 frente a una vtable dentro de la DLL).
* Model infos falsos en `ms_modelInfoPtrs[19990..19992]` (vtable falsa: hueco 1 `AsAtomicModelInfoPtr` sin traza, 4 `GetModelType`, 11 `CreateInstance`; el hueco 10 es trampa para detectar que la DLL use la sobrecarga con matriz), colmodels, efectos 2d, objetos RenderWare falsos (atomic tipo 1, clump tipo 2, frames con su matriz en +0x10), asociaciones de animacion, enlaces de streaming.
* Referencias: nodos `CReference` propios (lista de libres falsa en `pEmptyList`), celdas de puntero con la entidad probada, otra, entidades de pool, nulo o basura, y tres pools falsos (4 peatones, 3 vehiculos y 3 objetos, estados de hueco aleatorios) cuyos punteros reales solo se cambian durante la llamada. Todo el codigo de referencias se ejecuta de verdad en ambos lados.
* Stubs trazados en la direccion del EXE **y** en el simbolo/cuerpo de la DLL, instalados por funcion (nunca se sustituye la funcion probada ni sus cuerpos): ver la cabecera de `LiveComparison.hpp`. La lista de cuerpos en movimiento se simula en `AddItem`/`DeleteItem` (el EXE reserva el nodo con 0x5523C0 y lo enlaza con 0x5329A0; ambos se unifican en una sola traza `MovingList.AddItem`).
* Se comparan retorno, traza completa, la entidad probada palabra a palabra, el hash de todos los buferes auxiliares y los globales (gBuildings, `ms_bCreateDamagedVersion`, numMatrices, `pEmptyList`, punteros de pools, cabeza de la lista de movimiento); evento `layout` con los desplazamientos de la DLL frente a los del EXE; `slot_is_exe_function` comprueba que cada hueco de la vtable volcada apunta a la funcion esperada del EXE.

## No cubierto y por que

* `GetModelInfo()` de la DLL usa el indice sin signo (`movzx`) y el EXE `movsx` (`word [+0x22]`): solo difieren con indices >= 0x8000, que no existen en el juego (no se prueban; leerian `ms_modelInfoPtrs[-n]`).
* `CreateRwObject` / `DeleteRwObject` ejecutan con RenderWare simulado: la creacion e destruccion reales de atomics, clumps y frames, `AddToMovingList`, la lista de cuerpos en movimiento real y el `CStreaming` real no se tocan (se trazan las llamadas).
* `ResolveReferences` con `m_ppEntity` nulo no se prueba (el EXE se cae; la DLL ahora tambien).
* `SetIsStatic` con un argumento distinto de 0/1 (el EXE solo mira el bit 0 del byte, la DLL trata `bool`).
* `CPlaceable::CPlaceable/~CPlaceable` no tienen hook ni se simulan: se ejecutan de verdad en ambos lados (constructor/destructor) y cualquier diferencia aparece como fallo de `Constructor`/`Destructor`.
* Tipos de entidad 6 y 7 en `RegisterReference` se prueban (no son edificio); `AddItem` de la lista de movimiento no se prueba con la lista real.

## Supuestos a comprobar al compilar

1. Compilacion en Debug sin inlinado (`/Ob0`): `CPtrListDoubleLink<CPhysical*>::AddItem/DeleteItem` (miembros de `CPtrList<...>`), `CTagManager::*`, `CBaseModelInfo::AddRef/RemoveRef/Get2dEffect` y los helpers de Entity.h deben ser funciones reales para poder parchearlas.
2. `DllBody("CEntity","DeleteRwObject")` devuelve el cuerpo real de la virtual (no un thunk de vtable); si no, el destructor no queda simulado y el evento `warning` lo anunciara.
3. `sizeof(CCopPed) = 0x7C4`, `sizeof(CHeli) = 0xA18` y `sizeof(CCutsceneObject) = 0x19C` (paso de los tres pools); si no, `RegisterReference` mostrara diferencias que no son de la funcion.
4. El evento `layout` (`layout_ok`) debe ser verdadero: `m_pMovingList` en 0xB4, `m_boundSphere` en 0x18/0x24, `C2dEffect` (tipo en 0xC, id en 0x10), `CBaseModelInfo` (0xD, 0x12, 0x14), `CAnimBlendAssociation::m_CurrentTime` en 0x20.
5. Los indices de modelo 19990..19992 de `ms_modelInfoPtrs` estan libres (se restauran de todas formas).
6. `CAnimBlendAssociation::SetCurrentTime`, `CCustomBuildingRenderer::IsCBPCPipelineAttached` y `CStreaming::RemoveEntity` son publicos; si alguno es privado hay que usar un desplazamiento crudo.
7. El hueco 11 de la vtable de `CBaseModelInfo` es `CreateInstance()` (0x533D3F: `call [edx+2Ch]`); si la DLL llama al hueco 10 el banco lo marca (`mi_virtual_inesperada`).
