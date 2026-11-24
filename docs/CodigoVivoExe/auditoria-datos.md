# Auditoria de datos propios (DatosExe) frente a accesos por direccion fija (23/11/2026, rama `auditoria-datos`, base `1a98b831c3`)

Motivo: `GenerateOneRandomCar` y `ChooseModel` fallaban porque leian por direccion lo que el C++ escribia en objetos `DatosExe::*` (ver `ejecucion-trafico-nubes.md`). Aqui se busca el mismo defecto en todo `source/`.

## Metodo (herramientas en `contrib/auditoria_datos/`)

1. Tamano real de los 2152 objetos declarados en `source/game_sa/DatosExe/*.h` (`sizeof` compilado, `gen_sz.py`).
2. 6235 citas por literal 0x858000-0xCB0FFF en `source/` (`StaticRef`, `reinterpret_cast`, `Raw`, `CAM_LF`, `put`, literales sueltos...).
3. Extension real de cada `StaticRef<T>(0x...)`: se parchea `StaticRef` con `-DAUDIT_PROBE` (variable `volatile audit_sz = sizeof(T)`), se compila con `/FAs` StdInc.h y los 156 `.cpp` con `StaticRef` y se lee del ensamblador `push <direccion>; call StaticRef<T>` + el tamano de cada instancia (1708 direcciones con tamano; solo 1 sin resolver, `CPlaneTrail[3]`).
4. Cruce: cita (direccion + extension) contra objeto mutable (.data/.bss, 1756): 80 pares, 53 con objeto referenciado por nombre.
5. Aparte: busqueda de aritmetica de punteros a partir de un objeto escalar (`&DatosExe::...::x + i`, `(&x)[i]`), y de lecturas con base literal e indice (`0x8CC430 + i*4`).

## Fallos corregidos

Los efectos de la ultima columna estan deducidos leyendo el codigo; ninguno se midio en ejecucion. Todos los archivos tocados compilan (`cl` con las opciones del proyecto, sin enlazar).

