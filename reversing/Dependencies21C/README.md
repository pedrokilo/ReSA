<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies21C

Banco par a par dentro del juego de `CTaskSimpleFight::FightSetUpCol` (0x61D5F0), `CTaskSimpleFight::FightHitCar` (0x61D0B0) y `CTaskSimpleFight::FightStrike` (0x6240B0): grupo C del encargo 21 (los 21 destinos inciertos de la cohorte de 499).

PREPARADO, sin compilar en el árbol real ni ejecutar: la sonda debe construirla y correrla el coordinador (serie, clon de pruebas). No constituye validación hasta entonces. El código de la DLL que valida está en `gc.patch` (TaskSimpleFight.cpp, TaskGangHassleVehicle.cpp, TaskSimpleUseGun.cpp) más los anexos `gc-x87.cpp.txt` / `gc-x87.h.txt` (final de X87Math.cpp/.h); las filas del catálogo, en `gc-hooks.csv.txt`.

## Qué compara

Cada par llama dos veces a la MISMA entrada del EXE con las mismas semillas: con el hook RH apagado (código original del EXE) y con el hook activo (versión de la DLL), mediante `OriginalDependencies499/HookRoute.hpp`. Se exige `JumpsToDll(entrada) == candidato` en cada llamada.

Por cada llamada se registra y se compara en bits (`original == candidato`, sin tolerancia):

- la traza completa de colaboradores (nombre, orden, argumentos en bits, punteros como (región, desplazamiento) o `stack`);
- las palabras de memoria que cambian en cada objeto del fixture (tarea, atacante, vehículo, ocupantes, matrices, animación, puntos de colisión temporales, columna estática 0xC17824…);
- ESP antes/después, EBX/ESI/EDI (sembrados con valores distintos) y EBP, cabecera SEH (`fs:[0]`);
- x87: TOP y palabra de etiquetas iguales antes y después (pila equilibrada), FCW intacta; MXCSR: bits de control intactos. Los indicadores acumulados (excepciones pegajosas, C0..C3) no son contrato: ningún llamador del EXE los lee y el C++ de la DLL (SSE) no los produce.
- compuertas por llamada (todas deben ser `true` en las dos rutas): `route_ok`, `stack_restored`, `nonvolatile_preserved`, `seh_restored`, `fpu_top_tag_balanced`, `fpu_control_preserved`, `mxcsr_control_preserved`, `guards_intact` (bytes contiguos a las regiones tocadas) e `inputs_intact` (lo que la función solo lee).

EAX/ECX/EDX no se comparan: ningún llamador del EXE los lee tras estas tres rutinas (ver `gc-classification.md`, llamadores 0x6241C9, 0x624577, 0x6640C6, 0x66412C, 0x629EED y 0x62A754: todos recargan el registro antes de usarlo).

## Colaboradores

Falsos con `EntryPatch` en la dirección del EXE y en la entrada de la DLL (`Body()` salta los thunks), con la misma traza:

- FightSetUpCol: `CColSphere::Set` (0x40FD10).
- FightHitCar: `GetStrikeDamage` 0x61C740 (solo EXE; devuelve el float por ST0), `GetAnimHierarchyFromSkinClump` 0x734A40, `RpHAnimIDGetIndex` 0x7C51A0, `RpHAnimHierarchyGetMatrixArray` 0x7C5120, `Fx_c::AddSparks` 0x49F040, `Fx_c::AddPunchImpact` 0x49F670, `VehicleDamage` (vtable +0xE0, falso en una vtable propia que además fija la vida posterior y puede cambiar el arma), `CCrime::ReportCrime` 0x532010, `CEventGroup::Add` 0x4AB420, `CEntity::RegisterReference` 0x571B70 / `CleanUpOldReference` 0x571A00, `CAEPedWeaponAudioEntity::AddAudioEvent` 0x4E69F0 y `CAEPedAudioEntity::AddAudioEvent` 0x4E2BB0. Los eventos temporales (`CEventVehicleDamage` 0x4B18D0 / 0x4B1990) corren de verdad y usan los falsos de referencias.
- FightStrike: `GetWeaponInfo` 0x743C60, `FindPlayerPed` 0x56E210, `BreakGlassPhysically` 0x71CF50, `CPed::IsPlayer` 0x5DF8F0 / `IsAlive` 0x5E0170, `FindObjectsInRange` 0x564A20, `FightSetUpCol` 0x61D5F0, `GetBoundCentre` 0x534250, `AnimatePedColModelSkinnedWorld` 0x4C7170, `AllocateMatrix` 0x54F560, `CSimpleTransform::UpdateMatrix` 0x54F1B0, `GetColModel` 0x535300, `ProcessColModels` 0x4185C0, `FightHitCar` 0x61D0B0, `GetEventGlobalGroup` 0x4ABA50, `CEventGroup::Add` 0x4AB420, `RpAnimBlendClumpGetAssociation` 0x4D68B0, `RegisterReference` / `CleanUpOldReference`, y solo en el EXE `FightHitPed` 0x61CBA0 y `FightHitObj` 0x61D400 (sin reconstruir). Corren de verdad: la copia de `CMatrix`, `SetTranslateOnly`, el destructor, y `CEventSoundQuiet` (constructor/destructor).

## Fixtures y pares (3 x 1536 = 4608)

