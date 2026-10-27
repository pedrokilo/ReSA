<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote de audio: Ambience, Weather y StaticChannel

[Inicio](../../README.md) · [Registro de validaciones](../ValidationRegistry/README.md) · [Proceso y continuidad](../MEJORAS_DEL_PROCESO.md#continuidad-vigente)

El 01/10/2026 las tres funciones del lote quedaron reconstruidas, habilitadas, desbloqueadas y con `validated=1`, tras **10.086 pares exactos dentro del motor**:

| Función | Dirección | Pares | Clase |
| --- | --- | ---: | --- |
| `CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume` | 0x4D6E60 | 7.846 | **7/7** |
| `CAEWeatherAudioEntity::AddAudioEvent` | 0x506800 | 656 | **5/5** |
| `CAEStaticChannel::SetAudioBuffer` | 0x4F0C40 | 1.584 | **8/8** |

Las tres figuran en el [registro](../validation-registry.json), y los hashes de sus fuentes coinciden con el manifiesto de la compilación probada (`artifacts/20261001-220721-744302`).

## Qué se corrigió

El C++ de las tres ya existía, pero ninguna estaba activa en el juego. El banco encontró un defecto real en cada una:

- **`UpdateAmbienceTrackAndVolume`**, caso `water-fade-pitch`. El C++ suponía que el original, en 0x4D7486, leía el *volumen* al bajar la frecuencia bajo el agua (una "sobrepasada" deliberada). Pero en 0x4D7485 hay un `pop edi` **antes** del `fld [esp+14h]`, así que ese desplazamiento apunta a la **frecuencia objetivo**. Ahora la línea es `next > target ? next : target`; la comparación es explícita porque, con NaN, el original también carga la frecuencia objetivo.
- **`AddAudioEvent`**, caso `thunder-gates`. La velocidad del trueno izquierdo difería en un ulp. La constante del original en 0x862E44 vale `0x3EB674D2` (0.35636002f), y el literal `0.35636f` del C++ redondea a `0x3EB674D1`. Ahora se usa `0.35636002f`.
- **`SetAudioBuffer`**, caso `pcm-size-frequency`. Los flags del buffer de DirectSound no coincidían. El original calcula `lea ecx,[ecx*4+80B4h]` en 0x4F0D56, es decir `GLOBALFOCUS | CTRLVOLUME | CTRLFREQUENCY | CTRL3D | LOCHARDWARE/LOCSOFTWARE`. El C++ ponía `GETCURRENTPOSITION2` en lugar de `GLOBALFOCUS`.

## Banco

Los archivos `Probe.hpp`, `LiveComparison.hpp` e `instrument.py` venían del 30/09 y no había forma de compilarlos ni de ejecutarlos. Se añadieron `build.py` y `run_local.py`, siguiendo el esquema de ConversationForPed, y el caso `audio-completion` en `cases.json` y `run_all.py`.

Cada caso ejecuta el original y el candidato por separado, compara el estado completo y la traza de llamadas, comprueba la entrada original de la función y restaura el estado del motor. Proveedores controlados en cada función:

- **Ambience:** zonas de audio activas, radio, clima, lluvia por zona, cámara, distancia al agua, atenuación, disturbios y `StopAmbienceTrack`.
- **Weather:** área, lluvia del jugador y de la cámara, banco cargado, inicialización y petición de sonidos.
- **StaticChannel:** dispositivo y buffers de DirectSound simulados (`Create`, `Lock`, `Unlock`, `Position`, `Query`, `Volume`, con fallos inyectados) y `ConvertFromBytesToMS`.

| Familia | Función | Qué cubre |
| --- | --- | --- |
| `audio-zone`, `zone-track-transition`, `radio-handoff` | Ambience | Cada zona −2…69, transiciones de estado y pista, traspaso de radio |
| `weather-zone-priority`, `water-riot-stop`, `water-fade-pitch`, `dam-height`, `water-distance`, `mission-track` | Ambience | Tormenta de arena, agua, disturbios, rampas de volumen y frecuencia, presa, misiones |
| `thunder-gates`, `thunder-voices` | Weather | Puertas de lluvia y área, banco cargado, contador de variación, duraciones límite, volúmenes, peticiones fallidas |
| `pcm-size-frequency`, `pcm-loop-failures`, `pcm-loop-layout` | StaticChannel | Tamaños y frecuencias límite, bucles, siete puntos de fallo y buffers previos |

**Resultado:** 10.086 de 10.086 pares iguales, cierre natural con código 0, partidas e INI intactos. [Resumen](validation-summary.json).

Pasos para repetirlo:

```powershell
python reversing/AudioCompletion/build.py
python reversing/AudioCompletion/run_local.py
```

La ejecución tarda unos ocho minutos, sobre todo por los casos PCM, y escribe un registro de unos 350 MB en la copia de pruebas. El límite del ejecutor es de 600 s. Cada compilación archiva unos 790 MB de ASI/PDB en `artifacts/`; los binarios de compilaciones superadas pueden borrarse.

## Límites

- No se escucha ni se valida la mezcla o reproducción real del audio.
- `dependency_review` es nulo: las fuentes probadas coinciden, pero la vigencia global queda pendiente, igual que en las demás acreditaciones.
- Una ejecución terminó con código 3 justo después de `Initialised SoundManager`, antes de llegar a los casos. Es el fallo intermitente al iniciar el audio que ya estaba documentado; al repetir se completó.

**Instalación:** el ASI normal con los tres hooks activos pasó 60 s dentro de partida con el audio activo. `audit_hook_catalog.py --runtime` no encontró diferencias entre `hooks.csv` y el registro real. Al terminar se restauró el ASI instalado (`a0c5e7fb…`).
