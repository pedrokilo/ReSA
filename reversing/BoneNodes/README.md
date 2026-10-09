<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Huesos: BoneNode_c y BoneNodeManager_c

Validadas el 02/10/2026 con **12.256 pares original/candidato dentro de GTA**, exactos bit a bit en los flotantes: las 17 funciones de `BoneNode_c` y las 8 de `BoneNodeManager_c`. 25/25 con `validated=1`.

**Diferencias reales corregidas por el banco:**
- Constructor (0x616B30): no toca `m_BoneTag`; ahora es un `int32` en bruto (antes `WEnum` lo ponía a cero).
- `EulerToQuat` (0x6171F0): el original "normaliza" multiplicando por 1/(x²+y²+z²+w²), **sin raíz**; además usa fsin/fcos x87.
- `QuatToEuler` (0x617080): la simplificación con `asin` no daba el mismo resultado que el original con `atan2`.
- `Limit` (0x617650): el mínimo Z del hueso de la cabeza se compara sin redondear a float, y la sujeción aplica primero el mínimo y luego el máximo.
- `CalcWldMat` (0x616CD0): matriz construida en línea con s = 2/|q|².

Estas cuatro funciones son ahora transcripción x87 literal (`__declspec(naked)`), con el mismo criterio que `CGeneral::GetRandomNumberInRange<float>`, y su fórmula documentada en C++.

**Límites:** `Init` con etiqueta desconocida queda excluido (el original lee `ms_boneInfos[-1]`), igual que `Init` dos veces sin `Exit` (doble inserción, uso inválido).

Repetir: `python reversing/BoneNodes/build_probe.py` y `python reversing/run.py --case bones`. Evidencia: [validation-summary.json](validation-summary.json).
