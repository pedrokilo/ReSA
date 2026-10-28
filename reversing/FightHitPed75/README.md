<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FightHitPed75: CTaskSimpleFight::FightHitPed 0x61CBA0 (lote 75)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-4`, HEAD 98ff78aaa + `source.patch`). 21808 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y CADA lado igual al oraculo independiente (`fixture_oracle.py`).
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fighthitped75\20261027-150639-057653`.

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `CPed* CTaskSimpleFight::FightHitPed(CPed* creator, CPed* victim, CVector& posn1, CVector& posn2, int16 component)` (antes la envoltura libre `FightHitPedExe`) | 0x61CBA0 | `CTaskSimpleFight` / `FightHitPed` / S (`RH_ScopedInstall`) | `Tasks/TaskTypes/TaskSimpleFight.cpp/.h`, ayudantes en `X87Math.cpp/.h` (bloque «Lote 75») |

Fila de `docs/hooks.csv` (la anade el coordinador): `CTaskSimpleFight,FightHitPed,0x0061CBA0,1,0,1,S`.

## Que hace (desensamblado 0x61CBA0..0x61D0A6, 1289 bytes, thiscall `ret 0x14`, ECX = tarea, EAX = victima o 0)
Unico llamador: 0x6244E5 (`FightStrike`, ya en C++), que solo lee EAX. `posn2` (4.o argumento) no se lee. `component` se lee como `movsx word`.
1. Victima jugador (`IsPlayer`): si la tarea activa de su gestor (`GetActiveTask` + `GetTaskType` virtual, sin comprobar nulo) es la 0xCD, sale con 0.
2. Si la victima pelea (`GetTaskFighting`, dos veces) y `BeHitWhileBlocking` (0x61C650, sin reconstruir) lo da por bloqueado: suena el golpe bloqueado (`AddAudioEvent` del atacante, volumen -9.0f, fila +0x70; con combo 5 y movimiento 0..2 un segundo con el retardo de 0x8D2E3C) y sale con 0.
3. Golpe normal: `GetWeaponInfo` (sin usar), fila de combo fijada ANTES de llamar a `FightStrikeDamage`, danio = `_ftol2` (32 bits bajos) de ST0, `m_nContinueStrike = -1`, `ComputePedShotSide`, `GenerateDamageEvent` (el lado se pasa como dword y se prueba entero: bajo byte 0 no implica lado 0), sonido de motosierra (tipo de arma releido), sonidos de golpe (lado 0: fila +0x5C, combo 5 con retardo, combo 7 movimiento 1 con retardo `_ftol2(fild(tabla) * 2.8f)`; lado distinto de 0: fila +0x70), `Say(0x59)` si el atacante es el jugador, `CEventSoundQuiet(55.0f)` al grupo global.
4. Sangre: probabilidad = 100 (combos 8..12), -1 (combo 4, movimiento 4) o `_ftol2(100.0f - vida)`; tirada = `_ftol2(((rand() & 0xFFFF) * 2^-15) * 100.0f)` con la rand del EXE; si tirada < probabilidad: copia en bruto de `*posn1` (despues de rand), direccion = atacante - victima (x87), `Normalise`, si la victima esta muerta (0,0,2.0f), con combos 8..12 cantidad 16 y, si vive, x1.5f; `Fx_c::AddBlood(posn1, direccion, cantidad, brillo)`. El evento se destruye al final y se devuelve la victima si `GenerateDamageEvent` devolvio verdadero.

## Cambios de produccion (`source.patch`)
- `TaskSimpleFight.cpp/.h`: metodo `CTaskSimpleFight::FightHitPed` en C++ puro con `RH_ScopedInstall(FightHitPed, 0x61CBA0)`; se elimina la envoltura `FightHitPedExe`; `FightStrike` llama al metodo. Colaboradores sin reconstruir por direccion del EXE: `BeHitWhileBlocking` 0x61C650, `CVector::Normalise` 0x59C910, rand 0x821B1E (Fx_c::AddBlood ya era un envoltorio de Fx.cpp).
- `X87Math.cpp/.h`, bloque «Lote 75 (FightHitPed75)» al final: `FightHitPed75FtolCpp`, `LossCpp`, `RollCpp`, `DelayCpp`, `DirectionCpp` (resta x87 en el orden z, y, x) y `ScaleCpp` (x1.5f).

