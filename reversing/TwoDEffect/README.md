<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# C2dEffect

Validadas el 02/10/2026 con **3.520 pares original/candidato dentro de GTA**, todos exactos: las 14 funciones de `C2dEffect` (utilidades de señales, `Shutdown`, `DestroyAtomic`, `PluginAttach`, acceso a los efectos de la geometría y constructor/destructor/copia/lectura/escritura/tamaño del plugin) con `validated=1`.

**Diferencias reales corregidas en la lectura del plugin (0x6F9FD0):**
- Luz: `m_bCoronaEnableReflection` es un `eCoronaReflType` y se copia en bruto. Al ser `bool`, la DLL lo normalizaba a 1.
- Atractor de peatones: también se copian `m_nPedExistingProbability`, `field_36`, `m_nFlags` y `m_szScriptName` (había un `TODO`).
- ENEX: `m_nTimeOff` sale de `TimeOff`; la DLL usaba `TimeOn` (errata).
- Partícula: el nombre se copia con `strcpy`, hasta el NUL incluido, sin tocar los bytes posteriores.

**Límites:**
- Solo secciones con tamaño válido; las comprobaciones de tamaño con aviso son añadidas por la DLL.
- ENEX solo en su forma completa: en la corta, el original deja `m_nFlags2` con datos de su búfer de pila (indefinido).
- Los nombres de partícula van terminados en NUL, como en los archivos reales; sin terminador, el original se sale del búfer.

**Método:** geometría falsa (plugin en `g2dEffectPluginOffset`), streams de memoria reales de RenderWare y dependencias controladas (registro de plugins, `RwTextureRead`/`Destroy`, `CMemoryMgr::Malloc`/`Free`, `RpAtomicSetFrame`/`Destroy`, `RwFrameDestroy`).

Repetir: `python reversing/TwoDEffect/build_probe.py` y `python reversing/run.py --case twod`. Evidencia: [validation-summary.json](validation-summary.json).
