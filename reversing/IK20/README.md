<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cadenas IK (lista de 20)

Validadas el 03/10/2026 con **11.264 pares original/candidato dentro de GTA**, todos exactos: 19 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `IKChain_c` | `Init`, `Exit`, `Update`, `MoveBonesToTarget`, `IsAtTarget`, `IsFacingTarget`, `UpdateEntity`, `GetBoneNodeFromTag`, `ClampLimits`, `GetLimits`, `SetLimits`, `GetBoneSpeed`, `SetBoneSpeed`, `GetPriority`, `SetBlend`, `SetOffsetBoneTag`, `SetOffsetPos`, `UpdateOffset`, `UpdateTarget` |

**Diferencias reales corregidas:**
- `Init`:
  - `m_Ped` se escribe en bruto (sin `CleanUpOldReference`) y la referencia solo se registra si la cadena es válida;
  - pone `m_Blend` a 0 y `m_Bones` a nulo antes de `SetupBones`;
  - la entidad objetivo se registra sin limpiar la anterior (`UpdateEntity` en línea).
- `UpdateEntity`: escritura directa y `RegisterReference`, sin `CleanUpOldReference` de la entidad anterior.
- `Update`: la matriz del pivote sale de `GetAnimHierarchyFromSkinClump` (0x734A40) y del índice con signo, no de `GetAnimHierarchyFromClump`.
- `MoveBonesToTarget`:
  - dirección al efector `(efector + raíz) - hueso`;
  - longitudes con `RwV3dLength` del EXE (raíz por tabla) y NaN descartado;
  - producto escalar `(y·y + x·x) + z·z` y solo sigue si `dot < 0,997` (NaN se descarta);
  - ángulo `GetSpeed · acos · m_Speed · 57,29578` en el x87 con el acos del CRT del juego (resultado redondeado a double como en 0x828EA7);
  - inverso del cuaternión en otro cuaternión;
  - `Limit` solo si el indicador 0x8D2BD2 está activo;
  - la posición del hueso del objetivo se escribe con 0x5E4280 directamente en `m_OffsetPosWS`.
- `IsFacingTarget`: la dirección al objetivo parte de la posición del hueso raíz (no del efector transformado) y el producto escalar sigue el orden del EXE.
- `IsAtTarget`: longitud con `RwV3dLength` del EXE en vez de `CVector::Magnitude`.

**Método:**
- Cadena falsa de 0x58 bytes con relleno aleatorio y nodos `BoneNode_c` reales (`Init`, `AddChild` y `CalcWldMat` del EXE) sobre cadenas de huesos reales de `ms_boneInfos`.
- Peatón falso con clump, datos de mezcla, `AnimBlendFrameData` y frames interpolados propios; jerarquía RpHAnim falsa con matrices aleatorias; dos entidades objetivo con `RwFrame` propio.
- Se simulan y trazan `GetAnimHierarchyFromSkinClump`, `SetupBones`, `CPed::GetBonePosition`, `ReturnBoneNode` y `operator delete[]` (solo en `Exit`).
- Se comparan la cadena, los nodos, los frames, las referencias de las entidades falsas (que se limpian en cada ejecución) y los retornos; se alternan y restauran 0x8D2BD0, 0x8D2BD1 y 0x8D2BD2.
- Fuera de los fixtures:
  - etiquetas que no están en la cadena para `ClampLimits`, límites y velocidad (el EXE llama con `this` nulo);
  - efectores fuera de la jerarquía en `Init` (lee `frames[-1]`);
  - cadenas vacías y `|dot| > 1` en el acos.

Repetir: `python reversing/IK20/build_probe.py` y `python reversing/run.py --case ik20`. Evidencia: [validation-summary.json](validation-summary.json).