## Banco
`Original` = bytes del EXE con el hook de la funcion apagado; `candidato` = DLL con SOLO ese hook encendido. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran argumentos EN ORDEN y mutan memoria para probar relecturas: combo/movimiento, vida, arma activa, posn1): IsPlayer, GetActiveTask, GetTaskFighting, GetWeaponInfo, FightStrikeDamage, ComputePedShotSide, GenerateDamageEvent, Say, GetEventGlobalGroup, CEventGroup::Add, RegisterReference, CleanUpOldReference, IsAlive (con hook); sin hook (un unico falso en la direccion del EXE): `CAEPedAudioEntity::AddAudioEvent` 0x4E2BB0 y `CAEPedWeaponAudioEntity::AddAudioEvent` 0x4E69F0 (`--unhook=Audio`), BeHitWhileBlocking, Normalise, AddBlood, rand. `GetTaskType` va por la vtable falsa de la tarea activa. `CEventSoundQuiet` corre de verdad en ambos lados.
21808 pares: 16 familias de 1024 (jugador, bloqueo, lado 0/!=0, lado como dword, arma, danio, probabilidad, sangre, posiciones especiales, brillo, mutaciones, resultado de GenerateDamageEvent, mezclas) + 1024 con dos NaN no canonicos en la misma resta + 1024 con un NaN en cada operacion + 2352 de barrido exhaustivo de la tabla de sonidos (combo -8..15 x movimiento -8..40, lado 0 y no 0) + 1024 de bordes de `_ftol2`, del sorteo y del retardo.
Oraculo (`fixture_oracle.py`): traza completa, EAX, bytes cambiados en las 11 arenas; aritmetica x87 a 24 bits con Fracciones (NaN solo como «es NaN»). `run_local.py`: compuertas + minimos de `coverage.json` (50 % de la primera corrida).
NaN: no se emula; los ayudantes usan las mismas instrucciones x87 que el EXE, de modo que la unidad elige el mismo operando. La comparacion bit a bit EXE/DLL (2694 ejes con dos NaN, mixtos QNaN/sNaN, signos y significandos iguales o distintos, en ambos sentidos) sirve de validacion contra el hardware.

## Pruebas
- Prueba negativa (produccion mutada y restaurada; `TaskSimpleFight.cpp` identico al bueno despues): `roll < chance` por `roll <= chance`: **173/21808 diferencias de resultado** (exactamente los casos `roll_equals_chance`), sin caida.
- Hallazgo del banco (no de produccion): la primera corrida fallo solo porque el oraculo no aplicaba la mutacion del combo hecha dentro de `CEventGroup::Add`; corregido en el oraculo. Produccion: sin discrepancias respecto al desensamblado.

## Quien valida a cada colaborador (docs/hooks.csv, reversing/validation-registry.json) y que NO prueba este banco
Este banco no valida la correccion de los colaboradores (cada uno su banco): IsPlayer/IsAlive (CPed), GetActiveTask/GetTaskFighting (tareas), GetWeaponInfo, FightStrikeDamage (`Helpers65D`), ComputePedShotSide, GenerateDamageEvent, Say, eventos y grupo global; las dos AddAudioEvent (`AudioWave2`, `Data23`), BeHitWhileBlocking/Normalise/AddBlood/rand (sin reconstruir). No cubre: dos NaN en una resta con el mismo significando y distinto signo entre unidades distintas, indices de tabla fuera de -8..15 / -8..40, ECX/EDX de salida (solo se registran; FightStrike no los lee).
Banco antiguo adaptado: `Dependencies21C` (FightStrike) ahora lleva el falso de FightHitPed tambien en el cuerpo de la DLL.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh FightHitPed75`; `python -B reversing/FightHitPed75/run_local.py --analyze <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
