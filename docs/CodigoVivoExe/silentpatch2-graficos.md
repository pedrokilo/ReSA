# SilentPatch, segunda tanda: gráficos, HUD, escalado a resolución, sombras, coronas y tiempo (rama `silentpatch2-graficos`, 25/11/2026)

Fuente: SilentPatch para SA (MIT, Copyright (c) 2024 Adrian Zdanowicz «Silent»; `CHANGELOG-SA.md`, `Config\SilentPatchSA.ini`, `SilentPatchSA\*.cpp`; atribución en [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)). Se aplica **reconstruyendo el efecto en nuestras funciones C++** (no se copian parches de bytes); cada sitio lleva `// SilentPatch (MIT, Silent): <arreglo>`. Clasificación por viñeta (cada línea del changelog que casa con el filtro amplio de gráficos, 78 líneas más las de vehículos que tocan el dibujado): [silentpatch-clasificacion/graficos.csv](silentpatch-clasificacion/graficos.csv). Desviaciones registradas en [DesviacionesConocidas.md](../DesviacionesConocidas.md), sección 15.20. Primera tanda: [silentpatch-graficos.md](silentpatch-graficos.md).

## Cómo apagarlo

- Todo SilentPatch: `GTASA_SILENTPATCH=0` (entorno) o `SilentPatch=0` en `gta-reversed.ini`. Apagado, `SilentPatchOption()` devuelve `false` y cada sitio ejecuta el código del EXE (conservado literal en la rama «apagada»).
- Un arreglo: `GTASA_SILENTPATCH_<NOMBRE EN MAYÚSCULAS>=0|1` o `SilentPatch_<Nombre>=0|1` en el ini. Las opciones se leen una vez por proceso.
- Mismo nombre y valor por defecto que `SilentPatchSA.ini` cuando existe allí (`SlidingMissionTitleText=0`, `SlidingOddJobText=0`, `MinimalHUD=0`, `SmallSteamTexts` sin activar, `ColouredZoneNames=0`, `DirectionalFromSun=0`, `SunSizeHack=1`); el resto no tiene clave en el ini de SilentPatch (siempre activos allí): nombre propio y activado, salvo `DesktopResolutionDefault` y `PreciseFrameLimiter` (apagadas; motivo en la tabla).

## Implementado (clase A) en esta tanda

| Viñeta (L) | Función nuestra | Opción (defecto) | Estado |
| --- | --- | --- | --- |
@TABLA@

## Ya hechas en la primera tanda (verificadas, sin tocar)

`CoronaRotation` (L80), `SunSizeHack`, `StaticShadowAlphaFix` (L81: sombras de la luz del fuego y de los pickups), `ScriptSpritesBilinear` (L59), `TimecycMissingDataFix` (L57-58), `StreamedEntityList` (L187), `FixRadioStationNameWrap` (L148: verificado en `AERadioTrackManager.cpp`, `SetCentreSize(ancho)` antes del `PrintString`), `Extra6Rule` (la regla del extra 6 de los componentes, no las luces: las luces de L213 son `ExtraDirectionalLights` de esta tanda).

## Clase B/C/D/E y bloqueados

@NOAPLICA@

## Pruebas

@PRUEBAS@
