<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CEntity, grupo C: efectos 2D, dibujo e iluminacion (16 funciones)

Banco de validacion por pares (EXE gta_sa 1.0 US frente a DLL) de: `CreateEffects`, `DestroyEffects`, `GetRandom2dEffect`, `HasPreRenderEffects`, `ProcessLightsForEntity`, `PreRender`, `PreRenderForGlassWindow`, `Render`, `RenderEffects`, `RemoveEscalatorsForEntity`, `RemoveLighting`, `SetupLighting`, `SetAtomicAlpha`, `SetCompAlphaCB`, `SetRwObjectAlpha` y `MaterialUpdateUVAnimCB` (las virtuales por la vtable del EXE 0x863928: `PreRender` 17, `Render` 18, `SetupLighting` 19, `RemoveLighting` 20; verificado volcando la vtable).

Estado: **escrito y revisado contra el desensamblado; pendiente de compilar y ejecutar** (lo hace el responsable de la integracion). Prefijo de caso `centc`, espacio de nombres `CEntCLive`. Repetir: `python reversing/CEntC/build_probe.py` y `python reversing/run.py --case centc`. Filtro opcional: `centc-only.txt` con `CEntity::<Funcion>`.

## Diferencias reales corregidas en `Entity.cpp`

- `ProcessLightsForEntity` (reescrita siguiendo el EXE 0x6FC7A0; la DLL era una aproximacion con muchas diferencias):
  - Sol: el id de la corona es `this + C + 0x21` (no la semilla); la distancia a la camara se suma `(y*y + z*z) + x*x`; el producto escalar con el sol es `(y + z) + x`; el color usa `* 0,33333334f` (0x859040) en vez de `/ 3`.
  - Luces: el desplazamiento temporal de los parpadeos aleatorios es `>> 11` (/2048), no `/ 4096`; los modos 5/6/4 ON-OFF usan `(resto * 9) / 10000` con signo (la DLL calculaba `9 * (resto / 10000)` = 0 siempre), la constante 0x872538 es 1/1111 (0x3A6BF405, no 0,0009f), y las posiciones entran por `fmul` + `fiadd` del x87 antes del `_ftol2`; el brillo es `sprite * brillo * 0,1f` (0x858B1C), no `/ 10`; la comprobacion de direccion usa la matriz con `Multiply3x3` (0x59C790) y el producto `(y + z) + x`, y solo falla si el resultado es < 0; el color de la sombra es `((canal * multiplicador) * TimeFade) * (1/256)` (la DLL multiplicaba primero `multiplicador * TimeFade / 256`); la distancia Z de la sombra se lee sin signo (movzx); la luz con color usa `LightFade * TimeFade * (1/256)`; las luces de niebla y la antiluz salen del mismo orden de saltos que el EXE (LaRiotsActiveHere se consulta siempre).
- `PreRender`: el limite de distancia de las sombras de grua/imanes es 110,0 (0x858BA8), no 100; la intensidad de la sombra de la pelota de playa es la palabra de 16 bits completa de `m_nShadowStrength` (solo r/g/b usan el byte bajo); `fDistToNearestTree` se actualiza con `!(arbol < dist)` (como `fcomp` + `test ah,5`, con NaN guarda); verificado el resto de ramas (coleccionable, dinero, minas, misil, bengala, cristal, pickup, granada, molotov, pelota, gruas, manga de viento, semaforos y farolas).
- `CreateEffects` (ENEX): los dias se decrementan como entero de 32 bits (`dec eax`: 0 pasa a 0xFFFFFFFF y no a 255) antes de `srand(this + dias)`; el rumbo para el angulo de entrada/salida se calcula con `fpatan` a precision completa y se convierte a grados con `fmul` 57,2957763671875f + `fadd` del x87 sin redondearlo antes a float (la DLL pasaba por `GetHeading()` en float).
- `GetRandom2dEffect`: el indice sale de `(rand() & 0xFFFF) * (1/32768) * n` truncado (`GetRandomIntInRange(0, n)`, 0x858B14), no de la interpolacion 1/32767 de `GetRandomNumberInRange`; sin `notsa::cast` (el EXE no comprueba el tipo antes de `HasEmptySlot`).
- `RemoveEscalatorsForEntity`: el EXE no sale tras la primera coincidencia (recorre las 32 escaleras y elimina todas las de la entidad).
- `SetRwObjectAlpha`: un tipo de objeto distinto de atomic/clump no hace nada (la DLL abortaba con `NOTSA_UNREACHABLE_CASE`).
- `MaterialUpdateUVAnimCB`: el paso de tiempo es `ms_fTimeStep * 0,02f` (0x858B38), no `/ 50`.
- `Render`: la bandera `m_bImBeingRendered` se baja despues de restaurar el estado de render de la medusa (orden del EXE; sin efecto observable).

