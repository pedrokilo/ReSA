<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BreakablePlugin y BreakManager_c

Validadas el 02/10/2026 con **3.232 pares original/candidato dentro de GTA**, todos exactos: las 6 funciones de `BreakablePlugin` y las 6 de `BreakManager_c`. 12/12 con `validated=1`.

**Diferencia real corregida:** `BreakManager_c::Render(uint8 state)` pasa el byte tal cual a `BreakObject_c::Render`, que lo compara con `m_bDrawLast`; la DLL lo convertía en `bool` (un 2 pasaba a 1 y dibujaba los objetos "último"). `BreakObject_c::Render` recibe ahora `uint8`.

**Método:** el registro de plugins de RpGeometry, `RwTextureRead`/`RwTextureDestroy`, el `operator new`/`delete` del EXE (que la DLL también usa; solo interceptados durante la llamada probada), las funciones de `BreakObject_c` y el estado de render (puntero `fpRenderStateSet`) están controlados. Los streams de memoria de RenderWare son reales. `BreakablePluginAttach` se prueba sin registrar nada de verdad.

Repetir: `python reversing/BreakBatch/build_probe.py` y `python reversing/run.py --case break`. Evidencia: [validation-summary.json](validation-summary.json).
