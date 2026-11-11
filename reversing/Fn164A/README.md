# Fn164A: 25 funciones diminutas del EXE (lote 164A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-3` desde main 284fb34374 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn164a/20261111-164217-356786` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn164A-20261111-164217-356786`). 25 x 16384 = **409600 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 (vacia, o solo ST0 exacto en las tres funciones que devuelven float) igual y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `c0427d09a0693e309594f0046b1d0d6188071d5bea6c4b68bf4663aa4fc66471`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Ninguna de las 25 tenia hook (codigo, `docs/hooks.csv`, `git worktree list`). Todas son viables en C++ puro; no se salto ninguna. Las clases del listado eran erroneas en tres: 0x6298E0 es el destructor con borrado de `CTaskComplexKillPedFromBoat` (vtable 0x86DA98, cuerpo 0x622830) y 0x629900 el de `CTaskComplexSeekCoverUntilTargetDead` (cuerpo 0x622C20), no de `CTaskSimpleStealthKill`.

| # | Funcion real | Entrada | Bytes | Fuente | Tipo |
|---|---|---|---|---|---|
| 0 | `CBike::OpenDoor` (ranura 27) | 0x6B5820 | 3 | `Entity/Vehicle/Bike.cpp` (+ `.h`) | V |
| 1 | `CBike::GetDooorAngleOpenRatioU32` (29; ST0 = 0.0f) | 0x6B5830 | 9 | idem | V |
| 2 | `CBike::IsOpenTopCar` (39) | 0x6B5840 | 3 | idem | V |
| 3 | `CBike::BlowUpCarCutSceneNoExtras` (42) | 0x6B5850 | 3 | idem | V |
| 4 | `CBike::AddMovingCollisionSpeed` (49) | 0x6B5860 | 17 | idem | V |
| 5 | `CBike::SetTowLink` (61) | 0x6B5880 | 5 | idem | V |
| 6 | `CBike::BreakTowLink` (62) | 0x6B5890 | 3 | idem | V |
| 7 | `CBike::GetNumContactWheels` (55) | 0x6B58A0 | 8 | idem | V |
| 8 | `CBike::GetHeightAboveRoad` (53; ST0 = campo +0x790) | 0x6B58B0 | 7 | idem | V |
| 9 | `CBike::GetRideAnimData` (47) | 0x6B58C0 | 7 | idem | V |
| 10-13 | `CBike::IsDoorReady / FullyOpen / Closed / Missing` (32, 34, 36, 38) | 0x6B58E0, 58F0, 5900, 5910 | 5 | idem | V |
| 14-17 | `CBike::IsDoorReadyU32 / FullyOpenU32 / ClosedU32 / MissingU32` (31, 33, 35, 37) | 0x6B5920, 5930, 5940, 5950 | 5 | idem | V |
| 18 | `CBike::IsComponentPresent` (26; indice sin comprobar) | 0x6B59E0 | 19 | idem | V |
| 19 | `CBike::IsRoomForPedToLeaveCar` (45) | 0x6B7270 | 5 | idem | V |
| 20 | `CBike::FindWheelWidth` (63; ST0 = 0.15f) | 0x6B8940 | 9 | idem | V |
| 21 | `CBike` destructor con borrado (ranura 0) | 0x6B8950 | 30 | idem | S |
| 22 | `CTaskSimpleStealthKill` cuerpo del destructor (marco SEH) | 0x622680 | 109 | `Tasks/TaskTypes/TaskSimpleStealthKill.cpp` | S |
| 23 | `CTaskComplexKillPedFromBoat` destructor con borrado | 0x6298E0 | 30 | `TaskComplexKillPedFromBoat.cpp` | S |
| 24 | `CTaskComplexSeekCoverUntilTargetDead` destructor con borrado | 0x629900 | 30 | `TaskComplexSeekCoverUntilTargetDead.cpp` | S |

V = `RH_ScopedVMTInstall` (fila `0x00000000`); S = `RH_ScopedNamedGlobalInstall`. Filas anadidas a `docs/hooks.csv`.

