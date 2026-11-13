# Fn181A: 29 funciones del EXE (lote 181A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-3` desde main aaf5bb8072 + el commit del lote 5cdf45c8c8 aplicado con cherry-pick; plantilla `Fn179A` (y por ella `Fn173A`/`Fn171A`/`Fn168A`) con piezas de `Fn166A` (operator new escalar), `Fn151A` (vtables falsas y colaboradores virtuales) y `Fn129A`/`Fn144A` (pool)). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn181a/20261112-192758-640557` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn181A-20261112-192758-640557`, `comparisons.jsonl.xz`/`.gz` de la corrida completa; 1,3 GB sin comprimir). 29 x 16384 = **475136 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `32e5a31c63f9dc274da0c56a8fe137812a6c23988ec2dba54f58ad281797b535`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El lote llegaba reconstruido y enganchado, sin banco (`reversing/PendienteValidar/181A.md` en 5cdf45c8c8). **El banco no encontro ninguna diferencia: no hizo falta corregir ninguna fuente y no se excluyo ninguna funcion** (el lote no saltaba ninguna). Las 29 filas de `docs/hooks.csv` pasan a `validated=1`.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CEntity::IsInCurrentArea` (CPopulation) | 0x407000 | 26 | S |
| 1 | `CRemote::SetPlayerRemoteControlledCar` (CVehicleRecording) | 0x45AD40 | 316 | S |
| 2 | `CMatrix::ScaleAll` (x87) | 0x45AF40 | 47 | S |
| 3 | `CPhysical::SetTurnSpeed(x, y, z)` | 0x45AFB0 | 54 | S |
| 4 | `CEventKnockOffBike::GetEventType` (8) | 0x4AFCB0 | 6 | V |
| 5 | `CEventKnockOffBike::GetLifeTime` (0) | 0x4AFCC0 | 3 | V |
| 6 | `CEventKnockOffBike::GetLocalSoundLevel` (60.0f en ST0) | 0x4AFCD0 | 7 | V |
| 7 | `CEventKnockOffBike::GetEventPriority` (70) | 0x4AFCE0 | 6 | V |
| 8 | `CEventKnockOffBike::IsCriminalEvent` (true) | 0x4B06F0 | 3 | V |
| 9 | `CEventGunAimedAt::TakesPriorityOver` | 0x4B0810 | 138 | V |
| 10 | `CEventGunAimedAt::AffectsPed` | 0x4B4EE0 | 232 | V |
| 11 | `CVehicleAnimGroup::CVehicleAnimGroup` | 0x5BD420 | 113 | S |
| 12 | `CDecisionMaker::operator=` | 0x5BD4E0 | 45 | S |
| 13 | `CDecisionMakerTypes` copia en +0xC1B4 (aleatorio de peatones) | 0x5BD510 | 43 | S |
| 14 | idem +0xCB50 (mision de peatones) | 0x5BD540 | 43 | S |
| 15 | idem +0xD4EC (jugador) | 0x5BD570 | 43 | S |
| 16 | idem +0xDE88 (aleatorio de grupos) | 0x5BD5A0 | 43 | S |
| 17 | idem +0xE824 (mision de grupos) | 0x5BD5D0 | 43 | S |
| 18 | `CTaskComplexDieInCar` constructor | 0x62FC80 | 41 | S |
| 19 | `CTaskComplexDieInCar::GetTaskType` (215) | 0x62FCB0 | 6 | V |
| 20 | `CTaskComplexDieInCar::Clone` | 0x635F90 | 124 | V |
| 21 | `CTaskSimpleDrownInCar::Clone` | 0x636010 | 80 | V |
| 22 | `CTaskComplexDie::Clone` | 0x636060 | 152 | V |
| 23 | `CTxdStore`: lectura de `defaultTxds[indice]` (GetDefaultTxdSlot) | 0x7316D0 | 13 | S |
| 24 | `CPool<TxdDef>::Flush` | 0x731800 | 66 | S |
| 25 | `CPool<TxdDef>::CPool(capacidad, nombre)` | 0x731AD0 | 92 | S |
| 26 | `CPool<TxdDef>::Flush` (segunda copia identica) | 0x731B30 | 66 | S |
| 27 | `CPool<TxdDef>::New` | 0x731B80 | 104 | S |
| 28 | `CPool<TxdDef>` destructor con borrado | 0x731C20 | 84 | S |

