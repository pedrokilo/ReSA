# Sin tráfico ambiental en el EXE independiente y comparación de nubes (23/11/2026)

Rama `fix-trafico-nubes` (base `d3ec2c9a03`, rama de integración `integracion-fase7b`). Herramientas nuevas en `reversing/FnTraf/` (sonda por memoria, matriz hora x clima, control con el ASI).

## 1. Tráfico: causa raíz

En `gta_sa_standalone.exe` los datos del EXE que el C++ ya guarda en **objetos propios** (`DatosExe::Bss::*`, `DatosExe::Data::*`) no viven en la dirección del EXE. El código que sigue leyéndolos **por dirección fija** ve ceros (o el valor inicial del `.data`) en lugar de lo que escribe el C++. Dos sitios de ese tipo anulaban el tráfico:

1. `source/game_sa/CarCtrlGenerateOneRandomCar.inl` (cuerpo de `CCarCtrl::GenerateOneRandomCar`, 0x430050, elevado por `lift.py` del desensamblado, con `Ld<>/St<>` sobre direcciones literales). Leía/escribía por dirección: `0x969098` (NumLawEnforcerCars), `0x96909C` (NumMissionCars), `0x9690A8/AC` (ambulancias/bomberos), `0x9690B8` (LastTimeLawEnforcerCreated), `0x9690BC` (TimeNextMadDriverChaseCreated), `0x9690CC` (bCarIsBeingCreated), `0xB72958/59` (gbLARiots), `0xC0BC30..3C` (densidades de coches de `CPopCycle`), `0xC0BC64` (zona actual) y `0x8A5B24` (MaxNumberOfCarsInUse). Medido en el standalone: objeto propio `CPopCycle::m_NumOther_Cars...` = `[4.2 0.3 3.6 0.9]` frente a la dirección `0xC0BC30..3C` = `[0 0 0 0]` (líneas `TRAF:` del log). Con suma de densidades 0 y zona nula, la generación de coches aleatorios no hacía nada.
2. `CCarCtrl::ChooseModel` (`CarCtrl.cpp`): el modelo de la categoría «dealers» (0x19) se leía del objeto `DatosExe::Bss::bC0F240`, que **solapa** el arreglo `CPopulation::m_CarGroups` (0xC0ED38 + 28 x 0x2E: el elemento 0 del grupo 28) y nunca se escribe: valía 0. Con 0 «cargado» (el modelo 0 es CJ) se creaba un `CAutomobile` con el modelo 0 y caía en `CVehicle::CustomCarPlate_TextureCreate` (acceso a 0x36, `m_pPlateMaterial` de la info de un modelo de ped). Este fallo estaba tapado por el anterior (nunca se llegaba a crear un coche); aparecía el primer coche aleatorio a los 8-55 s según la ejecución.

### Arreglo (commit `c4161ca2ed`)

* `.inl`: las 20 lecturas/escrituras por dirección pasan a los objetos propios (`DatosExe::Bss::b969098`, `bC0BC30..3C`, `bC0BC64`, `b9690BC`, ...).
* `ChooseModel`: `StaticRef<uint16>(0xC0F240)` (la memoria del arreglo, que es la que rellena `LoadCarGroups`).
* Barrido: `python` sobre todo `source/` (literales 0x858000-0xCB0FFF dentro de objetos de `source/game_sa/DatosExe/*.h`): el resto de coincidencias son constantes de `.rdata` (mismo valor), comentarios o `ScopedStaticRef`. No quedan otras lecturas por dirección de objetos propios en código elevado. Único `.inl` del repositorio con ese defecto.

### Evidencia (standalone, partida 2 de la copia de pruebas, hora 12, mismas posiciones que `--robar`)

| situación | NumRandomCars | pool de vehículos | pool de peds |
| --- | ---: | ---: | ---: |
| ANTES (ejecución `b2`, junto a la casa de CJ, 190 s) | 0-1 | 3-9 (casi todos aparcados) | 9-10 |
| DESPUÉS (`f12`, posición 1 `2652,-1720`) | 12 | 16 | 34 |
| DESPUÉS (posición 2 `2672,-1652`) | 10 | 15 | 28 |
| DESPUÉS (posición 3 `2658,-1434`) | 10 | 11 | 28 |
| CONTROL ASI con el código original de CCarCtrl/CPopCycle/CPopulation/CTheCarGenerators (`c7`) pos 1/2/3 | 4 / 6 / 7 | 8 / 10 / 8 | 6 / 8 / 9 |

