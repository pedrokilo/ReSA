<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plugin RpAnimBlend (lista de 43)

Banco preparado el 04/10/2026 con **13.248 pares original/candidato dentro de GTA** (pendiente de ejecutar): 42 funciones de 1 clase.

| Clase | Funciones |
|---|---|
| `RpAnimBlend` | `ClumpAnimConstruct`, `ClumpAnimCopy`, `ClumpAnimDestruct`, `RpAnimBlendPluginAttach`, `RpAnimBlendAllocateData`, `RpAnimBlendClumpInit`, `RpAnimBlendClumpIsInitialized`, `RtAnimBlendKeyFrameApply`, `RpAnimBlendKeyFrameInterpolate`, `RpAnimBlendCreateAnimationForHierarchy`, `RpAnimBlendFrameGetName`, `RpAnimBlendFrameSetName`, `RpAnimBlendClumpAddAssociation`, `RpAnimBlendClumpExtractAssociations`, `RpAnimBlendClumpGiveAssociations`, `RpAnimBlendClumpFillFrameArray`, `RpAnimBlendClumpFindBone`, `RpAnimBlendClumpFindFrame`, `RpAnimBlendClumpFindFrameFromHashKey`, `RpAnimBlendClumpGetAssociation` (id, nombre y jerarquía), `RpAnimBlendClumpGetFirstAssociation` (con y sin flags), `RpAnimBlendClumpGetMainAssociation`, `RpAnimBlendClumpGetMainAssociation_N`, `RpAnimBlendClumpGetMainPartialAssociation`, `RpAnimBlendClumpGetMainPartialAssociation_N`, `RpAnimBlendClumpGetNumAssociations`, `RpAnimBlendClumpGetNumNonPartialAssociations`, `RpAnimBlendClumpGetNumPartialAssociations`, `RpAnimBlendClumpPauseAllAnimations`, `RpAnimBlendClumpUnPauseAllAnimations`, `RpAnimBlendClumpRemoveAllAssociations`, `RpAnimBlendClumpRemoveAssociations`, `RpAnimBlendClumpSetBlendDeltas`, `RpAnimBlendGetNextAssociation` (con y sin flags), `FrameUpdateCallBackSkinned`, `FrameUpdateCallBackNonSkinned`, `FrameUpdateCallBackCompressedSkinned`, `FrameUpdateCallBackCompressedNonSkinned` |

**Diferencias reales corregidas:**
- `ClumpAnimConstruct` anula el puntero en el offset recibido como argumento (no en el global `0xB5F878`).
- `ClumpAnimDestruct` anula el puntero del plugin después de liberar los datos (antes, antes de liberarlos).
- `RpAnimBlendPluginAttach`:
  - cualquier offset negativo es un fallo (`jge`; antes solo `-1`);
  - devuelve `RwBool` (EAX completo): el llamador `0x53D8AE` hace `test eax,eax` y un `bool` deja basura en los bits altos.
- `RpAnimBlendAllocateData` devuelve el puntero creado (EAX).
- `RpAnimBlendClumpInit`:
  - mira el skin antes de reservar los datos (cada rama los reserva, como `0x4D6510`/`0x4D66A0`);
  - skinned: posiciones de los bones con `SkinGetBonePositionsToTable` (`0x735360`) sobre una tabla de 64 a cero, en vez de recalcularlas con `RwMatrixInvert`; jerarquía pedida después de `SetNumberOfBones`; contador con signo;
  - no skinned: cuenta los hijos con `RwFrameForAllChildren` (sin `RwFrameCount`);
  - recorridos con `ForAllFrames` (contador con signo).