Tipo V = `RH_ScopedVMTInstall` (fila `0x00000000` en `docs/hooks.csv`). Fuentes (en `source/game_sa/`): `Entity/Entity.cpp`, `Remote.cpp/.h`, `Core/Matrix.cpp` (con `X87Math.cpp/.h`: `MatrixScaleAllCpp`), `Entity/Physical.cpp`, `Events/EventKnockOffBike.cpp/.h`, `Events/EventGunAimedAt.cpp`, `VehicleAnimGroupData.cpp`, `DecisionMakers/DecisionMaker.cpp`, `DecisionMakers/DecisionMakerTypes.cpp`, `Tasks/TaskTypes/TaskComplexDieInCar.cpp/.h`, `TaskSimpleDrownInCar.cpp/.h`, `TaskComplexDie.cpp/.h`, `TxdStore.cpp`.

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; falsos identicos en la direccion del EXE y en el cuerpo de la DLL de los colaboradores con hook: `FindPlayerVehicle` 0x56E0D0, `CEntity::CleanUpOldReference` 0x571A00, `RegisterReference` 0x571B70, `CCamera::TakeControl` 0x50C7C0, `SetZoomValueCamStringScript` 0x50C1B0, `CPed::IsPlayer` 0x5DF8F0, `CEvent::TakesPriorityOver` 0x4ABC30, `CTaskManager::GetActiveTask` 0x681720, `CPedIntelligence::IsInSeeingRange` 0x600C60, `CPed::IsAlive` 0x5E0170, `CPedGroups::AreInSameGroup` 0x5F7F40, `CVehicle::IsPassenger` 0x6D1BD0, `FindPlayerPed` 0x56E210, `CCrime::ReportCrime` 0x532010, `CDecision::From` 0x6006B0, constructores base de tareas 0x61A3B0 (CTaskComplex) y 0x61A390 (CTaskSimple), `CTask::operator new` 0x61A5A0 y constructor de `CTaskComplexDie` 0x630040 (todos con hook, apagado durante el banco). Solo en la direccion del EXE (la DLL los llama por ella) y con guarda de hilo: `operator new` escalar 0x82119A (en otros hilos llama a malloc del CRT, 0x824257) y `operator delete` 0x8214BD. Las llamadas virtuales de `TakesPriorityOver` y `AffectsPed` (prioridad propia y del otro evento, tipo, origen, tipo de tarea) pasan por vtables falsas estaticas identicas para el EXE y la DLL, que anotan cada llamada (orden incluido). Memoria global del juego tocada y restaurada tras cada lado: `CGame::currArea` 0xB72914, jugador en foco 0xB7CD74, tabla de jugadores 0xB7CE48 (0x320 bytes) y la tabla de slots de TXD por defecto 0xC87F00..0xC88610 (se rellena con un patron por caso).

- **IsInCurrentArea (0)**: byte de area con 0, 1, 2, 13, 0x7F, 0x80, 0xFE, 0xFF y aleatorios; `currArea` igual, distinto, 13, con signo extendido (`0xFFFFFF00 | area`, la trampa de la version con `eAreaCodesS8`) y con el byte bajo igual pero bits altos distintos. Solo cuenta AL.
- **SetPlayerRemoteControlledCar (1)**: jugador en foco 0 o 1; vehiculo remoto actual nulo, el mismo que el nuevo o distinto (en B); primer `FindPlayerVehicle` nulo, el nuevo, el de B o el de C; segunda llamada distinta de la primera; bytes +0x36, +0x428 y +0x42D aleatorios (bit 2 de +0x42D puesto o no); se comprueban los estados 3/8/9, los campos 0x3B7..0x3D0, el bit 0x10, `CleanUpOldReference`, `RegisterReference` (con la direccion del campo del jugador), `TakeControl` con INTERPOLATION o JUMPCUT segun la bandera, y el zoom.
- **ScaleAll (2)**: factor y 12 componentes con ceros con signo, unos, infinitos, NaN senalizantes y silenciosos (con la regla de la x87 para dos NaN), denormales, productos cerca del limite de los denormales y del desbordamiento; el oraculo hace el producto a 24 bits con fracciones exactas y el redondeo de `fstp` a float. El relleno de cada fila se conserva.
- **SetTurnSpeed (3)**: tres dwords arbitrarios (NaN senalizantes incluidos: se copian los bits sin pasar por la x87).
- **Constantes (4-8, 19)**: valor exacto en EAX (AL en `IsCriminalEvent`; bits del float en ST0 en `GetLocalSoundLevel`, con la x87 vacia al volver).
- **TakesPriorityOver (9)**: `m_AimedBy` nulo o no; jugador o no; origen del otro evento nulo, `m_AimedBy`, otro o crudo; tipo 9, 36, 39 u otros; prioridades con signo (-1 frente a 1, INT_MIN, INT_MAX, iguales y a +-1); valor de `CEvent::TakesPriorityOver` con los bytes altos arbitrarios. Se comprueba el orden de las llamadas (el EXE pide primero la prioridad del otro evento).
- **AffectsPed (10)**: jugador, tarea activa nula / de tipo 601 / otra (dos llamadas a `GetActiveTask`, la segunda con otra tarea distinta), `m_AimedBy` nulo, posicion de la matriz o propia, `IsInSeeingRange`, `IsAlive`, mismo grupo, en vehiculo con conductor (`IsPassenger`, +0x460), tipo de peaton 6 (policia: `ReportCrime(22, ped, FindPlayerPed(-1))`) u otros.
- **VAG (11)**: ceros exactos en +0, +1, +4..+0x3F y +0x40..+0x7B; +2..+3 y +0x7C..+0x93 sin tocar (arenas con relleno aleatorio, unos y 0xCD).
- **DecisionMaker (12-17)**: 41 llamadas a `CDecision::From` con ECX = destino + i*0x3C y origen + i*0x3C (se anota cada una); `operator=` devuelve `this`; las copias con `this` ajustado de modo que la region destino quede en una arena.
- **DieInCar (18, 20), Drown (21), ComplexDie (22)**: memoria de `CTask::operator new` nula o en B; arma releida tras la reserva (el falso de `new` cambia el campo y el codigo debe verlo); EAX de los falsos de los constructores (ComplexDie devuelve el EAX del constructor, el resto la memoria); nueve argumentos de pila de `CTaskComplexDie` (de las banderas solo cuenta el byte bajo).
- **GetDefaultTxdSlot (23)**: indices de -64 a 0x2FF (incluidos los extremos y negativos) sobre un patron distinto por caso; se comprueba la extension de signo de la palabra.
- **Pool de TxdDef (24-28)**: como `Fn179A` con elementos de 0xC bytes, `operator delete` / `operator new` escalares: capacidades con signo (cero, negativas, enormes, 1..0x400), almacenamiento y mapa nulos, en arena o crudos, BYTE de propiedad 0, 1, 0x80, 0xFF, 0x100...; `New` con los mapas de 0 libres, 1 libre, 10 %, 50 % y todos libres, cursor desde -1 hasta capacidad - 1 y contadores de 7 bits (incluido 0x7F); el constructor con `cap * 12` que desborda 32 bits.

