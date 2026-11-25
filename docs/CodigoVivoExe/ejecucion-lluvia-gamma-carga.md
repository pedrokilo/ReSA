<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lluvia, gamma y carga del standalone frente al original (24/11/2026)

Rama `fix-lluvia-carga` (worktree `D:\Original\wv-lluvia`, base `origin/main` = `e7bf0896ee`). Continúa `ejecucion-comparacion-original.md`. Herramientas nuevas o ampliadas en `reversing/FnComparar/`: `comparar.py` (fases `lluvia` y `carga`, y muestreo de la pantalla de carga con `--cap-gs8 1`), `estad_lluvia.py`, `gamma_rampa.py`, `gamma_prueba_captura.py` y `ejecutar_comparar.ps1` (anota la rampa de gamma antes y después de cada ejecución en `gamma.txt` y admite `-IniExtra`). Evidencia (registros, JSON, PNG, estadísticas) en `reversing/FnComparar/evidencia/lluvia-gamma-carga/`. Standalone usado: el de la comparación anterior (`wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe`, 23/11 23:21), sin recompilar. **No se cambió código del juego**: ninguna de las tres diferencias resultó ser un defecto demostrable del standalone.

## 1. Lluvia (clima 8)

**Conclusión: no es un defecto; es ruido propio del juego (rayos y variación de la intensidad de lluvia) con el mismo reparto en los dos.**

Método: en el mismo punto, hora y clima forzado se repite la captura 8-12 veces (3 s entre capturas, cámara fija, reloj a minuto 0), en el original y en el standalone, con la misma sonda. En cada captura se leen 38 variables de `CWeather` (`WVARS` en `comparar.py`); en el standalone las que son objetos propios `DatosExe` se resuelven con el `.map` del exe (están en otra dirección, p. ej. `bC812CC` en `0x0341F8D2`), en el original por dirección.

1. **El original ya es muy ruidoso consigo mismo.** `p3_h18_c08`, el peor par de la tabla anterior: 12 capturas del original dan canal verde 127-146 y una con ráfaga de rayos de 168 (en la tabla anterior eran 136 en el original frente a 176 en el standalone). El 173/176/162 del standalone es un fotograma de destello: el original da 166,8/168,5/152,5 en el suyo. Con `LightningFlash` activo el cielo y la niebla pasan a 255 y la luz ambiente a 1,0 (`app_game.cpp`, `app_light.cpp`).
2. **Misma distribución en los dos** (canal verde medio; capturas por escena; entre asteriscos, fotogramas con destello):

| escena | original (n=8) media / sd / valores | standalone (n=8) media / sd / valores |
|---|---|---|
| p2 h00 | 67,9 / 5,3 / 72 74 63 62 76 64 69 63 | 66,9 / 3,6 / 72 64 65 65 67 65 74 64 |
| p2 h06 | 80,5 / 32,4 / *165* 68 64 64 63 68 75 78 | 79,3 / 34,4 / *169* 63 64 59 65 65 69 80 |
| p3 h00 | 81,0 / 3,8 / 90 83 79 79 79 80 79 80 | 79,9 / 5,5 / 92 77 75 76 85 77 76 78 |
| p4 h12 | 119,4 / 16,3 / 115 112 111 117 *162* 112 116 110 | 114,5 / 18,5 / 110 109 *163* 108 103 112 103 108 |
| p3 h18 (n=12) | 139,5 / 10,3 / 127-146 y *168* | 132,7 / 2,4 / 129-137 |

   Medias dentro de 1-7 de color y destellos de 160-170 en ambos (capturas de esta sesión con valor mayor o igual a 160: original 3 de 44, standalone 2 de 44). Las dos primeras filas muestran incluso un destello en la primera captura de la escena en los dos modos. No hay sesgo sistemático del standalone.
