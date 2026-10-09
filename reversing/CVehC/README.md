<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco CVehC: CVehicle grupo C (mejoras, atomics, remaps y componentes)

Fecha: 06/10/2026. Estado: escrito, **sin compilar ni ejecutar** (pendiente de `build_probe.py` y `run_local.py`).

26 funciones de `CVehicle` (EXE gta_sa 1.0 US contra la DLL). Prefijo de caso `cvehc`, namespace `CVehCLive`, 8640 pares.

## Método

* Cada función se llama **en la dirección del EXE** sobre un `CVehicle` falso (búfer de 0x5A0 bytes con patrón aleatorio por caso + 32 bytes de
  guarda 0xCD, vptr a una vtable falsa con trampas numeradas y el hueco 48 = `SetupSuspensionLines` trazado):
  * original: los 26 hooks del grupo desactivados (el EXE ejecuta su código, incluidos los callbacks que se pasan por dirección);
  * candidato: solo el hook de la función probada activo (la DLL pasa sus propios callbacks).
* Se compara: retorno, argumentos de salida (`int&`, `data`), búfer del vehículo (hash + diferencias byte a byte si falla), guarda, clumps,
  marcos, atomics, geometrías, materiales, texturas, model infos, estado lateral de los dobles (identificador de atomic, jerarquía de marco,
  alfa de clump, callback de render), globales tocadas y la **traza ordenada** de llamadas salientes.
* Escena falsa (`BuildScene`): marcos con listas de objetos e hijos (acíclicas), atomics 0..5 repartidos en marcos/clump, 6/7 devueltos por
  `CreateInstance` (hueco 11 de una vtable falsa), 8 = clon; geometrías con listas de materiales; materiales con alfa 255/otro; texturas con
  `refCount` en +0x54; model infos de atomics con la palabra de +0x12 (CarMod en bits 10..14, bit 8); `CVehicleModelInfo` falso en
  `ms_modelInfoPtrs[15..18]` (remaps en +0x2FA, material de matrícula en +0x24, tipo en +0x31, `m_pVehicleStruct` en +0x5C con
  `m_aUpgrades[15].m_nParentComponentId` en +0x2B0, `m_pColModel` en +0x14, clump en +0x1C).
* **Dobles trazados** (parche de entrada en la dirección del EXE y, si existe, en el cuerpo de la DLL; instalados durante todo el banco):
  `CVisibilityPlugins` (GetAtomicId 0x732370, GetModelInfo, GetModelInfoIndex, Get/SetFrameHierarchyId, SetAtomicId/Flag/RenderCallback,
  Get/SetClumpAlpha), RenderWare (RpClumpForAllAtomics, RwFrameForAllObjects/Children, RpGeometryForAllMaterials: **ejecutan los callbacks**
  sobre las listas falsas y paran si devuelven NULL como RW; AtomicClone/Destroy/SetFrame, ClumpAdd/RemoveAtomic, FrameAddChild/Destroy,
  RwMatrixUpdate, RwTextureDestroy), `CBaseModelInfo::AddRef/RemoveRef`, `CClumpModelInfo::GetFrameFromId`, `CVehicleModelInfo`
  (GetNumRemaps, Get/SetCustomCarPlateText, SetRenderPipelinesCB), `CCustomCarPlateMgr::CreatePlateTexture`, `CTxdStore::RemoveRef`,
  `CAutomobile::NitrousControl`, `CAudioEngine::SetRadioBassSetting`, `CEntity::RegisterReference`, `CCollision::RemoveTrianglePlanes`
  (0x4185A0), `CColModel::AllocateData()` y `operator=`, `CQuaternion::Get(RwMatrix*)`, `AddVehicleUpgrade` (0x6E3290).
* **Se ejecutan de verdad** en ambos lados: `HasAlphaMaterialCB`, los callbacks del EXE `RemoveUpgradeCB`/`FindUpgradeCB`/
  `FindReplacementUpgradeCB`/`SetCompAlphaCB` (la DLL los llama por su dirección), `CMatrix` (ctor, SetRotateX, RotateY, UpdateRW, dtor: el
  EXE usa el suyo y la DLL el de la DLL) y las matrices de los marcos falsos.
