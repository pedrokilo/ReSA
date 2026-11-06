<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 06/11/2026 (Ciudad de México):** 837 grupos / 9160 funciones reconstruidas; 9160 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Player82C (lote 82C)

`CPlayerPed::SetRealMoveAnim` (EXE 0x60A9C0, thiscall, ECX = jugador, sin argumentos, `ret`, void; 0xA97 bytes medidos con `dis.sh`: 0x60A9C0..0x60B456). Hook `S`.

La funcion que mezcla las animaciones de movimiento del jugador (andar, correr, sprint, arranque, reposo, frenada de carrera, apuntado lateral, cansancio). Pide de una vez diez animaciones con
`RpAnimBlendClumpGetAssociation` (ids 0, 1, 2, 5, 3, 6, 7, 0x89, 0x8A, 0x0A: andar, correr, sprint, `WALK_START`, reposo, `RUN_STOP`, `RUN_STOP_R`, apuntado izquierda y derecha, cansado) y decide por ramas
cual crear (`BlendAnimation`/`AddAnimation`), cual mezclar y cual borrar (destructor virtual, ranura 0):
1. `bResetWalkAnims` (bit 0 de +0x470): pone a 0 el tiempo de andar, correr y sprint y apaga el bit.
2. `RUN_STOP` o `RUN_STOP_R` reproduciendose: estado = correr (y arranca `RUN_STOP` si le queda tiempo).
3. Con alguna con delta >= 0 (no se reproduce): se desvanece (delta -8) y se fuerza el reposo (`RestoreHeadingRate`).
4. Con ratio 0 (`m_fMoveBlendRatio`, datos+0x14) y sin sprint: apuntado lateral (pad con direccion forzada, camara activa en modo 4: `AimWeaponLeftRight` tres veces, animacion 0x89 si es negativo o 0x8A, velocidad `|v| / 128`) y, con
   `m_fTimeCanRun` < 0 y sin tarea de pelea, disparo, agacharse, lanzar ni jetpack ni esfera de mundo, crea la animacion de cansancio (grupo 0x4C si `GetDefaultPlayerMotionGroup` es 0x37); si no, la cancela (delta -2). Estado = parado.
5. Si no (rama principal): con reposo, crea o reutiliza `WALK_START`, borra el reposo y el sprint y estado = andar; borra frenadas y apuntados; crea andar y correr si faltan; cierra `WALK_START` cuando ha terminado
   (suma `timeStep + currentTime` en la pila x87); y reparte mezcla y estado: bloque del sprint (cierre con `RUN_STOP`/`RUN_STOP_R` segun `currentTime / total`, desvanecimiento), sprint desde correr (estadisticas) o
   mezcla andar/correr por el ratio (< 1, < 2, >= 2; estadisticas al correr).
6. Final comun (0x60B399): adrenalina (escala de tiempo 1/3 y velocidad 2, o fin), velocidad del sprint (0.7 con la camara en modo 15; si no `max(1, GetButtonSprintResults(0))`).

## Reconstruccion
`PlayerPed.cpp` (el metodo, ~240 lineas) y `X87Math.cpp` (`Player82CWalkStartFinishedCpp`: el unico calculo con un temporal x87 sin guardar que importa, `total <= timeStep + currentTime`, con la suma redondeada a 24 bits de
mantisa y exponente ampliado). El resto de los float se guardan como float en cada paso. Condiciones float con la semantica de `fcomp` + `test ah` del EXE: `>= 0` es "ordenado", `!(x < 0)` incluye NaN.
Defecto del primer intento corregido por el banco: la cancelacion del cansancio escribe delta -2 (0xC0000000), no -4; y con FTZ activo en SSE (MXCSR 0x9F80 del juego) un `timeStep` desnormal daba una suma aplastada a cero.

## Banco
- 16.384 pares (16 familias de 1.024 casos): `RUN_STOP` reproduciendose, desvanecimiento, apuntado lateral (izquierda y derecha, con y sin la animacion existente), cansancio (creacion, existente, cancelacion, ocupado por pelea,
  disparo, agacharse, lanzar, jetpack o esfera), rama principal con y sin reposo, cierre de `WALK_START`, bloque del sprint (todos los cruces de mezcla / delta / ratio / cociente), sprint desde correr, mezcla andar/correr, adrenalina
  (fin y marcha), camara fija y velocidad del sprint, mas una mezcla amplia con NaN, infinitos, desnormales y ceros con signo.
