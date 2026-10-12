<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# VisPluginsFinal4: banco por pares de CVisibilityPlugins

Banco de comparación par a par dentro del juego completo (flujo Small2/PadMouse1/VehModelInfoFinal4) para las **84 funciones** de `CVisibilityPlugins` (`docs/hooks.csv`, `validated=0`, EXE gta-sa.exe 1.0 US). Sin compilar ni ejecutar todavía. **18176 pares** (84/84 funciones cubiertas, ninguna excluida).

Uso: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh VisPluginsFinal4 3`. Filtro opcional: `vispluginsfinal4-only.txt` en la carpeta del juego con `CVisibilityPlugins::<función>`.

## Qué se compara (original = hooks de la clase desactivados, candidato = hooks activos)

Cada par construye desde cero (semilla por función y caso) un mundo falso **estático** (`World`, unos 50 KB): atomics, frames (modelling en 0x10, LTM en 0x50), clumps (tipo 2), geometrías con tablas de materiales (con materiales repetidos), materiales, texturas, model infos (vtable propia, tipo, alfa, banderas, distancia), CColModel, entidades (vtable propia, banderas), peds (arma activa, slot, clump de arma con frame, 13 armas), CWeaponInfo, jerarquía de animación y cámara RW con su frame. Los offsets de plugin se leen del EXE (0x8D608C/90/94) y se validan antes de empezar. La tabla `ms_modelInfoPtrs` del EXE se intercambia temporalmente (índices 450..465) y las listas de alfa del EXE (0xC88070..0xC88254) se reconstruyen sobre nodos propios (llenas, vacías, ordenadas, con duplicados). Cada ejecución guarda y restaura: variables y listas de la clase (0xC88000..0xC88260), offsets de plugin, multiplicadores de cámara (0xB6F118/1C), clip lejano (0xB76848), escala LOD (0x8CD804), `CGame::currArea`, `MI_GRASSHOUSE`, textura de piel del jugador 0, model infos 450..465 y el puntero de estados de render.

Comparación: retorno, traza completa de llamadas con argumentos (hash FNV de la traza y nº de eventos), hash del mundo falso, hash de la arena de `operator new/delete`, hash de todos los globales guardados y los campos de salida de cada caso (listas de alfa volcadas nodo a nodo, alfas de materiales tras la llamada, banderas, etc.). Los registros pasantes llevan `original` y `candidate` no nulos (`Summary()` nunca devuelve null, tampoco en funciones `void`). En fallos se guardan `diff_values`, `diff_blocks` (mundo) y `diff_arena_blocks`.

## Qué se simula (stubs trazados en la dirección del EXE y en el símbolo de la DLL)

- Primitivas RW: `RpClumpForAllAtomics` (recorre los atomics del clump falso y llama a los callbacks reales), `RpGeometryForAllMaterials` (recorre la tabla real de materiales), `RwFrameGetLTM` (devuelve frame+0x50), `RwFrameUpdateObjects`, `RwMatrixTranslate/Rotate`, `RpClumpRender`, `RwV3dTransformPoints`, `RwCameraFrustumTestSphere`, `_rpAtomicResyncInterpolatedSphere`, `RpMaterialSetTexture`, `RpHAnimIDGetIndex`, `RpHAnimHierarchyGetMatrixArray`, `GetAnimHierarchyFromSkinClump`.
- Render real: `AtomicDefaultRenderCallBack` se traza con el hash de las alfas de los materiales y de las banderas de la geometría vistas **en el momento del render** (es el observable de `RenderAtomicWithAlpha`).
- Estados de render: se sustituye el puntero `fpRenderStateSet` del dispositivo (`[RwEngineInstance+0x20]`, lo usan igual EXE y DLL) solo durante la llamada (guarda RAII y restauración).
- Luces, peds y armas: `SetAmbientColours`, `DeActivateDirectional`, `CRenderer::RenderOneNonRoad`, `CPed::SetGunFlashAlpha/ResetGunFlashAlpha/GetWeaponSkill()`, `CWeaponInfo::GetWeaponInfo(tipo, habilidad)`; ranuras virtuales 19/20 (SetupLighting/RemoveLighting) de las vtables falsas de entidad y ped, y ranura 4 (`GetModelType`) de los model infos falsos.
- `RpAtomicRegisterPlugin/RwFrameRegisterPlugin/RpClumpRegisterPlugin` (solo en `PluginAttach`): el stub devuelve los offsets del caso y **ejecuta realmente los tres callbacks** (constructor, copia y destructor) sobre objetos de prueba, de modo que `AtomicConstructor` (0x732150, sin hook ni entrada en `hooks.csv`) también queda comparado.
- `operator new/new[]/delete/delete[]` (0x82119A/0x821195/0x8214BD/0x8213AE) solo en `Initialise` y `Shutdown`, sobre una arena de 128 KB; el conjunto de parches se cierra antes de construir el JSON. El `delete[]` de nulo no se traza (el compilador puede omitir la llamada).
- Los stubs solo actúan en el hilo del banco (`owner`/`Mine()`).
- Los punteros a funciones que la DLL guarda (callbacks de listas, visibilidad del clump, `renderCallBack` del atomic, argumentos de `Rp*RegisterPlugin`) se normalizan con `Canon()` (pares EXE y destino de la DLL leídos de los hooks). El valor `0x7491C0` **no** se normaliza: se compara en bruto (comprueba la corrección de `SetAtomicRenderCallback`).

## Correcciones de la fuente (`source/game_sa/VisibilityPlugins.cpp`)

Resumen con evidencia (dirección: instrucción del EXE):

1. `IsClumpVisible` 0x732AE0 `jmp dword ptr [ecx+eax]`: **llama** a la callback del plugin con el clump; la DLL devolvía el puntero como bool.
2. `RenderAtomicWithAlphaCB` 0x73266B-0x732671 `mov ecx,[eax]`: `data` apunta al entero con la alfa; la DLL pasaba el puntero como alfa.
3. `IsAtomicVisible` 0x7329C4-0x7329D8: `RwV3dTransformPoints(out=in=&sphere.center)`; la DLL transformaba a un `point` descartado y probaba la esfera sin transformar.
4. `RenderHeliRotorAlphaCB` 0x7340E8-0x734137: ordenación `dist + 20*dot(pos-cámara, at del clump)`; la DLL usaba `GetVehicleDotProduct` (con ATOMIC_ID).
5. `RenderHeliTailRotorAlphaCB` 0x7341C2-0x734202: `((-dot_right) - dot_up) + dist` con suma y*y + x*x + z*z (`fsubp st(1),st` = DE E9); la DLL hacía `dist - d1 - d2`.
6. `GetDistanceSquaredFromCamera` (Frame y RwV3d) 0x732CA7-0x732CB5 y 0x732CDA-0x732CE8, y `SetupVehicleVariables` 0x733190-0x73319E: suma `(z²+y²)+x²` (no `x²+y²+z²`).
7. `GetDotProductWithCameraVector` 0x7326E0-0x732776 y `RenderObjNormalAtomic` 0x732420-0x73243A: productos escalares en orden `y*y + x*x + z*z` (`CVector::Dot` suma z, y, x).
8. `SetupVehicleVariables` 0x7331AC-0x7331D4: `atan2` por `fpatan` de x87 (`__asm`), `sqrt` en float de `cy²+cx²`, resta cámara menos posición de la matriz local del frame.
9. `gVehicleDistanceFromCamera` y `gAngleWithHorizontal` ahora son referencias a 0xC88024/0xC88020 (eran variables propias de la DLL que el EXE no veía).
10. `CalculateFadingAtomicAlpha` 0x732500-0x7325B8: selección `<` exacta (no `std::min`), `* 0x863E0C` (0.06666667f) en vez de `/ 15`, recorte solo si `> 1.0`, `_ftol2` de 64 bits.
11. `RenderAtomicWithAlpha` 0x7324B6-0x7324EF: comparación con signo contra el entero completo (`cmp ecx,edx / jle`; la DLL usaba `min(alpha,(uint8)alfa)`) y restauración de atrás hacia delante.
12. `SetAtomicRenderCallback` 0x7328A6-0x7328B8: guarda `0x7491C0` (la DLL guardaba su envoltorio).
13. `SetRenderWareCamera` 0x7328C0-0x732988: `v*v` en float (sin `powf`).
14. `VehicleVisibilityCB` 0x73372B-0x73373C y `RenderVehicleCB_ControlRenderMultiPassLOD` 0x732DE0: condiciones `!(a > b)` y `!(a < b)` (con NaN el EXE toma la otra rama).
15. `InsertEntityIntoSortedList` 0x734570-0x73458B y `RenderEntity` 0x732B50: `movsx` del índice de la entidad frente a `movzx` del global.

## Corpus por función

| Función | EXE | Pares |
|---|---|---|
| `AddWeaponPedForPC` | `0x5E46D0` | 192 |
| `AtomicCopyConstructor` | `0x732170` | 96 |
| `AtomicDestructor` | `0x7321A0` | 96 |
| `CalculateFadingAtomicAlpha` | `0x732500` | 512 |
| `ClearAtomicFlag` | `0x732310` | 96 |
| `ClearClumpForAllAtomicsFlag` | `0x732350` | 96 |
| `ClumpConstructor` | `0x732E10` | 96 |
| `ClumpCopyConstructor` | `0x732200` | 96 |
| `ClumpDestructor` | `0x732220` | 96 |
| `DefaultVisibilityCB` | `0x732A30` | 96 |
| `FrameConstructor` | `0x7321B0` | 96 |
| `FrameCopyConstructor` | `0x7321D0` | 96 |
| `FrameDestructor` | `0x7321F0` | 96 |
| `FrustumSphereCB` | `0x732A40` | 256 |
| `GetAtomicId` | `0x732370` | 96 |
| `GetClumpAlpha` | `0x732B20` | 96 |
| `GetClumpModelInfo` | `0x732AC0` | 96 |
| `GetDistanceSquaredFromCamera-Frame` | `0x732C80` | 256 |
| `GetDistanceSquaredFromCamera-RwV3d` | `0x732CC0` | 256 |
| `GetDotProductWithCameraVector` | `0x7326D0` | 512 |
| `GetFrameHierarchyId` | `0x732A20` | 96 |
| `GetModelInfo` | `0x732260` | 96 |
| `GetModelInfoIndex` | `0x732250` | 96 |
| `GetUserValue` | `0x7323A0` | 96 |
| `InitAlphaAtomicList` | `0x734530` | 96 |
| `InitAlphaEntityList` | `0x734540` | 96 |
| `Initialise` | `0x733A20` | 96 |
| `InsertAtomicIntoReallyDrawLastList` | `0x733E10` | 256 |
| `InsertEntityIntoReallyDrawLastList` | `0x733E50` | 256 |
| `InsertEntityIntoSortedList` | `0x734570` | 384 |
| `InsertEntityIntoUnderwaterList` | `0x733D90` | 256 |
| `InsertObjectIntoSortedList` | `0x733DD0` | 256 |
| `IsAtomicVisible` | `0x732990` | 256 |
| `IsClumpVisible` | `0x732AE0` | 96 |
| `PluginAttach` | `0x732E30` | 192 |
| `RenderAlphaAtomics` | `0x733E90` | 256 |
| `RenderAtomicWithAlpha` | `0x732480` | 384 |
| `RenderAtomicWithAlphaCB` | `0x732660` | 256 |
| `RenderBoatAlphaAtomics` | `0x733EC0` | 256 |
| `RenderEntity` | `0x732B40` | 512 |
| `RenderFadingAtomic` | `0x732610` | 256 |
| `RenderFadingClump` | `0x732680` | 256 |
| `RenderFadingClumpCB` | `0x733630` | 256 |
| `RenderFadingEntities` | `0x733F10` | 256 |
| `RenderFadingUnderwaterEntities` | `0x7337D0` | 256 |
| `RenderHeliRotorAlphaCB` | `0x7340B0` | 256 |
| `RenderHeliTailRotorAlphaCB` | `0x734170` | 256 |
| `RenderObjNormalAtomic` | `0x7323E0` | 256 |
| `RenderOrderedList` | `0x7337A0` | 192 |
| `RenderPedCB` | `0x7335B0` | 256 |
| `RenderPlayerCB` | `0x732870` | 256 |
| `RenderReallyDrawLastObjects` | `0x733800` | 256 |
| `RenderTrainHiDetailAlphaCB` | `0x734240` | 384 |
| `RenderTrainHiDetailCB` | `0x733330` | 384 |
| `RenderVehicleHiDetailAlphaCB` | `0x733F80` | 512 |
| `RenderVehicleHiDetailAlphaCB_BigVehicle` | `0x734370` | 512 |
| `RenderVehicleHiDetailAlphaCB_Boat` | `0x7344A0` | 256 |
| `RenderVehicleHiDetailCB` | `0x733240` | 512 |
| `RenderVehicleHiDetailCB_BigVehicle` | `0x733420` | 384 |
| `RenderVehicleHiDetailCB_Boat` | `0x733550` | 256 |
| `RenderVehicleLoDetailCB_Boat` | `0x7334F0` | 256 |
| `RenderVehicleReallyLowDetailCB` | `0x7331E0` | 256 |
| `RenderVehicleReallyLowDetailCB_BigVehicle` | `0x732820` | 256 |
| `RenderWeaponCB` | `0x733670` | 256 |
| `RenderWeaponPedsForPC` | `0x732F30` | 512 |
| `RenderWheelAtomicCB` | `0x7323C0` | 256 |
| `SetAlphaCB` | `0x7323B0` | 128 |
| `SetAtomicFlag` | `0x7322B0` | 96 |
| `SetAtomicId` | `0x732290` | 96 |
| `SetAtomicRenderCallback` | `0x7328A0` | 128 |
| `SetClumpAlpha` | `0x732B00` | 96 |
| `SetClumpForAllAtomicsFlag` | `0x7322F0` | 96 |
| `SetClumpModelInfo` | `0x733750` | 192 |
| `SetFrameHierarchyId` | `0x732A00` | 96 |
| `SetModelInfoIndex` | `0x732230` | 96 |
| `SetRenderWareCamera` | `0x7328C0` | 256 |
| `SetTextureCB` | `0x732850` | 96 |
| `SetUserValue` | `0x732380` | 96 |
| `SetupVehicleVariables` | `0x733160` | 256 |
| `Shutdown` | `0x732EB0` | 192 |
| `VehicleVisibilityCB` | `0x7336F0` | 256 |
| `VehicleVisibilityCB_BigVehicle` | `0x732AB0` | 192 |
| `clearClumpForAllAtomicsFlagCB` | `0x732330` | 96 |
| `setClumpForAllAtomicsFlagCB` | `0x7322D0` | 96 |

Total: **18176** pares.

## Notas de dominio y supuestos

- Fixtures sin sNaN. Se incluyen NaN silenciosos solo donde la comparación forma parte de lo probado (distancias y umbrales de los callbacks, distancia de `CalculateFadingAtomicAlpha`, `Insert*`).
- `CalculateFadingAtomicAlpha`: se prueban productos fuera de int32 pero dentro de int64 (el EXE toma los 32 bits bajos de `_ftol2`).
- Los callbacks de visibilidad de prueba devuelven 0 o 1 (la DLL declara `bool`; los reales devuelven RwBool 0/1). En las funciones `bool` (`IsClumpVisible`, `IsAtomicVisible`, `Insert*`, `PluginAttach`) solo se compara AL, porque la parte alta de EAX es basura.
- `GetUserValue` devuelve AX (la parte alta de EAX es basura): se enmascara a 16 bits.
- No se compara el rastreo de Tracy (`ZoneScoped`) de `RenderReallyDrawLastObjects` y `RenderWeaponPedsForPC`.
- `RenderAtomic` (0x732B30) y `Insert*IntoSortedList` inline no tienen hook; se ejercitan a través de los callbacks y de `Insert*` con la normalización de punteros.

## Aprobación (10/10/2026)

Corrida `20261010-130234-135622`: passed=true, 18176 pares, 84 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_vispluginsfinal4.py` (evidencia en `runs/`, `validation-summary.json`). Con este banco CVisibilityPlugins queda sin pendientes. Límites declarados en el alcance de cada función y en las secciones anteriores de este README.