3. **Las variables de `CWeather` coinciden** (original por dirección, standalone por objeto): `Rain` recorre 0,7-0,8-0,9-1,0 en el mismo ciclo y `HeadLightsSpectrum` = `Rain`, `RainFXAlpha` = 0,2 `Rain`, `WetRoads` 1, `CloudCoverage` 1, `Old` = `New` = `Forced` = 8, `StreamAfterRainFlag` 1, `Foggyness` 0, `ExtraSunnyness` 0 en los dos; `LightningBurst`/`LightningFlash` se activan y se leen en los dos. La hipótesis de la variable de clima en un objeto propio mientras otro código lee otra dirección no se sostiene: en `source/` los únicos accesos por literal a `0xC812xx-0xC813xx` son `WeatherRegion`, `ForcedWeatherType`, `OldWeatherType` y `NewWeatherType` (`StaticRef`: misma memoria para quien escribe y quien lee) y comentarios. Los ceros de `cov/fog/extra` de la tabla anterior eran lecturas del standalone por la dirección del EXE (donde no están sus objetos) y valores reales cero en el original.
4. **Sin explicar:** el valor del original en `p2_h06_c08` de la tabla anterior (verde 84) queda por encima de las 7 capturas sin destello (63-78); es una sola muestra y puede ser un destello parcial, no lo reproduje. En `p3_h18` el original tuvo más dispersión (sd 10,3) que el standalone (2,4); en las otras escenas es parecida.

Consecuencia: las filas de clima 8 de `ejecucion-comparacion-original.md` no demuestran diferencia del standalone.

## 2. Gamma de la primera ejecución del standalone (`s_int1`, ~0,85 más oscura)

**Conclusión: la hipótesis de gamma heredada queda refutada con medidas; la causa de `s_int1` sigue sin explicar y no se reproduce.**

* **El juego no cambia la rampa en estos escenarios.** Volcados de `gamma` (0xC92134) en 4 ejecuciones de la comparación anterior (`o_int2`, `a_int2`, `s_int2`, `s_int3`): `m_IsEnabled` 1, `m_CurrentLevel` 0,5, `m_TargetLevel` -1000, `m_PrevLevel` 1000 (valores de `CGamma::Init`), es decir `SetGamma` no llega a ejecutarse y `gbGammaChanged` sigue falso: ni se guarda `ms_SavedGamma`, ni se hereda nada, ni hay qué restaurar. `ResetGammaWhenExiting` (`app_game.cpp:122`) solo restaura si `gbGammaChanged`. El brillo guardado (`m_PrefsBrightness` = 384, offset 0x3C del volcado del menú) es el mismo en original y standalone.
* **Rampa antes y después de cada ejecución** (`gamma.txt`, `GetDeviceGammaRamp` del escritorio): identidad (`ramp[i] = 257 i`) en las 22 lecturas de 11 ejecuciones de esta sesión (original, ASI y standalone; incluida una original cerrada con `Stop-Process` y las demás cerradas con `TerminateProcess` por la sonda). Ni el original ni el standalone dejan la rampa cambiada en estos flujos.
* **La rampa de hardware no afecta a la captura.** `gamma_prueba_captura.py`: con una rampa potencia 1,2 (la que mejor ajusta la curva de `s_int1`: 120 pasa a ~99, 248 a ~245) aplicada con `SetDeviceGammaRamp`, la media del escritorio capturada con el mismo `BitBlt` de la sonda no cambia (36,1/36,1/36,2 antes y después) aunque la rampa leída sí cambia (`ramp[128]` 28660 frente a 32896). Además en `s_int1` `PrintWindow` y `BitBlt` dan lo mismo (52,1/47,2/35,6 y 52,0/47,2/35,5): es el contenido real de la ventana, no la salida del escaneo.
* **Primera ejecución tras el original, como `s_int1` tras `o_int1`:** `o_carga1` seguida de `s_carga1` da la casa del standalone en 61,9/55,9/42,4, igual que la del original (61,9/56,0/42,4). No se reproduce el oscurecimiento.
* Lo que sí mostró `s_int1`: oscurecimiento también en el HUD (radar 38,9 frente a 46,2; HUD superior 63,3 frente a 72,0), no lineal (cercano a potencia 1,2, con el blanco conservado). La nota anterior decía «solo escena 3D»; no es así. Qué lo produjo no se pudo determinar. Descartado: gamma del juego y rampa de hardware. No descartado: un cambio del sistema en ese momento (HDR, luz nocturna, otro programa) o un estado propio de esa ejecución.

## 3. Carga de `gGameState` 8 a 9