| Sitio | Direccion | Objeto propio | Quien escribe / lee | Efecto |
| --- | --- | --- | --- | --- |
| CarCtrl.cpp, EventScanner.cpp, Bike.cpp, Automobile.cpp, Population.cpp, PlayerPed.cpp, Stats.cpp | 0x96913F, 0x96914C, 0x969152, 0x96915B, 0x96917A, 0x96917C-E, 0x969180 | `b96913F`... `v969180` (dentro de `CCheat::m_aCheatsActive`, 0x969130, 92 bytes) | los trucos escriben `m_aCheatsActive[i]` (memoria); el codigo leia el objeto (siempre 0) | 9 trucos sin efecto: HAVE_A_BOUNTY, PERFECT_HANDLING, CARS_ON_WATER, GANGS_CONTROL_STREETS, REDUCED_TRAFFIC, WANNA_BE_IN_MY_GANG / NO_ONE_CAN_STOP_US / ROCKET_MAYHEM, MAX_SEX_APPEAL |
| Cam.cpp (`IsTimeToExitThisDWCineyCamMode`) | 0xB6EC70-73, 0xB6EC78 | `bB6EC70..73`, `bB6EC78` | los modos 20-23 y 28 escriben `failed` en el objeto; la funcion leia `0xB6EC5C + camId` | tras fallar un modo de camara cinematografica no salia del modo |
| Cam.cpp / Heli.cpp | 0xC0B184 | `bC0B184` (= `CInterestingEvents::m_nFlags`, instancia 0xC0B058) | `CInterestingEvents` lo escribe por la instancia; Cam.cpp y Heli.cpp usaban el objeto | bit 0 siempre 0 en la camara del helicoptero / camara de objetivo |
| WindowedMode.cpp / rwcore.cpp | 0xC97C34 | `bC97C34` | `WindowedMode` ponia TRUE en el objeto; `_rwD3D9CameraEndUpdate` leia/borraba 0xC97C34 | nunca se copiaba el backbuffer a la textura de camara en modo ventana |
| Cam.cpp (`CCam::...` linea 860) | 0xC8A860 | `bC8A860` (= `gCrossHair[0].m_bClearImmediately`) | `CWeaponEffects` escribe `gCrossHair`; Cam.cpp leia el objeto | no limpiaba la mira fuera del Hydra |
| Cam.cpp (linea 1209) | 0xBAADC0 | `bBAADC0` (= `CHud::m_BigMessage[2][0]`) | HUD escribe el texto; Cam.cpp leia el objeto | mostraba «BUSTED» aunque ya hubiera un mensaje grande |
| Plane.cpp:1236 | 0xA9B76C | `bA9B76C` (= `ms_modelInfoPtrs[425]`, Hunter) | `ms_modelInfoPtrs` esta en memoria; el avion leia un puntero nulo y `modelInfo[0x4A]` | desreferencia nula al volar el Hydra en modo VTOL |
| RunningScriptCommands1100To1199.cpp:314 | 0x8CD5E0 | `v8CD5E0` (= `MI_PICKUP_REVENUE`) | `ModelIndices` escribe el objeto; el comando de script leia el valor inicial del .data | modelo del pickup de ingresos incorrecto |
| AERadioTrackManager.cpp (10 sitios) | 0x8CA1B0, 0x8C9610, 0x8C9040, 0x8C8FB0, 0x8CADD0, 0x8C9010, 0x8C8BF0, 0x8C8C50, 0x8CB280 | `v8CA1B0`... (escalares `int32`) | `&objeto + i*N` / `(&objeto)[i]` leia mas alla del objeto de 4 bytes | rangos de pistas, intros, anuncios y bloqueos de la radio leidos de otras globales: pistas/IDs erroneos |
| AERadioTrackManager.cpp:1491 | 0xB62C1C | `bB62C1C` | `(&bB62C1C)[i]` mas alla del objeto; la tabla es `m_nMusicTrackIndexHistory` | historial de la emisora 11 incorrecto |
| PointLights.cpp:159 | 0x8D5068 | `v8D5068` (escalar `float`) | `table[tab]` mas alla del objeto | escala de sprites de niebla de luces leida de otras globales |

En todos los casos el cambio es minimo: un solo objeto (la memoria de la direccion o el objeto, segun el lado con mas usos) para escritura y lectura. No se eliminaron las definiciones sobrantes de `DatosExe` (los objetos que ahora no usa nadie siguen declarados).

## Revisados y no son fallo

* `bC0F240`, `bC0F074` dentro de `m_CarGroups`: sin referencias por nombre (el primero era el de `ChooseModel`, ya corregido).
* Cam.cpp `constant()`/`K()`, X87Math `K(0x8CDEC0)`: la «direccion» es un indice en un arreglo que se pasa por argumento, no memoria.
* AEAudioUtility (`int32[0x7D0]` en 0x8ABC70 abarca varios objetos .data): la extension declarada es mayor que la tabla real; en el .data ambos lados parten del mismo valor inicial.
* Tablas con base literal e indice (Cam.cpp 0x8CC430/0x8CC4C0/0x8CC600, Hud.cpp, MissionCleanup.cpp, ClothesBuilder.cpp...): el rango de indices no alcanza ningun objeto con nombre.
* Constantes .rdata (`k*`, `v85xxxx` de R5): mismo valor en ambos lados.

## Lo que NO cubre

* Direcciones calculadas en tiempo de ejecucion sin literal en el codigo (punteros leidos de memoria, `base + tabla[i]`), o literal construido por macros/`constexpr` en otra linea (se busco `0x...` en el texto).
* Indices con rango desconocido (revisados a mano solo los de la lista).
* Variables de clase con almacenamiento propio y sin comentario/direccion (no hay forma de enlazarlas con una direccion).
* Datos inicializados (.data) que el C++ cambia por nombre y el codigo vivo del EXE (RenderWare, CRT) lee: queda para el plan de `docs/DatosExe`.
* No se midio nada en ejecucion con el standalone (ver el informe de la rama).