- `RpAnimBlendClumpAddAssociation` estaba sin implementar (`NOTSA_UNREACHABLE`): enlaza al principio, llama a `Start`, hace OR de la palabra baja de los flags y asigna el blend.
- `RpAnimBlendClumpExtractAssociations` anula también el `prev` de la cabeza.
- `RpAnimBlendClumpFillFrameArray` usa la jerarquía del skin (`0x734A40`, antes la del frame `0x734B10`).
- `RpAnimBlendClumpFindBone`, `FindFrame` y `FindFrameFromHashKey` dejan el resultado en el global del EXE `0xB5F87C`, usan `_stricmp` y recorren con `ForAllFrames`.
- `RpAnimBlendFrameSetName` devuelve el frame (EAX).
- `RpAnimBlendGetNextAssociation` (flags): sin `assert(flags)`.
- `RpAnimBlendClumpGetNum*Associations` y `RpAnimBlendClumpPause`/`UnPauseAllAnimations` recorren desde `m_AnimList.next`: el iterador de `CAnimBlendLink` empieza en la propia cabeza y trataba `bd - 4` como una asociación (contaba una de más y escribía el flag fuera del bloque).
- `FrameUpdateCallBack*` (los cuatro):
  - normalizan con la rutina del EXE (`0x4D1610`, x87: multiplica por el recíproco; antes dividía);
  - skinned: con un producto escalar NaN se suma el quaternion (`test ah,5`);
  - no skinned: `pos = T + (1 - b)·FramePos` (la traslación no se escala por el blend; antes `lerp`);
  - las llamadas en ensamblador a las extracciones de velocidad del EXE limpian el argumento (`add esp,4`, cdecl).

**Método:**
- Clump (byte de tipo 2 y plugin en el offset real de `0xB5F878`), `CAnimBlendClumpData`, asociaciones (vtable falsa: el `delete` virtual se traza y desenlaza sin liberar), jerarquías, frame datas, RwFrames falsos (nombre en el offset real del plugin NodeName, hijos y hermanos enlazados a mano), keyframes de interpolación, nodos con secuencias y keyframes (normales y comprimidos), jerarquía HAnim, interpolador, atomic/skin y arena son búferes estáticos del banco: los punteros se comparan en crudo.
- Se guardan y restauran en cada ejecución `0xB5F878`, `0xB5F87C` y `0xB4EA0C`; el offset del plugin se restaura justo después de llamar a `RpAnimBlendPluginAttach`.
- Se simulan y trazan sin reservar memoria, solo durante la llamada y solo en el hilo principal:
  - `operator new`/`delete` y `CMemoryMgr::MallocAlign`/`FreeAlign` (arena fija rellena con 0xCD);
  - `GetFirstAtomic`, `RpSkinGeometryGetSkin`, `GetAnimHierarchyFromSkinClump`, `SkinGetBonePositionsToTable` y `CVisibilityPlugins::GetFrameHierarchyId`;
  - `CAnimBlendAssociation::Start`, `RtAnimAnimationCreate`, `RpClumpRegisterPlugin` y `RtAnimRegisterInterpolationScheme` (los callbacks de la DLL se leen como las direcciones del EXE).
- Corren de verdad (ya validadas): `CAnimBlendClumpData`, `CAnimBlendNode::Update`/`UpdateCompressed`/`CalcDeltas`, `GetFrameNodeName`/`SetFrameNodeName`, `CKeyGen::GetUppercaseKey`, `RpHAnimIDGetIndex`, `RwFrameForAllChildren`, `RwMatrixUpdate` y las extracciones de velocidad del EXE.
- `RpAnimBlendClumpIsInitialized` compara solo AL (los llamadores del EXE hacen `test al,al`).
- Fuera de los fixtures:
  - listas vacías en `ExtractAssociations` (el EXE escribe en `[0+4]`);
  - callbacks de actualización sin ningún array de nodos o, con extracción de velocidad, sin ningún nodo válido (el EXE se cae);
  - quaternions diminutos (la longitud del EXE vive en registros x87 con exponente extendido);
  - `operator new` nulo en `AllocateData` y `Init`.

Repetir: `python reversing/RpAnim43/build_probe.py` y `python reversing/run.py --case rpanim43`. Evidencia: `validation-summary.json` (tras la ejecución).
