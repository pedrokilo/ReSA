<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
