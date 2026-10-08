<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Núcleo y datos (lista de 70)

Validadas el 02/10/2026 con **14.992 pares original/candidato dentro de GTA**, todos exactos: 44 funciones con `validated=1`.

| Clase | Funciones |
|---|---|
| `CBox` | `Recalc`, `Set` |
| `CBuilding` | `IsBuildingPointerValid`, `ReplaceWithNewModel` |
| `CCollisionPlugin` | `PluginAttach`, `SetModelInfo` |
| `CColTrianglePlane` | `GetNormal-Out`, `Set` |
| `CCompressedMatrixNotAligned` | `CompressFromFullMatrix`, `DecompressIntoFullMatrix` |
| `CControllerState` | `CheckForInput`, `Clear` |
| `CData` | `Load`, `Unload` |
| `CDraw` | `CalculateAspectRatio`, `SetFOV` |
| `CDummyObject` | `CreateObject`, `UpdateFromObject` |
| `CEntryInfoNode`, `CNodeRoute` | `operator new`, `operator delete` |
| `CObjectData` | `Initialise`, `SetObjectData` |
| `CObjectPool` | `GetAt`, `New` |
| `CPedPool` | `GetAtRef`, `New` |
| `CObjectSaveStructure`, `CSimpleVariablesSaveStructure`, `CVehicleSaveStructure` | `Construct`, `Extract` |
| `CTempColModels` | `Initialise`, `Shutdown` |
| `ModelIndices` | `InitModelIndices`, `MatchAllModelStrings` |
| `FxPrimBP_c` | `GetRWMatrix`, `Load` |
| `CWindModifiers` | `FindWindModifier`, `RegisterOne` |
| `CPlantColEntEntry` | `AddEntry`, `ReleaseEntry` |

**Diferencias reales corregidas:**
- `CDraw::CalculateAspectRatio` devuelve el valor en ST0: los 5 llamadores del EXE lo sacan con `fstp st(0)` y la versión `void` desbalanceaba la pila x87.
- `CMatrix::Reorthogonalise` (fuera de la lista, usado por `DecompressIntoFullMatrix` y en todo el juego): transcripción exacta del x87 del EXE (productos vectoriales en línea, normalización sin comprobar cero con la suma z²+y²+x²).
- `CCompressedMatrixNotAligned::Decompress`: multiplica por el float 1/127 (no divide entre 127).
- `CColTrianglePlane::Set`:
  - distancia y orientación calculadas con la normal ya comprimida;
  - suma z, y, x;
  - sin copiar el relleno.
- `CData`:
  - `Load` lee byte a byte como el EXE y devuelve falso si falla una lectura;
  - `Unload` comprueba nulo.
- `CObjectData::Initialise`:
  - las entradas por defecto se copian con todos sus bytes (`rep movs`);
  - índice de evitar cámara;
  - flotabilidad `((100/p)·masa)·0,008`;
  - comparación del byte bajo de las chispas.
- `CObjectPool`/`CPedPool::New` y `CNodeRoute::operator delete` usan la política del EXE (sin relleno 0xCD/0xDD).
- `CBuilding::IsBuildingPointerValid`: diferencia con signo y división truncada, como `IsDummyPointerValid`.
- `CSimpleVariablesSaveStructure`:
  - los nombres largos terminan en "...";
  - el indicador de mensaje de la cámara cinematográfica es un byte (vale 2), no un `bool`.
- `CTempColModels::Initialise`:
  - constantes que no son el float más cercano;
  - el arma solo escribe la caja y el radio.
- `ModelIndices::InitModelIndices` reinicia solo los 116 índices que reinicia el EXE.
- `FxPrimBP_c::GetRWMatrix` multiplica por 1/32767 y construye la identidad a mano.
- `CWindModifiers`:
  - `RegisterOne` suma z²+y²+x² y escribe el contador como índice + 1;
  - `FindWindModifier` solo con valor 1, multiplicando por las constantes del EXE, sin dividir.
- `CPlantColEntEntry::AddEntry`: número de triángulos con signo.

**Método:**
- Pools falsos.
- Estructuras de guardado con objetos, vehículos y matrices falsos.
- Variables globales de `CSimpleVariablesSaveStructure`, tablas de `CTempColModels` y `ModelIndices` aleatorizadas y restauradas.
- Archivos (`LoadLine`, `Read`, `OpenFile`), RNG, `GetLocalTime`, model infos, efectos y memoria simulados.
- Los fixtures no contienen NaN señalizadores: el EXE copia los floats con `fld`/`fstp`, lo que los silencia, y los datos reales nunca los tienen.

Repetir: `python reversing/Core70/build_probe.py` y `python reversing/run.py --case core70`. Evidencia: [validation-summary.json](validation-summary.json).