Sin cambios necesarios (MATCH): `HasPreRenderEffects` (mismo orden de comprobaciones y bits 0x7800 del `m_nFlags` del model info), `DestroyEffects` (mismo orden: atractor, Fx, cartel con `SetFrame`/`FrameDestroy`/`AtomicDestroy`, ENEX con `FindNearestEntryExit(.., 1.5, -1)` y `bEnteredWithoutExit`/`DeleteOne`), `PreRenderForGlassWindow`, `RenderEffects`, `RemoveLighting`, `SetupLighting`, `SetAtomicAlpha`, `SetCompAlphaCB`.

Desviaciones que se dejan: `CreateEffects` comprueba `GetInSlot(...) != nullptr` (el EXE desreferencia sin comprobar; el banco nunca da un slot libre).

## Metodo

- La entidad es un bufer FALSO de 0x300 bytes con patron 0xCD y vptr a una vtable falsa de 64 thunks con traza (las virtuales de la entidad se llaman por la vtable del EXE 0x863928, comprobando el slot). Matriz, clumps (byte de tipo 2), atomics (tipo 1, frame, geometria y callback de dibujo), materiales, frames, model infos (vtable falsa: ranura 1 `AsAtomicModelInfoPtr`, ranura 4 `GetModelType`, flags de tipo especial 0x7800 en +0x12, conteo de efectos en +0x0D), listas de efectos 2D de todos los tipos (luz, particula, atractor, sol, interior, ENEX, cartel, trigger point, cover point, escalera y tipos fuera de rango), textos de los carteles, pool de entradas/salidas (cabecera de CPool falsa) y atomics de carteles son bufers propios; sin punteros a entidad en 0xCD ni entidades/pools reales. Los model infos falsos se colocan temporalmente en `ms_modelInfoPtrs` (indices de `ModelIndices` leidos del EXE, 342/344/345 y 777/1234).
- `CBaseModelInfo::Get2dEffect` (0x4C4C70) se simula (devuelve el efecto i del bufer) para no depender del plugin de geometria.
- Se simulan y trazan (direccion del EXE + simbolo de la DLL): rand/srand/`CGeneral::GetRandomNumber`, `GetDayNightBalance`, `CEntity::TransformFromObjectSpace` (las dos sobrecargas), `RegisterCorona`/`UpdateCoronaCoors`, `AddLight`, `StoreStaticShadow`, `StoreShadowToBeRendered` (dos sobrecargas)/`StoreShadowForPole`, `RegisterStreak`, `GetWaterLevelNoWaves`, `LaRiotsActiveHere`, `ShouldLightsBeFlashing`, semaforos, pickups, cristales, `ModifyMatrixForTreeInWind/Crane`, `BuildWindSockMatrix`, `UpdateRwMatrix`, RenderWare (`RwFrame*`, `RpAtomicSetFrame/Destroy`, `RpGeometryForAllMaterials`, `RpClumpForAllAtomics`, `RpClumpRender`, UV anim, MatFX; los `ForAll*` llaman de verdad a los callbacks sobre atomics y materiales falsos, de modo que `SetAtomicAlpha`/`SetCompAlphaCB`/`MaterialUpdateUVAnimCB` se ejecutan en EXE y DLL), `CCustomBuildingDNPipeline::PreRenderUpdate`, `CTagManager`, `CStreaming::RenderEntity`, `CCustomRoadsignMgr` y las funciones de banderas de carteles, Fx (`CreateEntityFx`/`DestroyEntityFx`), scripts de cerebros, gestor de entradas/salidas (`AddOne`, `FindNearestEntryExit`, `DeleteOne`), escaleras (`AddOne`, `SwitchOff`), gestor de atractores, `CPlaceable::AllocateMatrix`/`CSimpleTransform::UpdateMatrix` y las luces del renderer (`ActivateDirectional`, `SetAmbientColours`, `GenerateLightsAffectingObject`, ...). `RwRenderStateSet/Get` son macros del motor: se cambian los dos punteros de `RwEngineInstance` (+0x20/+0x24) y se restauran.
- No se simulan `CVector::Normalise` ni `Multiply3x3`: se ejecutan de verdad (el EXE llama a 0x59C910/0x59C790; la DLL usa su propia version), de modo que tambien se validan por este camino. `UpdateRwFrame` tampoco se simula porque el EXE lo expande en linea en la rama del coleccionable.
- Globales instantaneadas y restauradas en cada ejecucion: paso de tiempo, contador de fotogramas, reloj en ms, reloj del juego (hora/dia), camara (posicion, matriz, eje derecho), clima (SunGlare, WetRoads, Rain), ciclo horario (indice almacenado, vectores al sol, colores actuales), modo de Replay, distancia al arbol mas cercano, array de escaleras (0xC6E9A8), puntero del pool de entradas/salidas, estados del motor y `ms_modelInfoPtrs`.
- Comparacion: instantanea (hash con nombres estables de punteros) de la entidad, geometrias, materiales y objetos tocados + traza ordenada + retorno; ante una diferencia se repite con detalle completo.

