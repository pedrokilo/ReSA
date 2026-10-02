# BreakablePlugin y BreakManager_c

Validadas el 02/10/2026 con **3.232 pares original/candidato dentro de GTA**, todos exactos: las 6 funciones de `BreakablePlugin` y las 6 de `BreakManager_c`. 12/12 con `validated=1`.

**Diferencia real corregida:** `BreakManager_c::Render(uint8 state)` pasa el byte tal cual a `BreakObject_c::Render`, que lo compara con `m_bDrawLast`; la DLL lo convertía en `bool` (un 2 pasaba a 1 y dibujaba los objetos "último"). `BreakObject_c::Render` recibe ahora `uint8`.

**Método:** el registro de plugins de RpGeometry, `RwTextureRead`/`RwTextureDestroy`, el `operator new`/`delete` del EXE (que la DLL también usa; solo interceptados durante la llamada probada), las funciones de `BreakObject_c` y el estado de render (puntero `fpRenderStateSet`) están controlados. Los streams de memoria de RenderWare son reales. `BreakablePluginAttach` se prueba sin registrar nada de verdad.

Repetir: `python reversing/BreakBatch/build_probe.py` y `python reversing/run.py --case break`. Evidencia: [validation-summary.json](validation-summary.json).