## Revision contra el EXE
- Ranuras de la vtable 0x871360 leidas del PE. Las siete virtuales que `CBike` no declaraba (`OpenDoor`, `GetDooorAngleOpenRatioU32`, `IsOpenTopCar`, `BlowUpCarCutSceneNoExtras`, `AddMovingCollisionSpeed`, `SetTowLink`, `BreakTowLink`) se anaden como `override`; el resto pasa de inline en `Bike.h` a declarada y definida en `Bike.cpp`. Se anaden `VALIDATE_OFFSET` de +0x5A0, +0x640, +0x790 y +0x804.
- `GetHeightAboveRoad` y `FindWheelWidth` devuelven float en ST0 (`fld dword`); `GetHeightAboveRoad` no tiene argumentos de pila (`ret`). `AddMovingCollisionSpeed` recibe el puntero de salida oculto y lo devuelve en EAX (`ret 8`). `IsComponentPresent` usa `m_aBikeNodes.data()[i]` para no comprobar limites (el EXE no lo hace).
- 0x6B8950 llama al cuerpo 0x6B57A0 (con hook) y a `CVehicle::operator delete` 0x6E2D90 (sin hook) solo con el bit 0 del BYTE de banderas; devuelve `this`. 0x6298E0 / 0x629900: cuerpos 0x622830 / 0x622C20 y `CTask::operator delete` 0x61A5B0.
- 0x622680: `[esi]=0x86DA74`; `CleanUpOldReference` (0x571A00) del objetivo (+0x0C); `SetDeleteCallback` (0x4CEBC0) con `DefaultAnimCB` (0x4D1390) y dato nulo si hay animacion (+0x18); `~CTask` 0x61A3A0. Se reescribio `StealthKillDestructorBody` con esa forma (antes llamaba al destructor generado) y se engancha como S; sigue siendo la funcion que llama el destructor con borrado y que exporta `Lote126AStealthKillDestructorBody`.

## Banco
Una funcion por ruta (original = EXE con hook apagado; candidato = DLL con SOLO ese hook encendido). Arenas propias A, B y C con guardas de 0x40; plantilla `Fn155A` y `Fn141A` (falso de `CVehicle::operator delete`). Falsos identicos en la direccion del EXE y en la DLL: 0x6B57A0, 0x622830, 0x622C20 (hook propio; el falso va en la envoltura de la DLL), 0x61A5B0, 0x6E2D90 (sin hook; funcion propia de la DLL), 0x571A00, 0x4CEBC0, 0x61A3A0.
16 familias de 1024 casos por funcion. `python -I fixture_selfcheck.py` pasa los 409600 casos por el oraculo y mide la cobertura de ramas.
Fuera de los fixtures: `this` nulo o crudo, objetos reales y la exactitud de los colaboradores falseados.

## Prueba negativa
Dos rondas (`mutate.py`, una mutacion por funcion y ronda; `FN164A_CASES=2048`): **las 25 funciones fallan en ambas** (`negative-runs/`): ronda 1 `20261111-153644-628682` con 19780 de 51200 pares con diferencias, ronda 2 `20261111-163609-133296` con 14523; el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas (SHA256 igual a las copias buenas). Copias buenas: `Bike.cpp` 38d6d02d..., `TaskComplexKillPedFromBoat.cpp` 50954a94..., `TaskComplexSeekCoverUntilTargetDead.cpp` ede4dc39..., `TaskSimpleStealthKill.cpp` 3ffd50ca....

## Bancos antiguos que citan estas direcciones (NO repetidos; pase final del coordinador)
`Fn126A` (0x622680: su `run_local.py` lo da por sin hook, hay que adaptarlo), `BikeRoad355` (0x6B58B0), y por colaboradores `Fn125A`, `Fn140A`, `Fn141A`, `Fn152A`, `BikeWave4`, `CTasks10b` (0x6B57A0, 0x6E2D90, 0x622830, 0x622C20).