## Tabla

| Funcion | Direccion / slot | Pares |
|---|---|---:|
| CreateEffects | 0x533790 | 512 |
| DestroyEffects | 0x533BF0 | 384 |
| GetRandom2dEffect | 0x533410 | 256 |
| HasPreRenderEffects | 0x532B70 | 512 |
| ProcessLightsForEntity | 0x6FC7A0 | 1024 |
| PreRender | vtable 0x863928 slot 17 | 768 |
| PreRenderForGlassWindow | 0x533240 | 64 |
| Render | slot 18 | 384 |
| RenderEffects | 0x5342B0 | 128 |
| RemoveEscalatorsForEntity | 0x717900 | 128 |
| RemoveLighting | slot 20 | 64 |
| SetupLighting | slot 19 | 64 |
| SetAtomicAlpha | 0x533290 | 64 |
| SetCompAlphaCB | 0x533280 | 64 |
| SetRwObjectAlpha | 0x5332C0 | 128 |
| MaterialUpdateUVAnimCB | 0x532D70 | 64 |

Total 4.608 pares. Evidencia: `validation-summary.json` (se genera al aprobar).

## No cubierto y supuestos

- Los dibujos reales (`RpAtomicRender`, `RpClumpRender`, coronas, sombras, luces) y la creacion de sistemas Fx no se ejecutan: se simulan con trazas.
- `_ftol2` del EXE devuelve 64 bits; el banco mantiene los argumentos dentro del rango de int32 (tiempo < 2^30, posiciones < 60).
- Los valores NaN no se generan (el x87 y SSE difieren en `fcomp` + `test ah,5` frente a `<`/`>`; se ha reproducido el comportamiento del EXE en las comparaciones que pasaban por alto el NaN, pero no se prueban).
- Los indices de modelo de las entidades son positivos (< 0x8000): el EXE lee el indice con `movsx` y la DLL con `uint16`.
- `Normalise` de `ProcessLightsForEntity` se apoya en que `CVector::Normalise` (0x59C910) esta validado.