16 familias de 1024 casos por funcion (`Fixtures.hpp`); `python -I fixture_selfcheck.py` / `selfcheck_per_fn.py` pasan los 475136 casos por el oraculo y cuentan la cobertura de ramas (cada marca de `KEYS` aparece al menos 50 veces por funcion). Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn181A` (para ~475k pares hace falta un envoltorio con timeouts largos: la corrida tarda unos 40 minutos de juego; se uso una sesion unica con candado de juego para la principal y las dos rondas negativas). EAX/ECX/EDX tras la llamada no forman parte de la igualdad (solo se registran): difieren en las funciones que dejan ECX/EDX residuales (`IsInCurrentArea`: el EXE deja ECX = area y EDX = currArea; `SetPlayerRemoteControlledCar`, `ScaleAll`, `SetTurnSpeed`, constructor de `CVehicleAnimGroup`, copias de decision makers, constructor y Clone de las tareas, `GetDefaultTxdSlot` y las funciones del pool, y `TakesPriorityOver` / `AffectsPed` en los bytes altos de EAX); los llamadores directos de las que tienen (0x5543AE solo lee AL; 0x46D560 no usa el retorno; 0x45BB07, 0x45BB18, 0x45CB21 y 0x45CB32 de ScaleAll y 0x47274D, 0x48547E, 0x48CFF2, 0x48F254 y 0x6A9B9A de SetTurnSpeed no leen registros; ver `reversing/PendienteValidar/181A.md` en 5cdf45c8c8) no los leen segun el desensamblado, pero no se ha comprobado llamador por llamador.

## Prueba negativa
`mutate.py` (29 mutaciones por ronda, condicionadas a `this`, puntero del pool, puntero del vehiculo desalineado o a un indice de slot de TXD >= 0x100, algo que el juego real no produce). **Las 29 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-200947-819910` (21226 de 59392 pares con diferencias) y ronda 2 `20261112-201546-107818` (21112 de 59392); el lado EXE coincide con el oraculo en todos los pares. Las mutaciones del cuerpo comun de `Flush` (funciones 24 y 26) y de `TxdPoolFlushBody` afectan a varias funciones a la vez, pero cada ronda tambien muta cada funcion por separado. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas) y sonda buena recompilada (manifiesto regenerado).

## Fuera de los fixtures
`this` nulo o crudo, vehiculos, peatones, eventos y tareas reales construidos por el EXE, punteros crudos desreferenciados por la funcion, jugadores en foco distintos de 0 y 1 (la tabla tiene dos jugadores), segunda llamada de `FindPlayerVehicle` con resultado nulo (el EXE no lo comprueba), valores de retorno de los falsos booleanos con el byte bajo distinto de 0 o 1, indices de `GetDefaultTxdSlot` fuera de [-64, 0x2FF], pools con el cursor de `New` fuera de [-1, capacidad - 1] o capacidades positivas enormes con punteros reales, y la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep de `0x...` en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x731AD0 -> `Txd26` (banco de la clase CTxdStore: **repetir**, el constructor del pool cambio de fuente al engancharse); 0x731B80 -> `FixCore` (CPool<TxdDef>::New: **repetir**). El resto de direcciones solo aparece en `BinaryCoverageAudit` (listado de cobertura, no un banco) y en `RequestedValidation20261008` (lista de peticiones de validacion).