* Los punteros a callbacks se normalizan por nombre (`CbName`): la función del EXE y la de la DLL dan el mismo nombre en la traza.
* Globales guardadas y restauradas por caso y al final (`state_restored`): model infos 15..18, índices de modelo de mejoras (0x8CD760..0x8CD773),
  `ms_bCreateDamagedVersion` (0xA9B0B0), `CopyObjectsCB_TargetClump` y vehículos/colmodels/hidráulicos especiales (0xC1CB58..0xC1CD37).
* Evento `layout`: comprueba desplazamientos de RwFrame/RpAtomic/RpGeometry/RpMaterial/RwTexture/RwMatrix, `CVehicleModelInfo`,
  `UpgradePosnDesc`, `m_aUpgrades[15].m_nParentComponentId` (0x2B0) y tamaños de `CVehicle`, `CColModel`, `tHydraulicData`.
  Los campos de `CVehicle` (flags, audio, mejoras, remaps...) se verifican por comportamiento: cualquier desplazamiento distinto aparece como
  diferencia en el búfer o el retorno.
* Antes de cada caso se escribe y vacía `case_start`; cada `Write` llama a `PumpMessages`.

## Casos (8640 pares)

| Función | Casos | Variación principal |
|---|---|---|
| ClearVehicleUpgradeFlags | 512 | modId 15/16/17/otros (también 0x10010), flags 0x38C, RadioType/BassSetting (incl. 3/0x80/0xFF), índice de col especial -128..3 |
| SetVehicleUpgradeFlags | 512 | modId, índices de modelo = los de nitro/hidráulica, tipo de vehículo, estéreo |
| GetUpgrade / RemoveUpgrade | 384 / 384 | upgradeId 0..31 y -1, atomics con bit 0x800, CarMod coincidente o no |
| RemoveAllUpgrades | 192 | lista de atomics del clump, relleno de 15 int16 |
| GetReplacementUpgrade / RemoveReplacementUpgrade | 256 / 256 | nodeId/frameId -5..294, árbol de marcos |
| CreateUpgradeAtomic / CreateReplacementAtomic | 384 / 512 | dañado/no, rueda/no, flags de 16 y 32 bits, geometrías con/sin alfa |
| CustomCarPlate_TextureCreate / Destroy | 256 / 64 | con/sin material de matrícula y texto |
| GetRemapIndex / SetRemap | 384 / 384 | nº de remaps -2..40, coincidencias fuera de las 4 primeras entradas, índices int16 con basura en la parte alta |
| GetVehicleAppearance | 256 | flags de modelo con 0..4 bits de 0xF000000 |
| SetComponentVisibility / SetComponentAtomicAlpha | 256 / 192 | marco nulo, estados 0/1/2, alfa -5..1000 |
| UpdateClumpAlpha | 384 | alfa -5..1000 (incluye > 255), bFadeOut |
| SetVehicleAtomicVisibilityCB Object / Frame | 384 / 256 | data 0..4, 0x10000, -1, INT_MAX; árbol de marcos |
| RemoveObjectsCB Object / Frame | 512 / 256 | atomic u otro tipo, jerarquía 0/1/7/-1, modelo presente/ausente, bit 0x800 |
| CopyObjectsCB / RemoveAllUpgradesCB | 256 / 256 | atomic u otro tipo, modelo presente/ausente |
| SetupUpgradesAfterLoad | 128 | 15 mejoras con -1 y valores varios |
| GetSpecialColModel | 512 | índice -128..127, ranuras libres/ocupadas/this, `m_pColData` nulo o no |
| SetTransmissionRotation | 512 | marco nulo, ángulos/posiciones hasta ±50, X de la rueda 0, Y de la rueda = Y del marco |

## Diferencias halladas y corregidas (fuente: `Entity/Vehicle/Vehicle.cpp`)

* `RemoveUpgrade` (0x6D3630): el EXE pasa el identificador **por valor** en `data` (`RemoveUpgradeCB` lo compara directamente); la DLL pasaba
  `&upgradeId`, con lo que nunca coincidía. Corregido.
* `ClearVehicleUpgradeFlags` caso 17 (0x6D326D): el EXE actúa con `RadioType == CIVILIAN` (salta si es distinto de 0) y `bUpgradedStereo`; la DLL
  tenía `!=`. Corregido. Caso 16: `m_aSpecialColVehicle[idx]` con el int8 sin comprobar el rango (`.data()[]`).
