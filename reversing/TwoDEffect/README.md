<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