- Falsos identicos en ambos lados (parche de entrada del EXE y del cuerpo de la DLL; todos con hook): RpAnimBlendClumpGetAssociation 0x4D68B0, BlendAnimation 0x4D4610, AddAnimation 0x4D3AA0 (animaciones nuevas de arenas),
  SetCurrentTime 0x4CEA80, SetDeleteCallback 0x4CEBC0 (el callback del EXE 0x5DFD70 y el de la DLL se registran igual), RestoreHeadingRate 0x5DFD60, GetPadFromPlayer 0x609560, GetForceCameraBehindPlayer 0x540AE0,
  AimWeaponLeftRight 0x541040 (una cifra distinta por llamada), GetTaskFighting 0x600F30, GetTaskUseGun 0x600F70, GetTaskDuck 0x6010A0, GetTaskThrow 0x600FB0, GetTaskJetPack 0x601110, TestSphereAgainstWorld 0x569E20,
  GetDefaultPlayerMotionGroup 0x5A81B0, UpdateStatsWhenSprinting 0x55C660, UpdateStatsWhenRunning 0x55C6F0 y GetButtonSprintResults 0x60A820 (deja EDX intacto como el original: el llamador conserva en EDX la animacion de sprint).
  Registran sus argumentos en orden (punteros como etiquetas de arena). El destructor virtual de las animaciones es la ranura 0 de una tabla falsa (registra el borrado).
- Datos del caso: jugador (matriz, posicion, +0x470, grupo +0x4D4, estado +0x534, +0x560), datos del jugador (+0x14, +0x18, +0x34, +0x3C), 10 animaciones (existentes o ausentes) y 4 nuevas, tres jerarquias, camara activa y modos
  (0xB6F1A8 + 0x238 * i, el EXE lee una palabra), reloj, escala de tiempo (0xB7CB64) y la trampa de adrenalina (0x969177). Constantes vigiladas: 0x858B50, 0x858624, 0x858C58, 0x8D24EC, 0x858B88, 0x858EE8, 0x858C98, 0x858CA0 y las dobles
  0x859EF8 y 0x859060.
- Oraculo independiente (`fixture_oracle.py`, escrito desde el desensamblado): secuencia de llamadas con argumentos, campos escritos en cada animacion, jugador, datos del jugador, globales y borrados; ademas se exige la cobertura de
  las 70 ramas de `ALL_BRANCHES` (todas alcanzadas; ver `coverage.json`) y que ningun byte fuera de los campos modelados cambie.

## Dominio y limites
- NaN canonicalizado (carga util distinta entre x87 y SSE). Camaras activas 0..2; ratio, mezclas y tiempos de la lista de valores del fixture mas especiales y valores anchos.
- Fuera de los fixtures: animaciones ausentes que el EXE desreferencia (andar o correr nulos en la rama principal: se crean antes), `m_BlendHier` nulo, clump nulo, mas de cuatro animaciones nuevas en una pasada (maximo real: cuatro),
  y los 16 bits altos de los valores de `AimWeaponLeftRight` (el fixture los rellena con basura; el EXE solo lee AX).
- `CEntity::GetRpClump` comprueba `RwObject::type == rpCLUMP` en la DLL de depuracion: el fixture fija el tipo del clump falso.
- `GetButtonSprintResults`/`GetPadFromPlayer` del EXE se falsean tambien aqui; su banco propio los valida.

## Sensibilidad
La primera corrida (sin los dos arreglos de arriba y con el callback sin normalizar) dio 1.316 diferencias EXE/DLL de 16.384: el cansancio con -4 en vez de -2 (488 casos), el borrado de `WALK_START` con sumas desnormales (2 casos) y el
callback registrado con la direccion de la DLL (826 casos). Con las correcciones, 0 diferencias y cada lado coincide con el oraculo.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Player82C`; `python -B reversing/Player82C/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>`.