* `CreateReplacementAtomic` (0x6D3700): (a) la rama de ruedas escribe la diagonal a 1 y los otros 9 floats a 0 sin tocar pad1/pad2/pad3, hace
  `flags |= 0x20003` **antes** de `RwMatrixUpdate` (la DLL usaba `CMatrix::GetIdentity().UpdateRwMatrix`, que además escribe los pads y no llama a
  `RwMatrixUpdate` en ese punto); (b) el EXE **no** llama a `SetAtomicFlag(ATOMIC_ALPHA)` con material con alfa (sí lo hace `CreateUpgradeAtomic`):
  el ayudante `SetupUpgradeAtomicRendering` recibe ahora `setAlphaFlag`.
* `CustomCarPlate_TextureCreate` (0x6D111D): el EXE llama a `SetCustomCarPlateText(nullptr)`; la DLL pasaba el texto.
* `SetVehicleAtomicVisibilityCB` (Object, 0x6D2690): el EXE usa `GetAtomicId` (0x732370) dos veces; la DLL usaba `GetUserValue` (0x7323A0) una vez.
* `UpdateClumpAlpha` (0x6D29B0): con alfa >= 255 el EXE conserva el valor; la DLL lo recortaba a 255.
* `GetRemapIndex` (0x6D0B70): `GetNumRemaps` se llama una vez al empezar y una por iteración fallida (la DLL lo llamaba dos veces seguidas al
  empezar); índice int16 (movsx) sin comprobar rango (`.data()[]`).
* `SetRemap` (0x6D0C00): el índice se trunca a int16 (movsx) y se lee sin comprobar el rango (`.data()[]`).
* `RemoveReplacementUpgrade` (0x6D39E0): el EXE busca el marco del modelo (`GetFrameFromId`) **antes** de guardar el clump destino de `CopyObjectsCB`.
* `GetSpecialColModel` (0x6DF3D0): índice con signo de 8 bits sin comprobar el rango (`.data()[]`); `CCollision::RemoveTrianglePlanes(&cm)`
  (0x4185A0) en lugar de `CColModel::RemoveTrianglePlanes` (0x40FA40).
* `SetTransmissionRotation` (0x6DBBB0): `atan2` con `fpatan` del x87 a precisión completa (`TransmissionAtan2`, no el CRT), `(L + R) * 0.5` y PI exacto
  del EXE (0x858CB8 = 0x40490FDB) en lugar del `PI` impreciso de common.h.

Sin diferencias (MATCH): `SetVehicleUpgradeFlags`, `GetUpgrade`, `RemoveAllUpgrades`, `GetReplacementUpgrade`, `CreateUpgradeAtomic`,
`CustomCarPlate_TextureDestroy`, `GetVehicleAppearance`, `SetComponentVisibility`, `SetComponentAtomicAlpha`, `SetVehicleAtomicVisibilityCB` (Frame),
`RemoveObjectsCB` (Object y Frame), `CopyObjectsCB`, `RemoveAllUpgradesCB`, `SetupUpgradesAfterLoad`.

Ninguna de estas funciones tiene desviaciones `FIX_BUGS`/`NOTSA` que cambien el comportamiento.

## No cubierto

* Los efectos reales de RenderWare (destrucción de atomics/marcos, listas reales, texturas reales), `CreatePlateTexture` real (crea texturas en el
  pool de RW), el audio real, el registro de referencias real (`CReference`) y los `CColModel` reales de los vehículos (reservas de `AllocateData`,
  copia de `operator=`): se sustituyen por dobles trazados. Todo eso pertenece a otras clases.
* `CQuaternion::Get(RwMatrix*)` se sustituye por un doble (se valida en su propia clase); `AddVehicleUpgrade` (0x6E3290) no está transcrita y se traza.
* `SetCompAlphaCB`, `RemoveUpgradeCB`, `FindUpgradeCB`, `FindReplacementUpgradeCB` son envoltorios del EXE (sin transcribir): se ejecuta el código del EXE.
* `HasAlphaMaterialCB` escribe un `bool`; la DLL lo recibe en un `RpMaterial*` inicializado a null (equivalente: se comprueba como puntero tras poner solo el
  byte bajo a 1).

## Uso (no ejecutado)

`python reversing/CVehC/build_probe.py` y después `python reversing/CVehC/run_local.py`. Filtro opcional: fichero `cvehc-only.txt` con `Clase::Función`
(p. ej. `CVehicle::GetSpecialColModel`).