- FightSetUpCol: 64 radios (40 de estrés: ±0, subnormales, ±1e30, FLT_MAX, ±inf, NaN silenciosos con distinta carga, NaN señalizantes, constantes de la función; el resto normales) x 3 estados de la columna estática (sin preparar, preparada, puntero de datos ajeno) x 8 rellenos aleatorios de la región 0xC17824..0xC178A4.
- FightHitCar: ranura activa (incluida 0xFF = -1, con `movsx`), tipo de arma (motosierra, otras, 0), vida antes/después (estrés, ±1 ulp, igual), masa y daño (estrés y productos que desbordan/underflow en float pero no con el rango extendido del x87), conductor presente/ausente, 0..8 ocupantes con máscara, arma cambiada tras `VehicleDamage`, índice de hueso 0..3 y -1, combo/movimiento (incluye tablas fuera de rango), componente (con la mitad alta basura, como los llamadores del EXE) y superficie (0..255). La tabla de sonidos 0xC16E70..0xC177B0 se rellena con datos deterministas.
- FightStrike: 16 peds, 16 vehículos y 0..16 objetos por caso, con modelos propios en la tabla `ms_modelInfoPtrs` (8 entradas libres que se restauran), columnas con 0..4 esferas, radios de estrés, combos -1..16 (la fila de `m_aComboData` se rellena y se restaura), niveles de golpe 0..255, distancias en torno a los umbrales (0,25x..3x del alcance, 1 ulp por debajo/encima), jugador/no jugador, cristales, evento de sonido y animación del combo 7. Un caso de cada 24 es un escenario dirigido (avance en dos pasadas, animación con/sin ped golpeado y con/sin asociación, golpe a vehículo con/sin matriz, golpe a objeto con matriz nula, sonido, cristales).
- Cobertura exigida (no se relaja): el banco termina en `error` si alguna rama de `requiredCoverage` (33 etiquetas) no se recorre en el candidato.

## Límites declarados

- El valor de un float devuelto en ST0 por un colaborador (`GetStrikeDamage`) pasa por `fld`: un NaN señalizante llega silenciado a las dos rutas, igual que en el EXE.
- La mitad alta del argumento `int16` (componente) llega como basura desde los llamadores del EXE; los falsos registran solo los 16 bits bajos (el destino, `VehicleDamage`/`FightHitObj`, lee 16 bits) y el argumento `int8` de superficie se registra con 8 bits (eSurfaceType es de 8 bits). En el banco FightHitCar el argumento de superficie se pasa limpio (0..255), como los dos llamadores del EXE.
- La copia de `CMatrix` del EXE (0x59BCF0, `rep movsd` de 0x40 bytes) copia también el relleno (+0xC, +0x1C, +0x2C, +0x3C); la de la DLL solo las 12 componentes. El falso de `ProcessColModels` registra las 12 componentes, la posición y los campos +0x40/+0x44; el relleno no lo lee nadie.
- La vtable de los eventos temporales difiere (EXE 0x86D640 / 0x86C2B0 frente a la de la DLL); se identifica por nombre de clase.
- El banco no valida los colaboradores ni su independencia transitiva; solo que estas tres rutinas se comportan igual frente a los mismos colaboradores.

## Ejecución

```
python reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies21C
```
(equivale a `build_probe.py` + `run_local.py`; clon `E:\JUEGOS\GTA San Andreas - CVehicle Test`). Aplicar antes `gc.patch`, anexar `gc-x87.*.txt` a X87Math.cpp/.h y, para no romper los bancos que ya llamaban a estas rutinas, `gc-bank.patch` (HassleVehicle355 y CTaskSimpleUseGun parchean ahora también la entrada de la DLL).

Evidencia estática: `original-disassembly.txt` (desensamblado anotado de las tres funciones con los desplazamientos respecto al ESP de entrada).

## 2026-10-26: FightStrikeDamage y FightHitObj con hook (lotes 65 y 72)
Fallo observado: 12/12 casos de `FightHitCar` con diferencia, porque la traza original llevaba el evento `strike_damage` y la candidata no. Causa (defecto del banco, no de produccion): `FightStrikeDamage` (0x61C740, funcion local `FightStrikeDamageExe` con `RH_ScopedNamedGlobalInstall`) solo estaba sustituida por el falso en la direccion del EXE (`FakeExe`), asi que el candidato ejecutaba el C++ real de la DLL (llamado directamente en TaskSimpleFight.cpp). Cambios en `LiveComparison.hpp`: (1) nuevo `HookBodyByExe(exe)`, que localiza el hook Simple por su direccion del EXE y devuelve `Body(GetHookOurAddress())`; `FakeExe damageFake(0x61C740, ...)` pasa a `Fake damageFake(0x61C740, <cuerpo DLL>, FakeDamage)` (error y fallo del banco si el hook no esta registrado). (2) `FakeExe hitObj(0x61D400, ...)` pasa a `Fake hitObj(0x61D400, Addr(&CTaskSimpleFight::FightHitObj), FakeHitObj)` (FightHitObj, lote 72, thiscall ret 0x18; firma del falso comprobada). Solo quedan solo-EXE los colaboradores sin reconstruir/sin hook (FightHitPed 0x61CBA0). Compuertas y cobertura sin cambios. Sin cambios en produccion.
