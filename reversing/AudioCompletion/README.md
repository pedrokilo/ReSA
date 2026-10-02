# CAEAmbienceTrackManager — 7/7

[Inicio](../../README.md) · [Registro de validaciones](../ValidationRegistry/README.md) · [Proceso y continuidad](../MEJORAS_DEL_PROCESO.md#continuidad-vigente)

`UpdateAmbienceTrackAndVolume` (0x4D6E60) quedó reconstruida, habilitada y desbloqueada el 01/10/2026. Con ella, `CAEAmbienceTrackManager` llega a **7/7**. La entrada tiene `validated=1`, vinculada a **7.846 pares exactos dentro del motor**, y figura en el [registro](../validation-registry.json).

## Qué se corrigió

El C++ de esta función ya estaba escrito en el árbol, con las constantes x87 del original, pero seguía con `{ .reversed = false }`. El banco encontró una diferencia en el caso `water-fade-pitch`, al bajar la frecuencia cuando la cámara vuelve a estar bajo el agua.

- **Lo que hacía el C++:** suponía que el original, en 0x4D7486, leía el *volumen objetivo* en lugar de la frecuencia (una "sobrepasada" deliberada).
- **Lo que hace el original:** en 0x4D7485 hay un `pop edi` **antes** del `fld [esp+14h]`. Al desplazarse la pila 4 bytes, ese operando apunta a la **frecuencia objetivo**, de modo que la "sobrepasada" no existe.
- **Corrección:** `m_FreqFactor = next > target ? next : target`. La comparación es explícita porque, con NaN, el original también carga la frecuencia objetivo.

## Banco

Los archivos `Probe.hpp`, `LiveComparison.hpp` e `instrument.py` venían del 30/09 y no tenían forma de compilarse ni de ejecutarse. Se añadieron `build.py` y `run_local.py`, siguiendo el esquema de ConversationForPed, y el caso `audio-completion` en `cases.json` y `run_all.py`.

Cada caso ejecuta el original y el candidato por separado. Se controlan los proveedores externos: zonas de audio activas, radio (encendida, emisora, inicio y parada), clima, lluvia por zona, cámara, distancia al agua, atenuación, disturbios y `StopAmbienceTrack`. Se comparan los 44 bytes del gestor y la traza completa de llamadas, se comprueba la entrada original de la función y se restaura el estado del motor. Las familias de casos cubren:

| Familia | Qué cubre |
| --- | --- |
| `audio-zone` | Cada zona −2…69, esfera, caja y ambas, radio y estado |
| `zone-track-transition` | Zonas con distancia, estados 0–8, pistas y volúmenes límite |
| `radio-handoff` | Emisora anterior y actual, radio encendida, misión, zona 30 |
| `weather-zone-priority` | Tormenta de arena, área, lluvia de jugador y cámara, misión |
| `water-riot-stop` | Bajo el agua, atenuación límite, disturbios, radio del vehículo |
| `water-fade-pitch` | Rampas de volumen y frecuencia (el caso corregido) |
| `dam-height` | Zona 4 y la altura 1372 |
| `water-distance` | Distancias y alturas respecto al agua |
| `mission-track` | Pista de misión frente a estado y pista actual |

**Resultado:** los 7.846 casos de `UpdateAmbienceTrackAndVolume` coinciden y el juego se cierra de forma natural con código 0. Partidas e INI quedan intactos. [Resumen](validation-summary.json), con el manifiesto de compilación `artifacts/20261001-212155-352577`.

Pasos para repetirlo:

```powershell
python reversing/AudioCompletion/build.py
python reversing/AudioCompletion/run_local.py
```

## Límites

- La sonda se detiene en la primera diferencia. Después de la ambientación prueba `CAEWeatherAudioEntity::AddAudioEvent`, y el caso `thunder-gates-7897` difiere en un ulp de la velocidad de un sonido (0x3EB6DBD2 frente a 0x3EB6DBD1). `SetAudioBuffer` no llegó a compararse. **Ninguna de las dos está validada aquí.**
- No se escucha ni se valida la mezcla real del audio.
- `dependency_review` es nulo: coinciden los hashes de las fuentes probadas, pero la vigencia global está pendiente, igual que en las demás acreditaciones.

**Instalación:** el ASI normal con el hook activo pasó 60 s dentro de partida con el audio activo. El `runtime-hooks.csv` muestra `UpdateAmbienceTrackAndVolume` con `reversed=1`. Al terminar se restauró el ASI instalado (`a0c5e7fb…`).