Capturas completas del cliente (640x480, validadas contra el rectángulo de la ventana 654x518 con marco): `reversing/FnTraf/ejecuciones/f12-*/pos0_giro0.png` (dos coches antiguos, una moto, cuatro peatones junto a la vía elevada) y `pos1_giro0.png` (moto y peatones). Control: `c7-*/pos0_giro0.png` (un coche cercano y uno lejano, ningún peatón).

Observaciones sin cerrar:
* El standalone corregido genera **más** coches y sobre todo **más peatones** que el control en esos puntos (28-34 peds frente a 6-9). Los peds no pasan por código elevado ni por la ruta corregida; la diferencia no está explicada (el control se midió 14 s después de teletransportar, y el estado de streaming de modelos de peds del original es más lento). No se afirma que el recuento de peatones sea el del original.
* Las capturas son de 640x480 (tamaño del cliente de este ASI/EXE con `SkipDeviceSelection`); no se obtuvo 1024x768.
* Con el ASI «completo» antes de este arreglo la generación aleatoria usa las mismas lecturas por dirección; ver la bisección de la sección 3.

## 2. Nubes

Medido con `reversing/FnTraf/nubes_matriz.py` (fuerza hora por `CClock` y clima por `CWeather::ForcedWeatherType`, apunta la cámara al cielo y vuelca `CTimeCycle::m_CurrentColours`). 56 combinaciones (horas 0, 6, 9, 12, 15, 18, 21 x climas 0, 1, 2, 4, 8, 16, 9, 19) en el standalone (`nm1-*`) y con el ASI completo (`ncm4-*`, mismo código C++ de `CClouds`/`CTimeCycle` sobre el RenderWare y el D3D9 originales del EXE).

* `CTimeCycle` (LowClouds RGB, FluffyCloudsBottom RGB, CloudAlpha, cielo arriba/abajo) **coincide en 55 de las 56 combinaciones** entre standalone y ASI (tolerancia 2 por la interpolación del reloj; se midió con el reloj en el minuto 0 de cada hora).
* Color medio de la imagen (canal a canal): coincide dentro de ±10 en 47 de 56 combinaciones. Difieren: clima 8 (lluvia) a 0 y 6 h (standalone 10-12 más oscuro), clima 16 (tormenta) a 21 h (standalone 86/88/86, ASI 104/127/125), niebla (9) a 12, 15, 18 y 21 h (standalone más clara a mediodía: 137/139/138 frente a 123/124/123; a las 21 h más verde-azul en el ASI) y arena (19) a 12 y 15 h (standalone 13-14 más claro). Diferencias de niebla/atmósfera sin causa investigada.
* Con la cámara de la prueba (bajo el porche de la casa de CJ, vector frente z 0,2-0,56, es decir 11-34 grados sobre el horizonte) **ni el ASI ni el standalone muestran formas de nubes** en ninguna combinación revisada (soleado extra, soleado, nublado 4 y lluvia 8 a las 12 h, 18 h soleado). El cielo nublado (4) es en ambos un velo uniforme blanco-gris; el soleado es en ambos azul cian con gradiente. Es decir, **no se reproduce una diferencia de nubes con esa vista**. El «ExtraSunnyness = 1 da nubes bajas negras» es coherente con el código: `colorLowClouds = LowClouds * (1 - max(ExtraSunny, Fog, Cover)) = 0` y, con mezcla aditiva ONE/ONE, el color 0 no se ve; el original hace lo mismo.
* Contadores del standalone (`CONTADORES:` en `fase8_arranque.log`, ejecución `nm2`): `CClouds::Render` se ejecuta en cada fotograma, 6-7 de las 12 nubes bajas se proyectan a pantalla por fotograma y se encolan en el buffer de sprites. **Nubes esponjosas (volumétricas): `VolumetricCloudsRender` sale antes de dibujar** (`vol_vertices = 0`; el contador de pasadas hasta el bucle de dibujo es 31 frente a ~3500 llamadas de `CClouds::Render`): `gfVolumetricCloudFader` (objeto propio `DatosExe::Bss::bC6E970`) llega a 255 y la rama `m_bVolumetricCloudHeightSwitch` (1) con la cámara por debajo de 220 m devuelve sin dibujar. No se comprobó si el EXE original hace lo mismo a pie (ver sección 4).

## 3. Bisección con el ASI (función culpable)

