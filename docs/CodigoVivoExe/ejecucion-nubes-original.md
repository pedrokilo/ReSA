<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Nubes del standalone frente al original puro, con la cámara mirando al cielo (24/11/2026)

Rama `fix-nubes` (worktree `D:\Original\wv-nubes`, base `b9cdff2c0a`). Herramientas: `reversing/FnComparar/comparar.py` (fase nueva `--fase cielo`, más `nrot`, `mtest`, `ctest`), `cielo_analizar.py`, `cielo_agregar.py`, scripts de análisis en `reversing/FnComparar/analisis_cielo/`. Evidencia (composiciones original | standalone | |diferencia| x4 a 800x600 por panel): `reversing/FnComparar/evidencia_cielo/` (12 de 100 escenas, el resto en `ejecuciones/`, sin versionar). Standalone probado: `reversing/FnFase8/salida/gta_sa_standalone.exe` de `wv-int` (23/11 23:21). No se cambió código del juego.

## 1. Veredicto

**No se encontró ninguna diferencia real de nubes ni de cielo entre el standalone y el original puro** en las condiciones medidas. No hubo nada que corregir ni recompilar.

* Cielo despejado (clima 0) y nublado ligero (2): idénticos dentro del ruido (|dif| media del cielo 0,0-1,4 frente a 0,0-0,7 del original contra sí mismo).
* Nublado (4) y muy nublado (7): en la primera tanda el standalone salía 3-7 niveles más claro en 6 de 8 celdas, pero con 5 repeticiones por celda (a 3 horas) las medias son 41,3/42,0, 231,1/232,8, 218,9/219,5 (clima 4) y 60,4/62,2, 224,1/228,0, 128,8/129,6 (clima 7), con desviaciones de 1-8 dentro de cada modo. No es una diferencia demostrable.
* Lluvia (8): ruido enorme en los dos modos (luminancia del cielo 70-123 entre repeticiones consecutivas, ambos con la misma amplitud y secuencias repetibles dentro de cada modo). Con 16 capturas por modo y hora: 12:00 media 98 (orig) frente a 91 (std); 18:00 media 91 frente a 90. Sin relámpago activo (`LightningFlash` 0 en todas). Sin diferencia demostrable.
* Horizonte (z -0,05): el tráfico y los peatones aleatorios dominan (|dif| 8-22 incluso original contra original); no sirve para medir nubes.
* Volumétricas a 450 m: ver sección 4.

## 2. Método (y lo que costó que fuera válido)

* Escena: punto (2652,5,-1719,9) y (2658,1,-1433,7), horas 6/12/18/0, climas 0/2/4/7/8 por memoria (`CClock` 0xB70153/52, `CWeather::ForcedWeatherType` 0xC81318), ventana 800x600 de cliente, captura completa con BitBlt DPI-aware y tamaño validado (100 % de las capturas válidas).
* **Cámara al cielo.** La orientación por escritura de ángulos de `CCam` de la comparación anterior no sube de z = -0,05 (el seguimiento de CJ lo devuelve), y el ratón real por `SendInput` se comporta distinto en cada modo (en el original la calza de ventana lo vuelve inconsistente). Se resolvió escribiendo `CMouseControllerState::m_AmountMoved` (0xB73424/28) cada ~1 ms en bucle cerrado sobre el vector frente leído (0xB6F9AC), calibrando el signo en cada ejecución. Alcanza z = +0,66..+0,71 (límite del juego +0,707) y z = -0,012 en el plano de horizonte, azimut 90 +/- 3 grados, igual en los dos modos (`orient_ok` verdadero en las 100 escenas finales).
* **Cámara ociosa.** Sin tocar el mando, `CIdleCam` gira la cámara a ángulos arbitrarios a los pocos minutos (corrompió la primera tanda: yaw y pitch distintos en cada escena y modo). Se evita refrescando `CPad::LastTimeTouched` (CPad[0] 0xB73458 + 0x120) con el reloj de juego. Las tandas A1 y B1 se descartaron por esto.
* Ruido: original contra original (ejecuciones `o_C1` y `o_C2`), standalone contra standalone (`s_C1`, `s_C2`), más repeticiones dentro de la ejecución (`o_D1`, `s_D1`: n = 5; `o_E1/E2`, `s_E1/E2`: n = 8 + 8).
* Métrica: rectángulo de cielo sobre CJ (x 180-470, y 0-230), media por canal y |dif| media por píxel; en altura y punto 4 la franja y < 300 completa.

## 3. Tablas

Standalone menos original, medias de 2 ejecuciones por modo, punto 2, mirando arriba (`evidencia_cielo/_agg_C.json`). `oo`/`ss` = |dif| media entre las dos ejecuciones del mismo modo; `os` = entre modos.

| escena (hora, clima) | dif. de medias RGB | oo | ss | os (4 pares) |
|---|---|---:|---:|---|
| 6, 0 | -1,0 / 0,0 / 0,0 | 0,3 | 0,6 | 0,3-0,8 |
| 12, 0 | 0,0 / -0,9 / 0,0 | 1,2 | 1,9 | 0,2-2,0 |
| 18, 0 | 0,4 / -1,0 / 0,0 | 0,1 | 0,9 | 0,2-1,1 |
| 0, 2 | -0,1 / -0,6 / -1,6 | 0,0 | 1,3 | 0,1-1,4 |
| 12, 2 | -0,2 / 0,0 / 0,0 | 0,2 | 0,1 | 0,1-0,3 |
| 18, 2 | 0,1 / 0,0 / 0,0 | 0,1 | 0,2 | 0,2-0,3 |
| 12, 4 | +7,1 / +6,3 / +5,3 | 6,5 | 1,6 | 8,7-10,4 (con n = 5: 231,1 frente a 232,8) |
| 18, 4 | +3,7 / +2,9 / +3,2 | 0,5 | 1,8 | 6,3-6,7 (con n = 5: 218,9 frente a 219,5) |
| 6, 7 | +4,3 / +3,8 / +3,6 | 0,9 | 1,5 | 3,8-4,6 (con n = 5: 60,4 frente a 62,2) |
| 12, 7 | +1,8 / -0,6 / -1,2 | 1,3 | 3,3 | 6,8-8,0 (con n = 5: 224,1 frente a 228,0) |
| 12, 8 (lluvia) | +19 / +20 / +20 | 8,6 | 15,1 | 15-28 (con n = 16: 98 frente a 91) |
| 18, 8 (lluvia) | -14 / -15 / -15 | 10,9 | 10,4 | 20-26 (con n = 16: 91 frente a 90) |