**Conclusión: el standalone no omite datos de carga; la diferencia de tiempo está en la presentación de la pantalla de carga y en la ruta de menú del original. No he encontrado nada que falte, pero no he identificado el mecanismo exacto del ahorro.**

Tiempos 8→9 (misma sonda, `ARRANQUE` en `comparar.log`):

| ejecución | 8→9 |
|---|---|
| original, menú por teclado (`o_carga1`) | 11,5 s |
| EXE original + ASI, FastLoader ranura 2 (`a_carga1`) | 8,7 s |
| ASI con `NoLoadScreen = true` y `NoLoadBar = true` (`a_carga_nols`) | 3,0 s |
| standalone (`s_carga1`) | 1,9-2,2 s |
| standalone con `NoLoadScreen` y `NoLoadBar` (`s_carga_nols`) | 1,9 s |

* **Estado de streaming tras la carga igual.** `CStreamingInfo` (`0x8E4CC0`, 26316 entradas, `loadState`) muestreado cada 5 s durante 60 s tras `gGameState = 9` (12 volcados por ejecución): histograma idéntico en original y standalone (no cargados 23001, cargados 2480, pedidos 426, en canal 79, terminando 317, 13 en estado 5), mismo uso de memoria de streaming (9111552 bytes), y no cambia durante los 60 s (nada aparece después). Por rango en los dos: DFF 1901, TXD 513, COL 35, IPL 26, IFP 5. Entrada a entrada difieren 22 de 26316, todas TXD (índices 2282-3609 dentro del rango TXD): 11 cargadas solo en el original y 11 solo en el standalone, intercambio de TXD de props, no carencia (`diff_streaming_orig_std.txt`).
* **El contador de `CLoadingScreen` llega a 140 trozos en el standalone** (muestreado en gs 8: 1, 43, 64, ... 140 en ~1,8 s): se cargan los 140 trozos, no se salta la carga. La pantalla de carga se dibuja (misma imagen media 72,8/66,3/57,9 en el ASI y en el standalone) y termina con un fundido; la del ASI dura unos 5 s más.
* **Dónde está la diferencia con el ASI:** quitar la pantalla de carga baja el ASI de 8,7 a 3,0 s, y el standalone no cambia (1,9 s con y sin ella): con el dibujo y la presentación por trozo el EXE paga unos 5,7 s que el standalone no paga. Por qué no los paga el standalone **no se identificó** (sospecha sin comprobar: la presentación del dispositivo D3D9 del standalone no espera por trozo; no se comparó `PresentationInterval`). Queda ~1 s sin atribuir entre el ASI sin pantalla de carga (3,0 s) y el standalone (1,9 s). El original por menú añade el recorrido del menú y la música de carga.
* No verificado: el contenido de las colisiones por modelo (solo estado de carga por índice), ni el estado tras alejarse de la casa (los pools exteriores están en la comparación anterior).

## 4. Reproducir

```
pwsh -File reversing\FnComparar\ejecutar_comparar.ps1 -Modo orig -Etiqueta o_ll -Limite 900 -Sonda comparar.py -SondaArgs "x --fase lluvia --escenas 0:6,2:12,0:0,1:0 --climas 8 --n 8 --gap 3"
pwsh -File reversing\FnComparar\ejecutar_comparar.ps1 -Modo std  -Etiqueta s_ll -Limite 900 -Sonda comparar.py -SondaArgs "x --fase lluvia --escenas 0:6,2:12,0:0,1:0 --climas 8 --n 8 --gap 3"
python -I reversing\FnComparar\estad_lluvia.py <dir orig> <dir std>
pwsh -File reversing\FnComparar\ejecutar_comparar.ps1 -Modo std -Etiqueta s_carga -Limite 300 -Sonda comparar.py -SondaArgs "x --fase carga --dur 60"
python -I reversing\FnComparar\gamma_rampa.py ; python -I reversing\FnComparar\gamma_prueba_captura.py
```

`--escenas` son pares `punto:hora` (índices 0-2 de `PUNTOS`). El primer argumento de `-SondaArgs` (`x`) evita que `pwsh -File` tome el valor por un parámetro al empezar por `--`. En el standalone `comparar.py` resuelve las variables propias con `--mapa` (por defecto el `.map` de `wv-int`).