Misma partida, hora 12 y las tres posiciones de `--robar` (2652,-1720 / 2672,-1652 / 2658,-1434), ASI de `wv-int/bin/Debug` (compilado antes del arreglo), lanzado con `jugar.ps1` de `wv-8\FnBIS1`:

| variante | NumRandomCars (1/2/3) | pool vehículos | pool peds |
| --- | --- | --- | --- |
| ASI completo, sin arreglo (`bis_completo`) | 0 / 0 / 0 | 4 / 5 / 1 | 19 / 1 / 15 |
| `--unhook=Global/CCarCtrl/GenerateOneRandomCar` (`bis_genone`) | 0 / 0 / 0 | 4 / 5 / 1 | 19 / 2 / 14 |
| `--unhook=Global/CCarCtrl,CPopCycle,CPopulation,CTheCarGenerators,CLoadedCarGroup,CCarGenerator` (`c7`, código original de tráfico) | 4 / 6 / 7 | 8 / 10 / 8 | 6 / 8 / 9 |
| standalone con el arreglo (`f12`) | 12 / 10 / 10 | 16 / 15 / 11 | 34 / 28 / 28 |

* El ASI completo reproduce el fallo del standalone (sin coches aleatorios): no es un defecto exclusivo del EXE independiente, es de los datos propios de `DatosExe` frente a las lecturas por dirección.
* Quitar solo `GenerateOneRandomCar` no basta (el código original sigue leyendo `0xC0BC30..3C` por dirección, y `CPopCycle::Update` en C++ escribe sus objetos propios): hay que quitar el grupo completo (`c7`), que es lo que devuelve tráfico. Esto cuadra con la causa: **la función culpable es `CCarCtrl::GenerateOneRandomCar` (cuerpo elevado `CarCtrlGenerateOneRandomCar.inl`) por sus lecturas por dirección, y, detrás de ella, `CCarCtrl::ChooseModel` por el modelo de dealers (`bC0F240`)**. La bisección por función con el ASI mixto no puede aislarla porque las dos mitades (C++ y original) comparten datos que ya no están sincronizados.
* No se pudo hacer la bisección por grupos, funciones y comandos completa (`ChooseModel` sola, CPathFind, CStreaming, CTheScripts): el candado `gta_lock` lo ocupó otro lanzador durante horas y se dedicó el tiempo a lo ya medido. La causa se identificó leyendo el código y se confirmó con las líneas `TRAF:`/`CHOOSE:` del standalone (modelo 0 devuelto por `ChooseModel` con tipo 25) y con el efecto del arreglo.

## 4. Lo que NO se pudo verificar

* El control «original puro» (`--unhook-except=RenderWare/WindowedModeHooks`) **no llega a cargar la partida**: la DLL actual lo deja en `gGameState = 0` y la DLL antigua `ff45...` tampoco carga con FastLoader sin sus ganchos. `--unhook=Global/CClouds` cuelga el ASI actual (cae a los 20 s: el resto de código C++ usa objetos propios de DatosExe que el código original no actualiza). Por eso el control de tráfico desactiva solo las clases de tráfico y el de nubes es el ASI completo.
* No se pudo apuntar la cámara al cielo abierto (queda bajo el porche), ni se comparó el estado de `gfVolumetricCloudFader` del ASI (la sonda lo lee desde la ejecución `bis_genone` en adelante).
* Capturas de 1024x768 (el cliente de estas ejecuciones es de 640x480).
* Dos ejecuciones de control (`ncm1`, `c6`) se adjuntaron por error al `gta_sa.exe` de otro lanzador vivo en ese momento (la sonda buscaba por nombre) y escribieron en su memoria (ForegroundApp, hora y clima forzados): quedaron descartadas y la sonda ahora recibe el PID propio (`--pid`, marcado en la línea de comandos).

## 5. Cómo reproducir

```
bash /d/Original/_cc_TRAF.sh source/game_sa/CarCtrl.cpp     # recompila a over\ y enlaza reversing\FnFase8\salida
bash /d/Original/_run_traf.sh f12 diag5 partida2 330 15 '--salir --hora 12 --posiciones 2652.5,-1719.9,13.8;2672.1,-1652.5,11.5;2658.1,-1433.7,31.5' 8
python -I reversing/FnTraf/control_traf.py c7 reversing/FnTraf/control_trafico.args --espera 15 --auto 300 --sonda "--hora 12 --posiciones ..."
```