Punto 4 (2658,-1434; `_agg_G.json`, 2 ejecuciones por modo, horas 12 y 18, climas 0, 4, 7, mirando arriba): dif. de medias de -3,4 a +3,4 por canal con ruido original contra original de hasta 2,4, signo mixto. Sin diferencia demostrable.

Variables leídas en cada captura (original por dirección, standalone por el mapa de enlace): `CloudCoverage` 0,0 en clima 0 y 1,0 en 4 y 7 en los dos; `Wind` 0,7 en 4 y 7; `ExtraSunnyness` 1 en clima 0; `Foggyness`, `Rain`, `Rainbow` 0; `VolMaxDist` 600 (400 con el reloj en 0 h) y `VolUsedNum` 180 en los dos; `VolDensity` 1,0; `VolHeightSwitch` 1.

## 4. A pie no se dibujan las nubes esponjosas, también en el original

`CClouds::VolumetricCloudsRender` sale sin dibujar a pie en el standalone (`VolFader` = 255). **El original hace lo mismo**: `VolFader` (0xC6E970) = 255 en todas las escenas a pie del original, y el desensamblado de 0x716380 coincide con el C++ (altura de cámara < 220 m con `[0x8D538C]` = 1: `fader += paso*4`, si llega a 255 devuelve). Con CJ sostenido a 450 m (teletransporte y velocidad nula a 20 Hz) el fader baja a 0 en los dos y las 64+ nubes volumétricas pasan a "usadas", pero con clima 0, 4 y 7 a 12:00 no se ve forma de nube en ninguno (cielo azul liso o blanco por cobertura/niebla) y las dos imágenes coinciden (|dif| 1,3-3,0 de media en la franja de cielo, todo borde del HUD y de CJ; `evidencia_cielo/F1/`). Un único rasgo distinto: en `c_p2_h12_c00_pi-0.05_r0` el original muestra una estela blanca fina a la izquierda que el standalone no tiene (una captura; origen no identificado, posiblemente una estela de avión; no se repitió).

## 5. Otras comprobaciones

* **`CloudRotation` (0xC6AA70)** difiere de forma acumulada (original -1,70 y standalone -1,19 con clima 4; -3,69 y -2,53 con clima 7), pero es el historial desde el arranque (orientación de cámara durante la carga y la casa). La tasa instantánea es idéntica (-0,012492 y -0,012482 por segundo, 24,90 y 24,89 fotogramas por segundo, `m_fOrientation` y `Wind` iguales). El desensamblado de `CClouds::Update` (0x712FF0) coincide con el C++. Además nada en `CClouds::Render` (0x713950-0x716FFF) lee 0xC6AA70 ni 0xC6AA6C: no afecta a lo que se dibuja. `IndividualRotation` se leyó mal al principio (como float) y luego bien; solo cambia con `Update`.
* **Dato en dos sitios.** Las lecturas por dirección de `Clouds.cpp` (0xC6E958, 0xC6E964, 0xB6F9AC, 0xC4B950/54, 0x8D579C, `ms_vc` 0xC6AAB0, `ms_mf` 0xC6C158) no tienen un objeto `DatosExe` en la misma dirección y todos los usuarios usan la misma vía; las tablas `DatosExe` de `CClouds::Render` (nubes bajas 12 floats, arcoíris 3x6 bytes, estrellas 9 floats) tienen tamaño correcto. No se encontró doble ubicación en el camino de las nubes.
* `CTimeCycle::m_CurrentColours` (volcado de 256 bytes por escena): iguales salvo campos de interpolación del reloj y los bytes de relleno de los objetos propios.

## 6. Lo que NO se verificó

* Cámara en vehículo y en avión (no se probó vehículo). Casa de CJ exterior, punto (2672,-1652), horas con la cámara al cielo en el punto 4 en los climas 2 y 8.
* Climas 1, 3, 5, 6, 9 y superiores.
* Nubes volumétricas dibujadas de verdad: a 450 m no se ven en el original en esos climas; no se probó otra altura (el EXE a 220-450 m con la cámara en descenso, ni un avión) ni la hora 0 o 6 en altura.
* Rayos: no se produjo ningún relámpago en las capturas; el efecto de `LightningFlash` sobre el cielo no se comparó.
* Estela blanca del original en `F1` (una captura; no se repitió ni se identificó).
* La prueba de humo de la ASI (ropero y marcadores) no se ejecutó: no hay cambio de código. `python contrib/audit_hook_catalog.py --strict` da OK sin cambios.
* El mecanismo de ratón por memoria elimina la diferencia entre `SendInput` en el original y en el standalone, pero esa diferencia existe y no se investigó (en el original un `dy` pequeño mueve z unos 0,25 por evento y en el standalone casi nada; en el original no se pudo subir con `SendInput`). Si importa el ratón del standalone, hay que medirlo aparte.
